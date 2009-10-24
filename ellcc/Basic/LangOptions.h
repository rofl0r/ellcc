//===--- LangOptions.h - C Language Family Language Options -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the LangOptions interface.
//
//===----------------------------------------------------------------------===//

#ifndef LANG_OPTIONS_H
#define LANG_OPTIONS_H

// This type is used for options that nominally either allow or
// disallow some syntax, but can also trigger a warning.  Values of
// this type are intended to be tested like booleans in most places.
namespace ellcc {

enum bool3 {
  b3_FALSE = 0,      // syntax not allowed
  b3_TRUE = 1,       // accepted silently
  b3_WARN = 2,       // accept with a warning
};

/// LangOptions - This class keeps track of the various options that can be
/// enabled, which controls the dialect of C that is accepted.
class LangOptions {
public:
  // when this is true, and the parser sees "struct Foo { ... }",
  // it will pretend it also saw "typedef struct Foo Foo;" -- i.e.,
  // the structure (or class) tag name is treated as a type name
  // by itself
  bool tagsAreTypes;

  // when true, recognize C++ keywords in input stream
  bool recognizeCppKeywords;

  // when true, every function body gets an implicit
  //   static char const __func__[] = "function-name";
  // declaration just inside the opening brace, where function-name is
  // the name of the function; this is a C99 feature (section 6.4.2.2)
  bool implicitFuncVariable;

  // behavior of gcc __FUNCTION__ and __PRETTY_FUNCTION__
  // see also
  //   http://gcc.gnu.org/onlinedocs/gcc-3.4.1/gcc/Function-Names.html
  //   http://gcc.gnu.org/onlinedocs/gcc-2.95.3/gcc_4.html#SEC101
  enum GCCFuncBehavior {
    GFB_none,              // ordinary symbols
    GFB_string,            // string literal (they concatenate!)
    GFB_variable,          // variables, like __func__
  } gccFuncBehavior;

  // when true, and we see a class declaration inside something,
  // pretend it was at toplevel scope anyway; this also applies to
  // enums, enumerators and typedefs
  //
  // dsw: I find that having boolean variables that are in the
  // negative sense is usually a mistake.  I would reverse the sense
  // of this one.
  //
  // sm: The 'no' is a little misleading.  In the 'false' case,
  // syntax reflects semantics naturally; only in the 'true' case
  // is something unusual going on.  A positive-sense name might be
  // the unwieldy 'turnApparentlyInnerClassesIntoOuterClasses'.
  bool noInnerClasses;

  // when true, an uninitialized global data object is typechecked as
  // a common symbol ("C" in the nm(1) manpage) instead of a bss
  // symbol ("B").  This means that the following is not an error:
  //   int a; int a;
  // gcc seems to operate as if this is true, whereas g++ not.
  //
  // these are the so-called "tentative" definitions of C; the flag
  // is somewhat misnamed
  bool uninitializedGlobalDataIsCommon;

  // when true, if a function has an empty parameter list then it is
  // treated as supplying no parameter information (C99 6.7.5.3 para 14)
  bool emptyParamsMeansNoInfo;

  // when true, require all array sizes to be positive; when false,
  // 0-length arrays are allowed as class/struct fields
  //
  // dsw: UPDATE: allow them anywhere; needed for linux kernel
  bool strictArraySizeRequirements;

  // when true, assume arrays with no size are of size 1 and issue a
  // warning
  //
  // TODO: This is not the proper way to handle C's rules for arrays.
  // See C99 6.9.2p2, 6.9.2e5, 6.7p7 and 6.7p16.  What we have now
  // is just a hack for the sake of expedience.
  bool assumeNoSizeArrayHasSizeOne;

  // when true, we allow overloaded function declarations (same name,
  // different signature)
  bool allowOverloading;

  // when true, to every compound type add the name of the type itself
  bool compoundSelfName;

  // when true, allow a function call to a function that has never
  // been declared, implicitly declaring the function in the global
  // scope; this is for C89 (and earlier) support
  bool3 allowImplicitFunctionDecls;

  // when true, allow function definitions that omit any return type
  // to implicitly return 'int'.
  bool allowImplicitInt;

  // GNU extension: when true, allow local variable arrays to have
  // sizes that are not constant
  bool allowDynamicallySizedArrays;

