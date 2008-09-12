// str.h            see license.txt for copyright and terms of use
// a string class
// the representation uses just one char*, so that a smart compiler
//   can pass the entire object as a single word
// Scott McPeak, 1995-2000  This file is public domain.

// 2005-03-01: See string.txt.  The plan is to evolve the class
// towards compatibility with std::string, such that eventually
// they will be interchangeable.  So far I have converted only
// the most problematic constructs, those involving construction,
// conversion, and internal pointers.

#ifndef STR_H
#define STR_H

#include "typ.h"         // bool
#include <iostream>      // istream, ostream
#include <stdarg.h>      // va_list
#include <string.h>      // strcmp, etc.

class Flatten;           // flatten.h

// ------------------------- string ---------------------
namespace sm {
  class string {
  public:
    typedef int size_type;
  protected:     // data
    // 10/12/00: switching to never letting s be NULL
    char *s;     	       	       	       // string contents; never NULL
    static char * const emptyString;     // a global ""; should never be modified

  protected:     // funcs
    void dup(char const *source);        // copies, doesn't dealloc first
    void kill();                         // dealloc if str != 0

  public:	       // funcs
    string(sm::string const &src) { dup(src.s); }
    string(char const *src) { dup(src); }
    string() { s=emptyString; }
    ~string() { kill(); }

    // for this one, use ::substring instead
    string(char const *src, int length);

    // for this one, there are two alternatives:
    //   - stringBuilder has nearly the same constructor interface
    //     as string had, but cannot export a char* for writing
    //     (for the same reason string can't anymore); operator[] must
    //     be used
    //   - Array<char> is very flexible, but remember to add 1 to
    //     the length passed to its constructor!
    string(int length) { s=emptyString; setlength(length); }

    string(Flatten&);
    void xfer(Flatten &flat);

    // simple queries
    int length() const;  	       	// returns number of non-null chars in the string; length of "" is 0
    bool isempty() const { return s[0]==0; }
    bool contains(char c) const;

    // std::string has this instead; I will begin using slowly
    bool empty() const { return isempty(); }

    // array-like access
    char& operator[] (int i) { return s[i]; }
    char operator[] (int i) const { return s[i]; }

    // substring
    sm::string substring(int startIndex, int length) const;

    // conversions
#if 0    // removing these for more standard compliace
    //operator char* () { return s; }      // ambiguities...
    operator char const* () const { return s; }
    char *pchar() { return s; }
    char const *pcharc() const { return s; }
#else
    char const *c_str() const { return s; }
#endif

    // assignment
    sm::string& operator=(sm::string const &src)
    { if (&src != this) { kill(); dup(src.s); } return *this; }
    sm::string& operator=(char const *src)
    { if (src != s) { kill(); dup(src); } return *this; }

    // allocate 'newlen' + 1 bytes (for null); initial contents is ""
    sm::string& setlength(int newlen);

    // comparison; return value has same meaning as strcmp's return value:
    //   <0   if   *this < src
    //   0    if   *this == src
    //   >0   if   *this > src
    int compareTo(sm::string const &src) const;
    int compareTo(char const *src) const;
    bool equals(char const *src) const { return compareTo(src) == 0; }
    bool equals(sm::string const &src) const { return compareTo(src) == 0; }

#define MAKEOP(op)                                                      \
    bool operator op (string const &src) const { return compareTo(src) op 0; } \
    /*bool operator op (const char *src) const { return compareTo(src) op 0; }*/ \
    /* killed stuff with char* because compilers are too flaky; use compareTo */
    MAKEOP(==)  MAKEOP(!=)
    MAKEOP(>=)  MAKEOP(>)
    MAKEOP(<=)  MAKEOP(<)
#undef MAKEOP

    // concatenation (properly handles string growth)
    // uses '&' instead of '+' to avoid char* coercion problems
    sm::string operator& (sm::string const &tail) const;
    sm::string& operator&= (sm::string const &tail);

