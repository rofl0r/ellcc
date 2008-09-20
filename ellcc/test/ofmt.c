#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define double float

#if INT_MAX == LONG_MAX
/** Longs are the same as ints.
 */
#undef LONGS
#else
/** Longs are different than ints.
 */
#define LONGS
#endif

#define	INITPREC 6		///< number + (opt sign)
#define MAXLEN 100		///< Maximum length of any float, int, etc.

// Definitions for format flags.
#define	LEFTJUST	0x001	///< Left justify output.
#define	ALTERNATE	0x002	///< Use "#" alternate form.
#define	SHORTFLAG	0x004	///< Is this a short argument?
#define	LONGFLAG	0x008	///< Is this a long argument?
#define	LONGLONGFLAG	0x010	///< Is this a long long argument?
#define	SIGNFLAG	0x020	///< Must we output a sign?
#define	LEADINGZERO	0x040	///< Output a leading zero.
#define	PRECISIONFLAG	0x080	///< User specified precision.
#define	NOTRAILZERO	0x100	///< Don't output trailing zero's.

// Definitions for floating point format flags.
#define	FLOATFLAG	0x001	///< Are we in a float format?
#define	DECIMALPOINT	0x002	///< Should we output a decimalpoint?
#define	EXPSIGN		0x004	///< Sign of exponent 0:+, 1:-.
#define	EXPONENT	0x008	///< Exponent: yes or no.
#define	ETYPE		0x010	///< "e" format conversion in toASCII.
#define	EUPPER		0x020	///< 'e' is uppercase in "e" or "g" format.

/** Convert floating point to ASCII.
 * @param value The value to convert.
 * @param ndigits The number of digits to place in buffer.
 * @param decpt The place to return position of decimal point.
 * @param sign The place to return sign of number.
 * @param flag Defined above.
 * @param bufsize The size of 'buf'.
 * @param buf The array in which to place result.
 */
static void toASCII(double value, unsigned int ndigits, int* decpt,
                    int* sign, int flag, int bufsize, char *buf)
{
    int exp10;                                  // Base 10 exponent of number.
    // Values to add for rounding.
    static const double roundvals[] =
    {
        5.0e0, 5.0e-1, 5.0e-2, 5.0e-3, 5.0e-4, 5.0e-5,
        5.0e-6, 5.0e-7, 5.0e-8, 5.0e-9, 5.0e-10, 5.0e-11,
        5.0e-12, 5.0e-13, 5.0e-14, 5.0e-15, 5.0e-16, 5.0e-17,
    };
#define MAX (sizeof roundvals / sizeof *roundvals - 1)

    // Set sign.
    if (value < 0) {
        value = -value;
        *sign = 1;
    } else {
        *sign = 0;
    }

    // Scale the number so it is in the range 1 to 10.
    exp10 = 0;
    while (value >= 10) {
        value /= 10;
        ++exp10;
    }

    while (value < 1) {
        value *= 10;
        --exp10;
    }

    /* If 'f' format, we want 'ndigits' past the decimal point.
     * If 'e' format, we want ndigits + 1 digit before the decimal place.
     * If 'g' format, we trim off the trailing zeros.
     */
    if (!(flag & ETYPE)) {
        ndigits += exp10 + 1;
    } else {
        ++ndigits;
    }

    // Round the number to the specified number of digits.
    value += roundvals[ndigits > MAX ? MAX : ndigits];

    // It might need more scaling after rounding.
    if (value >= 10) {
        value = 1;
        ++exp10;
        if (!(flag & ETYPE)) {
            ++ndigits;
        }
    }

    // Set 'decpt'.
    *decpt = exp10 + 1;

    char *bufend = &buf[bufsize] - 1;
    int c;
    char *lastnonzero = buf + 1;                // Allow one zero.

    for (unsigned int i = 0; i < ndigits; ++i) {
        if (buf >= bufend) {
            break;
        }

        if (i > 17) {
            *buf++ = '0';
        } else {
            c = (int)value;			// Convert to integer.

            *buf++ = '0' + c;
            if (c) {
                lastnonzero = buf;              // Remember one past.
            }
            value = (value - c) * 10;
        }
    }

    if (flag & NOTRAILZERO) {
        buf = lastnonzero;
    }

    *buf = '\0';
}