  // GCC extension: when true, you can say things like 'enum Foo;' and
  // it declares that an enum called Foo will be defined later
  bool allowIncompleteEnums;

  // C language, and GNU extension for C++: allow a class to have a
  // member that has the same name as the class
  bool allowMemberWithClassName;

  // every C++ compiler I have does overload resolution of operator=
  // differently from what is specified in the standard; this flag
  // causes Elsa to do the same
  bool nonstandardAssignmentOperator;

  // permit prototypes to have mismatching exception specs if the
  // function is extern "C" (TODO: provide more documentation)
  bool allowExternCThrowMismatch;

  // allow main() to be declared/defined with an implicit 'int'
  bool allowImplicitIntForMain;

  // when true, "_Bool" is a built-in type keyword (C99)
  bool predefined_Bool;

  // dsw: when true, a function definition with 'extern' and 'inline'
  // keywords is handled specially.  How exactly is a function of the
  // optimization conditions; these are not language conditions so
  // they are handed by a tracing flag rather than by a language flag.
  // The tracing flag is 'handleExternInline-asPrototype'.  If true,
  // then we simply ignore the body of an extern inline.  When false
  // we handle extern inlines as weak static inlines: the 'extern
  // inline' is converted to 'static inline' and if another definition
  // in the translation unit is found, it replaces that of the extern
  // inline.  These two modes seem to reflect the behavior of gcc
  // 3.4.6 when optimizations are off and on respectively.
  bool handleExternInlineSpecially;

  // quarl: whether "inline" implies static linkage.  True in C++ but not in
  // C.
  bool inlineImpliesStaticLinkage;

  // dsw: C99 std 6.4.5p5: "For character string literals, the array
  // elements have type char...."; Cppstd 2.13.4p1: "An ordinary
  // string literal has type "array of const char" and static storage
  // duration"; But empirical results show that even in C++, gcc makes
  // string literals arrays of (nonconst) chars.
  bool stringLitCharsAreConst;

  // if the argument to a cast is an lvalue, make the cast expression
  // have lvalue type
  bool lvalueFlowsThroughCast;

  // when true, 'restrict' is a keyword (note that __restrict and
  // __restrict__ are always keywords)
  bool restrictIsAKeyword;

  // ---- bug compatibility flags ----
  // gcc-2 bug compatibility: permit string literals to contain
  // (unescaped) newline characters in them
  bool allowNewlinesInStringLits;

  // MSVC bug compatibility: allow implicit int for operator functions
  bool3 allowImplicitIntForOperators;

  // gcc bug compatibility: allow qualified member declarations
  bool3 allowQualifiedMemberDeclarations;

  // gcc bug compatibility: allow typedef names to combine with
  // certain type keywords, e.g., "u32 long", in/gnu/dC0014.c;
  // eventually, once the client codes have been fixed, it would be
  // good to delete this, since it involves some extra grammar
  // productions
  bool3 allowModifiersWithTypedefNames;

  // gcc/msvc bug/extension compatibility: allow anonymous structs;
  // see doc/anon-structs.txt
  bool3 allowAnonymousStructs;

  // gcc-2 bug compatibility: In gcc-2, namespace "std::" is actually
  // an alias for the global scope.  This flag turns on some hacks
  // to accept some code preprocessed with gcc-2 headers.
  bool gcc2StdEqualsGlobalHacks;

  // more gcc-2 bug compat: The gcc-2 headers contain some invalid
  // syntax.  Conceptually, this flag recognizes the invalid syntax
  // and transforms it into valid syntax for Elsa.  Actually, it just
  // enables some hacks that have similar effect.
  bool3 allowGcc2HeaderSyntax;

  // gcc C-mode bug compat: accept duplicate type specifier keywords
  // like 'int int'
  bool3 allowRepeatedTypeSpecifierKeywords;

  // gcc C-mode bug compat: silently allow const/volatile to be
  // applied to function types via typedefs; it's meaningless
  bool3 allowCVAppliedToFunctionTypes;

  // gcc bug compat: gcc does not enforce the rule that a definition
  // must be in a scope that encloses the declaration
  bool3 allowDefinitionsInWrongScopes;

  // gcc bug compat: in C++ mode, gcc allows prototype parameters to
  // have the same name (in/gnu/bugs/gb0011.cc)
  bool3 allowDuplicateParameterNames;

