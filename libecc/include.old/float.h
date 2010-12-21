/*
 * float.h
 *
 * Floating point definitions.
 */

#ifndef _FLOAT_H_
#define _FLOAT_H_

/** Decimal digits.
 * The number of decimal digits, N, such that any floating point number in the
 * widest supported floating type can rounded to a floating point number with
 * N decimal digits and back again without change in value.
 * __DECIMAL_DIG__ is defined by the compiler.
 */
#define DECIMAL_DIG __DECIMAL_DIG__
/** The floating point evaluation method.
 *     -1       indeterminate
 *      0       evaluate all operations and constants just to the range and prcision of the type
 *      1       evaluate operations and constants of float and double type to the precision of double,
 *              evaluate operations and constants of long double type to the precision of long double
 *      2       evaluate all operations and constants to the range and prcision of the long double type
 * __FLT_EVAL_METHOD__ is defined by the compiler.
 */
#define FLT_EVAL_METHOD __FLT_EVAL_METHOD__
/** The radix of the floating point representation.
 * __FLT_RADIX__ is defined by the compiler.
 */
#define FLT_RADIX __FLT_RADIX__
/** The floating point rounding method.
 *     -1       indeterminate
 *      0       toward zero
 *      1       to nearest
 *      2       toward positive infinity
 *      3       toward negative infinity
 * __FLT_ROUNDS__ is defined by the compiler.
 */
#define FLT_ROUNDS __FLT_ROUNDS__
/* The number of base-FLT_RADIX digits in the float floating point mantissa.
 * __FLT_MANT_DIG__ is defined by the compiler.
 */
#define FLT_MANT_DIG __FLT_MANT_DIG__
/* The number of base-FLT_RADIX digits in the double floating point mantissa.
 * __DBL_MANT_DIG__ is defined by the compiler.
 */
#define DBL_MANT_DIG __DBL_MANT_DIG__
/* The number of base-FLT_RADIX digits in the long double floating point mantissa.
 * __LDBL_MANT_DIG__ is defined by the compiler.
 */
#define LDBL_MANT_DIG __LDBL_MANT_DIG__
/** Floaing point type digits.
 * The number of decimal digits, N, such that any floating point number in the
 * float type can rounded to a floating point number with
 * N decimal digits and back again without change in value.
 * __FLT_DIG__ is defined by the compiler.
 */
#define FLT_DIG __FLT_DIG__
/** Floaing point type digits.
 * The number of decimal digits, N, such that any floating point number in the
 * double type can rounded to a floating point number with
 * N decimal digits and back again without change in value.
 * __DBL_DIG__ is defined by the compiler.
 */
#define DBL_DIG __DBL_DIG__
/** Floaing point type digits.
 * The number of decimal digits, N, such that any floating point number in the
 * long double type can rounded to a floating point number with
 * N decimal digits and back again without change in value.
 * __LDBL_DIG__ is defined by the compiler.
 */
#define LDBL_DIG __LDBL_DIG__

#define DBL_MAX_EXP __DBL_MAX_EXP__
#define DBL_MAX_10_EXP __DBL_MAX_10_EXP__
#endif