    // input/output
    friend std::istream& operator>> (std::istream &is, sm::string &obj)
    { obj.readline(is); return is; }
    friend std::ostream& operator<< (std::ostream &os, sm::string const &obj)
    { obj.write(os); return os; }

    // note: the read* functions are currently implemented in a fairly
    // inefficient manner (one char at a time)

    void readdelim(std::istream &is, char const *delim);
    // read from is until any character in delim is encountered; consumes that
    // character, but does not put it into the string; if delim is null or
    // empty, reads until EOF

    void readall(std::istream &is) { readdelim(is, NULL); }
    // read all remaining chars of is into this

    void readline(std::istream &is) { readdelim(is, "\n"); }
    // read a line from input stream; consumes the \n, but doesn't put it into
    // the string

    void write(std::ostream &os) const;
    // writes all stored characters (but not '\0')

    // debugging
    void selfCheck() const;
    // fail an assertion if there is a problem
  };
};

// tglek: for compatability while code is being rewritten
// to use sm::string
//using namespace sm;
// ------------------------ rostring ----------------------
// My plan is to use this in places I currently use 'char const *'.
typedef sm::string const &rostring;

// I have the modest hope that the transition to 'rostring' might be
// reversible, so this function converts to 'char const *' but with a
// syntax that could just as easily apply to 'char const *' itself
// (and in that case would be the identity function).
inline char const *toCStr(rostring s) { return s.c_str(); }

// at the moment, if I do this it is a mistake, so catch it; this
// function is not implemented anywhere
void/*unusable*/ toCStr(char const *s);

// I need some compatibility functions
inline int strlen(rostring s) { return s.length(); }

inline std::istream &getline(std::istream &in, sm::string &line) { line.readline(in); return in; }

int strcmp(rostring s1, rostring s2);
int strcmp(rostring s1, char const *s2);
int strcmp(char const *s1, rostring s2);
// string.h, above, provides:
// int strcmp(char const *s1, char const *s2);

// dsw: this is what we are asking most of the time so let's special
// case it
inline bool streq(rostring s1, rostring s2)       {return strcmp(s1, s2) == 0;}
inline bool streq(rostring s1, char const *s2)    {return strcmp(s1, s2) == 0;}
inline bool streq(char const *s1, rostring s2)    {return strcmp(s1, s2) == 0;}
inline bool streq(char const *s1, char const *s2) {return strcmp(s1, s2) == 0;}

char const *strstr(rostring haystack, char const *needle);

// there is no wrapper for 'strchr'; use string::contains

int atoi(rostring s);

// construct a string out of characters from 'p' up to 'p+n-1',
// inclusive; resulting string length is 'n'
sm::string substring(char const *p, int n);
inline sm::string substring(rostring p, int n)
  { return substring(p.c_str(), n); }


// --------------------- stringBuilder --------------------
// this class is specifically for appending lots of things
class stringBuilder : public sm::string {
protected:
  enum { EXTRA_SPACE = 30 };    // extra space allocated in some situations
  char *end;          // current end of the string (points to the NUL character)
  int size;           // amount of space (in bytes) allocated starting at 's'

protected:
  void init(int initSize);
  void dup(char const *src);

public:
  stringBuilder(int length=0);    // creates an empty string
  stringBuilder(char const *str);
  stringBuilder(char const *str, int length);
  stringBuilder(sm::string const &str) { dup(str.c_str()); }
  stringBuilder(stringBuilder const &obj) { dup(obj.c_str()); }
  ~stringBuilder() {}

  stringBuilder& operator= (char const *src);
  stringBuilder& operator= (sm::string const &s) { return operator= (s.c_str()); }
  stringBuilder& operator= (stringBuilder const &s) { return operator= (s.c_str()); }

  int length() const { return end-s; }
  bool isempty() const { return length()==0; }

  // unlike 'string' above, I will allow stringBuilder to convert to
  // char const * so I can continue to use 'stringc' to build strings
  // for functions that accept char const *; this should not conflict
  // with std::string, since I am explicitly using a different class
  // (namely stringBuilder) when I use this functionality
  operator char const * () const { return c_str(); }