  // gcc bug compat: gcc does not require "template <>" is some
  // cases for explicit specializations (in/gnu/bugs/gb0012.cc)
  bool3 allowExplicitSpecWithoutParams;

  // quarl: declaring (or defining) a function as static after previously
  // declaring it without 'static'. gcc-3.4 allows with warning; gcc-4.0
  // disallows.
  bool3 allowStaticAfterNonStatic;

  bool Trigraphs;         // Trigraphs in source files.
  bool BCPLComment;       // BCPL-style '//' comments.
  bool DollarIdents;      // '$' allowed in identifiers.
  bool AsmPreprocessor;   // Preprocessor in asm mode.
  bool GNUMode;           // True in gnu99 mode false in c99 mode (etc)
  bool Digraphs;          // C94, C99 and C++
  bool HexFloats;         // C99 Hexadecimal float constants.
  bool C99;               // C99 Support
  bool CPlusPlus;         // C++ Support
  bool CPlusPlus0x;       // C++0x Support
  bool NoExtensions;      // All extensions are disabled, strict mode.
  bool CXXOperatorNames;  // Treat C++ operator names as keywords.
    
  bool Boolean;           // Allow bool/true/false
  bool WritableStrings;   // Allow writable strings
  bool LaxVectorConversions;
  bool Exceptions;        // Support exception handling.

  bool Freestanding;      // Freestanding implementation
  bool NoBuiltin;         // Do not use builtin functions (-fno-builtin)

  bool ThreadsafeStatics; // Whether static initializers are protected by locks.
  bool Blocks;            // block extension to C
  bool EmitAllDecls;      // Emit all declarations, even if they are unused.
  bool MathErrno;         // Math functions must respect errno (modulo the platform support).

  bool OverflowChecking;  // Extension to call a handler function when signed integer arithmetic overflows.

  bool HeinousExtensions; // Extensions that we really don't like and may be ripped out at any time.

  bool Optimize;          // Whether __OPTIMIZE__ should be defined.
  bool OptimizeSize;      // Whether __OPTIMIZE_SIZE__ should be 
                                  // defined.
  bool Static;            // Should __STATIC__ be defined (as
                                  // opposed to __DYNAMIC__).
  unsigned PICLevel;      // The value for __PIC__, if non-zero.

  bool GNUInline;         // Should GNU inline semantics be used (instead of C99 semantics).
  bool NoInline;          // Should __NO_INLINE__ be defined.

  bool AccessControl;     // Whether C++ access control should be enabled.

  bool Microsoft;         // Microsoft bug compatability.
private:
  /// The user provided name for the "main file", if non-null. This is
  /// useful in situations where the input file name does not match
  /// the original input file, for example with -save-temps.
  const char *MainFileName;
  void setAllWarnings(bool enable);

public:  
  unsigned InstantiationDepth;    // Maximum template instantiation depth.

  LangOptions() {
    ANSI_C89();
  }
  
  // set any b3_TRUE to b3_WARN
  void enableAllWarnings() { setAllWarnings(true); }

  // set any b3_WARN to b3_TRUE
  void disableAllWarnings() { setAllWarnings(false); }

  // the following are additive incremental

  // enable gcc C features
  void GNU_C_extensions();

  // enable C99 features
  void ANSI_C99_extensions();

  // enable MSVC bug compatibility
  void MSVC_bug_compatibility();

  // The predefined settings below are something of a best-effort at
  // reasonable starting configurations.  Every function below sets
  // *all* of the flags; they are not incremental.  Users are
  // encouraged to explicitly set fields after activating a predefined
  // setting to get a specific setting.

  void KandR_C();           // settings for K&R C
  void ANSI_C89();          // settings for ANSI C89
  void ANSI_C99();          // settings for ANSI C99
  void GNU_C99();           // settings for GNU C
  void GNU3_KandR_C();      // GNU 3.xx C + K&R compatibility
  void GNU2_KandR_C();      // GNU 2.xx C + K&R compatibility
  void ANSI_Cplusplus98();  // settings for ANSI C++ 98
  void GNU_Cplusplus98();   // settings for GNU C++ 98
  void ANSI_Cplusplus0x();  // settings for ANSI C++ 0x
  void GNU_Cplusplus0x();   // settings for GNU C++ 0x

  const char *getMainFileName() const { return MainFileName; }
  void setMainFileName(const char *Name) { MainFileName = Name; }
};

}  // end namespace ellcc

#endif