/** Send a character to the output stream.
 */
static int putChar(FILE* fp, char** line, int value, int* count)
{
    int ch = (unsigned char)value;
    *count++;
    if (fp == NULL) {
        if (*line) {
            *(*line++) = ch;
        }
    } else {
	ch = putc(ch, fp);
    }

    return ch;
}

/** Format arguments according to the specified format string.
 */
int _vformat(FILE* fp, char* line, const char *format, va_list ap)
{
    int status = 0;				// Output status.
    int flags;                                  // Flags for output formatting.
    int fflags;                                 // Flags for float formatting.
    char work[MAXLEN];                          // Maximum character length or any float, int, etc.
    char *workptr;                              // Temp pointer used to move through work[].
    char c;                                     // Current character in format string.
    int base;                                   // Base for numerical conversions.
    char sign = ' ';                            // (+,-, ,\0) used when SIGNFLAG is set.
    int width;                                  // %3: format width: default 0.
    int precision;                              // %.3: format precision: default -1.
    int count = 0;                          	// Number of characters output.
    char pad;                                   // Character to pad fields with.
    int totalLength;                            // Number of spaces to pad determined in each case.
    int numchars = 0;                           // number of characters in 'number'.
    int decpt = 0;                              // Double decimal point.
    long exponent;                              // Represents the exponent value.
#ifdef  LONGS
    long temp = 0;				// long temporary.
    unsigned long utemp = 0;
#else
    int temp = 0;				// int temporary.
    unsigned int utemp = 0;
#endif
    long long lltemp = 0;			// long long temporary.
    unsigned long long ulltemp = 0;
    double ftemp;                               // Double temporary.
    int exponentBegin = 0;                      // Beginning index into "work[]" for the exponent.
    int negative;

    while ((c = *format)) {
        if (c != '%') {
            // If no conversion specifier just send character.
            status = putChar(fp, &line, c, &count);
	    if (status < 0) {
	        return status;
	    }
            ++format;
            continue;                           // And try again.
        }

        workptr = work;                         // Set up work buffer pointer.
        fflags = flags = 0;                     // Initialize all flag bits to 0.
        precision = -1;
        width = 0;
        pad = ' ';                              // Default padding character.
    incloop:
        ++format;                               // Skip format character.
    loop:
        switch (c = *format) {
        case ' ':
            flags |= SIGNFLAG;
            sign = ' ';
            goto incloop;
        case '#':
            flags |= ALTERNATE;
            goto incloop;
        case '-':
            flags |= LEFTJUST;
            goto incloop;
        case '+':
            flags |= SIGNFLAG;
            sign = '+';
            goto incloop;
        case '%':
            // Double format characters.
            status = putChar(fp, &line, c, &count);
	    if (status < 0) {
	        return status;
	    }
            ++format;
            break;
        case '0':
            // Set pad character to zero.
            pad = '0';
            goto incloop;
        case 'L':
        case 'l':
            if (flags & LONGFLAG) {
                flags |= LONGLONGFLAG;
            }
            flags |= LONGFLAG;
            goto incloop;
        case 'h':
            flags |= SHORTFLAG;
            goto incloop;
        case '*':
            // Get the width.
            width = (va_arg(ap, int));
            if (*(++format) != '.') {
                goto loop;
            }
            ++format;
            if (isdigit(*format)) {
                goto numprecision;
            }
            if (*format != '*') {
                goto loop;
            }

        argprecision:
            flags |= PRECISIONFLAG;
            precision = va_arg(ap, int);
            goto incloop;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            while (isdigit(*format)) {
                width = 10 * width + (*(format)++ - '0');
            }
            if (*format == '.') {
                // Get the maximum width allowed.
                ++format;                       // Skip the '.'.
                if (*format == '*') {
                    goto argprecision;
                } else {
                numprecision:
                    flags |= PRECISIONFLAG;
                    precision = 0;
                    while (isdigit(*format)) {
                        precision = 10 * precision + (*(format)++ - '0');
                    }
                }
            }
            goto loop;

        case 'G':
            fflags |= EUPPER;
            // Fall through.
        case 'g':
            fflags |= NOTRAILZERO;
            ftemp = va_arg(ap, double);
            toASCII(ftemp, precision, &decpt, &negative, fflags, sizeof(work), workptr);
            if (abs(decpt) <= 4) {
                goto float_common;
            }
            goto set_e_flags;

        case 'E':
            fflags |= EUPPER;
            // Fall through.
        case 'e':
            // Set the E format flag and other float flags.
            ftemp = va_arg(ap, double);
        set_e_flags:
            fflags |= EXPONENT | ETYPE;
            goto float_common;

        case 'F':
        case 'f':
            ftemp = va_arg(ap, double);
        float_common:
            fflags |= FLOATFLAG;
            exponent = 0;
            workptr = work;
            if (precision == -1)
            {
                precision = INITPREC;
            }

            toASCII(ftemp, precision, &decpt, &negative, fflags, sizeof(work), workptr);
            if (precision != 0) {
                fflags |= DECIMALPOINT;
            }

            if (   (fflags & NOTRAILZERO)
                   && (decpt >= 0 && decpt <= precision)
                   && (work[decpt] == '\0')) {
                fflags &= ~DECIMALPOINT;
            }

            if (negative) {
                flags |= SIGNFLAG;
                sign = '-';
            }

            if (decpt < 0) {
                flags |= LEADINGZERO;
            } else {
                flags &= ~LEADINGZERO;
            }

            if (fflags & EXPONENT) {
                if (decpt <= 0) {
                    decpt = -decpt;
                    exponent = decpt + 1;
                    fflags |= EXPSIGN;
                } else {
                    exponent = decpt - 1;
                }

                decpt = 1;
                if (!work[1]) {
                    fflags &= ~DECIMALPOINT;
                }
                workptr[MAXLEN - 1] = '\0';
                if (exponent < 10) {
                    workptr[MAXLEN - 2] = exponent + '0';
                    workptr[MAXLEN - 3] = '0';
                    exponent = 4;
                    exponentBegin = MAXLEN - 3;
                } else if (exponent < 100) {
                    workptr[MAXLEN - 2] = exponent % 10 + '0';
                    workptr[MAXLEN - 3] = (exponent / 10) % 10 + '0';
                    exponent = 4;
                    exponentBegin = MAXLEN - 3;
                } else {
                    char *s = workptr + MAXLEN - 1;
                    for (int tmp = 0 ; tmp < 3 ; ++tmp) {
                        *--s = exponent % 10 + '0';
                        exponent /= 10;
                    }
                    exponent = 5;
                    exponentBegin = MAXLEN - 4;
                }
            }

            if (decpt > 0) {
                totalLength = decpt;
            } else {
                totalLength = 1;
            }
            totalLength = precision + totalLength +
                          ((fflags & DECIMALPOINT) != 0) + ((flags & SIGNFLAG) != 0);
            totalLength += exponent;
            goto output;

        case 'N':
        case 'n':
            if (flags & SHORTFLAG) {
                *(va_arg(ap, short *)) = count;
            } else if (flags & LONGLONGFLAG) {
                *(va_arg(ap, long long *)) = count;
#ifdef LONGS
            } else if (flags & LONGFLAG) {
                *(va_arg(ap, long *)) = count;
#endif
            } else {
                *(va_arg(ap, int *)) = count;
            }
            continue;

        case 'I':
        case 'i':
        case 'D':
        case 'd':
            // Convert signed 'short', 'int', 'long', or 'long long' to decimal.
            if (flags & LONGLONGFLAG) {
                lltemp = va_arg(ap, long long);
#ifdef LONGS
            } else if (flags & LONGFLAG) {
                temp = va_arg(ap, long);
#endif
            } else {
                temp = va_arg(ap, int);
            }

            if (flags & LONGLONGFLAG) {
                if (lltemp < 0) {
                    flags |= SIGNFLAG;
                    sign = '-';
                    lltemp = -lltemp;
                }
                ulltemp = lltemp;
            } else {
                if (temp < 0) {
                    flags |= SIGNFLAG;
                    sign = '-';
                    temp = -temp;
                }
                utemp = temp;
            }

            base = 10;
            goto convert;

        case 'U':
        case 'u':
            // Convert unsigned 'short', 'int', 'long', or 'long long' to decimal.
            if (flags & LONGLONGFLAG) {
                ulltemp = va_arg(ap, unsigned long long);
#ifdef LONGS
            } else if (flags & LONGFLAG) {
                utemp = va_arg(ap, unsigned long);
#endif
            } else {
                utemp = va_arg(ap, unsigned);
            }

            // Clear signflag.
            flags &= ~SIGNFLAG;
            base = 10;
            goto convert;

        case 'P':
        case 'p':
        case 'X':
        case 'x':
            // RICH: Pointer size.
            // Convert to hexadecimal instead of decimal.
            if (flags & LONGLONGFLAG) {
                ulltemp = va_arg(ap, unsigned long long);
#ifdef LONGS
            } else if (flags & LONGFLAG) {
                utemp = va_arg(ap, unsigned long);
#endif
            } else {
                utemp = va_arg(ap, unsigned);
            }

            // Clear signflag.
            flags &= ~SIGNFLAG;
            base = 16;
            goto convert;

        case 'O':
        case 'o':
            // Convert to octal instead of decimal.
            if (flags & LONGLONGFLAG) {
                ulltemp = va_arg(ap, unsigned long long);
#ifdef LONGS
            } else if (flags & LONGFLAG) {
                utemp = va_arg(ap, unsigned long);
#endif
            } else {
                utemp = va_arg(ap, unsigned);
            }

            // Clear signflag.
            flags &= ~SIGNFLAG;
            base = 8;

            // Convert the value to the given base and advance all the pointers.
        convert: ;
            int val;
            char *v, *x, y, letter;

            // No alternate for zero.
            if (   ((flags & LONGLONGFLAG) && !ulltemp)
                || (!(flags & LONGLONGFLAG) && !utemp)) {
                flags &= ~ALTERNATE;
            }

            numchars = 1;
            letter = (c == 'X' ? 'A' : 'a');
            if (flags & LONGLONGFLAG) {
                val = ulltemp % base;
                *workptr++ = val < 10 ? val + '0' : val + letter - 10;

                // Convert the string backwards and then reverse it.
                while ((ulltemp = ulltemp / base)) {
                    val = ulltemp % base;	// Get the lowest digit.
                    *workptr++ = (val < 10 ? val + '0' : val + letter - 10);
                    ++numchars;
                }
            } else {
                val = utemp % base;
                *workptr++ = val < 10 ? val + '0' : val + letter - 10;

                // Convert the string backwards and then reverse it.
                while ((utemp = utemp / base)) {
                    val = utemp % base;		// Get the lowest digit.
                    *workptr++ = (val < 10 ? val + '0' : val + letter - 10);
                    ++numchars;
                }
            }

            // Set precision if not set.
            if (!(flags & PRECISIONFLAG) || precision < numchars) {
                precision = numchars;
            }

            // If alternate is set, use 'c' to determine which prefix to output.
            if (flags & ALTERNATE) {
                switch (c) {
                case 'x':
                    *workptr++ = 'x';
                    goto addthe0;

                case 'X':
                    *workptr++ = 'X';
                    addthe0:
                    ++numchars;
                    ++precision;

                case 'o':
                case 'O':
                    *workptr++ = '0';
                    ++numchars;
                    ++precision;
                    break;
                }
            }

            *workptr = '\0';                    // Terminate the work buffer.

            // Reverse the string.
            for (v = work,  x = v + numchars; v < x; ) {
                y = *v;
                *v++ = *--x;
                *x = y;
            }

            flags &= ~LEADINGZERO;
            fflags &= ~(DECIMALPOINT | EXPONENT);

            if (precision && !width) {
                width = precision;
            } else if (numchars <= precision && width <= precision) {
                pad = '0';
            }

            totalLength = numchars + ((flags & SIGNFLAG) != 0);
            goto output;

        case 'C':
        case 'c':
            // Send a character.
            *workptr++ = va_arg(ap, int);
            *workptr = '\0';                    // Terminate the work buffer.
            numchars = totalLength = precision = 1;
            goto output;

        case 'S':
        case 's':
            // Send a string.
            workptr = va_arg(ap, char *);
            numchars = totalLength = strlen(workptr);
            if ((flags & PRECISIONFLAG) && precision < totalLength) {
                totalLength = precision;
            } else {
                precision = totalLength;
            }

            if (totalLength > width) {
                numchars = totalLength;
                width =  totalLength;
            }

            // Fall into output, but don't overwrite "workptr".
            goto output1;

        output:
            workptr = work;                     // Point to work buffer.
        output1:
            // Send out leading padding character.
            if (pad == '0' && (flags & SIGNFLAG)) {
                // Send sign before leading zeros.
                flags &= ~SIGNFLAG;
                status = putChar(fp, &line, sign, &count);
	        if (status < 0) {
	            return status;
	        }
            }

            if (!(flags & LEFTJUST)) {
                int i;

                if (fflags & FLOATFLAG) {
                    width += 1;
                }
                for (i = width - (totalLength < precision ? precision : totalLength); i > 0 ; --i) {
                    status = putChar(fp, &line, pad, &count);
	            if (status < 0) {
	                return status;
	            }
                }

                for (i = precision - totalLength; i > 0 ; i--) {
                    status = putChar(fp, &line, '0', &count);
	            if (status < 0) {
	                return status;
	            }
                }
            }

            // Send out leading padding character if not already sent above.
            if (flags & SIGNFLAG) {
                status = putChar(fp, &line, sign, &count);
	        if (status < 0) {
	            return status;
	        }
            }

            // Transfer the work buffer to the output.
            if (fflags & FLOATFLAG) {
                if (decpt > 0) {
                    do {
                        if (*workptr) {
                            status = putChar(fp, &line, *workptr++, &count);
	                    if (status < 0) {
	                        return status;
	                    }
                        } else {
                            status = putChar(fp, &line, '0', &count);
	                    if (status < 0) {
	                        return status;
	                    }
                        }
                    } while (--decpt > 0);
                } else {
                    status = putChar(fp, &line, '0', &count);
	            if (status < 0) {
	                return status;
	            }
                }

                if (fflags & DECIMALPOINT) {
                    status = putChar(fp, &line, '.', &count);
	            if (status < 0) {
	                return status;
	            }
                }

                while (decpt < 0 && precision > 0) {
                    status = putChar(fp, &line, '0', &count);
	            if (status < 0) {
	                return status;
	            }
                    ++decpt;
                    --precision;
                }

                while (*workptr && precision > 0) {
                    status = putChar(fp, &line, *workptr++, &count);
	            if (status < 0) {
	                return status;
	            }
                    --precision;
                }

                if (fflags & EXPONENT) {
                    status = putChar(fp, &line, (fflags & EUPPER) ? 'E' : 'e', &count);
	            if (status < 0) {
	                return status;
	            }
                    if (fflags & EXPSIGN) {
                        status = putChar(fp, &line, '-', &count);
	                if (status < 0) {
	                    return status;
	                }
                    } else {
                        status = putChar(fp, &line, '+', &count);
	                if (status < 0) {
	                    return status;
	                }
                    }

                    int i;
                    for (char *s = &work[exponentBegin];  (i = *s); ++s) {
                        status = putChar(fp, &line, i, &count);
	                if (status < 0) {
	                    return status;
	                }
                    }
                }
            } else {
                int i;
                if (!precision || numchars < precision) {
                    i = numchars;
                } else {
                    i = precision;
                }

                for ( ; i; --i) {
                    status = putChar(fp, &line, *workptr++, &count);
	            if (status < 0) {
	                return status;
	            }
                }
            }

            // If left justified, pad on the right.
            if (flags & LEFTJUST) {
                if (fflags & FLOATFLAG) {
                    width += 1;
                }
                for (int i = width - totalLength; i > 0; i--) {
                    // Here always pad with spaces.
                    status = putChar(fp, &line, ' ', &count);
	            if (status < 0) {
	                return status;
	            }
                }
            }
            ++format;
            break;

        default:
            /* If the character after the '%' wasn't a nul then add it to the print buffer and
             * increment 'format'
             */
            if (*format) {
                status = putChar(fp, &line, *format++, &count);
	        if (status < 0) {
	            return status;
	        }
            }
        }
    }

    if (fp == NULL && line) {
        // Teminate the string but don't count the nul byte.
        *line++ = '\0';
    }

    return count;
}

/** Format arguments according to the specified format string.
 */
int _format(FILE* fp, char* line, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int status = _vformat(fp, line, fmt, ap);
    va_end(ap);
    return status;
}

/** Print arguments using the formatted string.
 */
int printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int status = _vformat(stdout, NULL, fmt, ap);
    va_end(ap);
    return status;
}