  stringBuilder& setlength(int newlen);    // change length, forget current data

  // make sure we can store 'someLength' non-null chars; grow if necessary
  void ensure(int someLength) { if (someLength >= size) { grow(someLength); } }

  // std::string compatibility name for ensure()
  void reserve(int someLength) { ensure(someLength); }

  // grow the string's length (retaining data); make sure it can hold at least
  // 'newMinLength' non-null chars
  void grow(int newMinLength);

  // this can be useful if you modify the string contents directly..
  // it's not really the intent of this class, though
  void adjustend(char* newend);

  // remove characters from the end of the string; 'newLength' must
  // be at least 0, and less than or equal to current length
  void truncate(int newLength);

  // make the string be the empty string, but don't change the
  // allocated space
  void clear() { adjustend(s); }

  // concatenation, which is the purpose of this class
  stringBuilder& operator&= (char const *tail);

  // useful for appending substrings or strings with NUL in them
  void append(char const *tail, int length);

  // append a given number of spaces; meant for contexts where we're
  // building a multi-line string; returns '*this'
  stringBuilder& indent(int amt);

  // sort of a mixture of Java compositing and C++ i/o strstream
  stringBuilder& operator << (rostring text) { return operator&=(text.c_str()); }
  stringBuilder& operator << (char const *text) { return operator&=(text); }
  stringBuilder& operator << (char c);
  stringBuilder& operator << (unsigned char c) { return operator<<((char)c); }
  stringBuilder& operator << (long i);
  stringBuilder& operator << (unsigned long i);
  stringBuilder& operator << (int i) { return operator<<((long)i); }
  stringBuilder& operator << (unsigned i) { return operator<<((unsigned long)i); }
  stringBuilder& operator << (short i) { return operator<<((long)i); }
  stringBuilder& operator << (unsigned short i) { return operator<<((long)i); }
  stringBuilder& operator << (double d);
  stringBuilder& operator << (void *ptr);     // inserts address in hex
  #ifndef LACKS_BOOL
    stringBuilder& operator << (bool b) { return operator<<((long)b); }
  #endif // LACKS_BOOL

  // useful in places where long << expressions make it hard to
  // know when arguments will be evaluated, but order does matter
  typedef stringBuilder& (*Manipulator)(stringBuilder &sb);
  stringBuilder& operator<< (Manipulator manip);

  // work around problems invoking non-const non-member funcs
  // on temporaries
  stringBuilder &myself() { return *this; }

  // stream readers
  friend std::istream& operator>> (std::istream &is, stringBuilder &sb)
    { sb.readline(is); return is; }
  void readall(std::istream &is) { readdelim(is, NULL); }
  void readline(std::istream &is) { readdelim(is, "\n"); }

  void readdelim(std::istream &is, char const *delim);

  // an experiment: hex formatting (something I've sometimes done by resorting
  // to sprintf in the past)
  class Hex {
  public:
    unsigned long value;

    Hex(unsigned long v) : value(v) {}
    Hex(Hex const &obj) : value(obj.value) {}
  };
  stringBuilder& operator<< (Hex const &h);
  #define SBHex stringBuilder::Hex
};


// ---------------------- misc utils ------------------------
// the real strength of this entire module: construct strings in-place
// using the same syntax as C++ iostreams.  e.g.:
//   puts(stringb("x=" << x << ", y=" << y));
#define stringb(expr) (stringBuilder().myself() << expr)

// experimenting with dropping the () in favor of <<
// (the "c" can be interpreted as "constructor", or maybe just
// the successor to "b" above)
#define stringc (stringBuilder().myself())


// experimenting with using toString as a general method for datatypes
sm::string toString(int i);
sm::string toString(unsigned i);
sm::string toString(char c);
sm::string toString(long i);
sm::string toString(char const *str);
sm::string toString(float f);


// printf-like construction of a string; often very convenient, since
// you can use any of the formatting characters (like %X) that your
// libc's sprintf knows about
sm::string stringf(char const *format, ...);
sm::string vstringf(char const *format, va_list args);


#endif // STR_H
