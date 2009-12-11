// cc_flags.h            see license.txt for copyright and terms of use
// enumerated flags for parsing C, C++

// Basically, this module is a set of enums that are used by at
// least two different modules in the C/C++ front end (if they
// were only used by one module, they'd be declared in that
// module instead).  It's intended to be a lightweight module,
// dependent on almost nothing else.
//
// Each enum has a 'toString' that yields a printable representation.
// If it yields 'char const *', then it's guaranteed to be a valid
// pointer with unchanging contents throughout execution.
//
// Those that are OR-able together have the necessary operators
// declared (ENUM_BITWISE_OPS), and their 'toString' uses bitwise-OR
// syntax.

#ifndef CC_FLAGS_H
#define CC_FLAGS_H

#include "str.h"     // string
#include "macros.h"  // ENUM_BITWISE_OPS
#include "TargetInfo.h"

// ----------------------- TypeIntr ----------------------
// type introducer keyword
// NOTE: keep consistent with CompoundType::Keyword (cc_type.h)
enum TypeIntr {
  TI_STRUCT,
  TI_CLASS,
  TI_UNION,
  TI_ENUM,
  NUM_TYPEINTRS
};

extern char const * const typeIntrNames[NUM_TYPEINTRS];    // "struct", ...
char const *toString(TypeIntr tr);
char const *toXml(TypeIntr tr);
void fromXml(TypeIntr &out, char const *str);


// --------------------- CVFlags ---------------------
// set: which of "const" and/or "volatile" is specified;
// I leave the lower 8 bits to represent SimpleTypeId, so I can
// freely OR them together during parsing;
// values in common with UberModifier must line up
enum CVFlags {
  CV_NONE     = 0x0000,
  CV_CONST    = 0x0400,
  CV_VOLATILE = 0x0800,
  CV_RESTRICT = 0x1000,     // C99
  CV_ALL      = 0x2C00,

  CV_SHIFT_AMOUNT = 10,     // shift right this many bits before counting for cvFlagNames
  NUM_CVFLAGS = 4,          // # bits set to 1 in CV_ALL

  // See TypeSpecifier::setCVOnce.  This isn't a flag like the others,
  // it is a bit used to ensure the flags are manipulated safely.  So,
  // it is not counted in the NUM_CVFLAGS, nor subject to the shift.
  CV_UNLOCKED = 0x0001
};

extern char const * const cvFlagNames[NUM_CVFLAGS];      // 0="const", 1="volatile", 2="owner"
sm::string toString(CVFlags cv);
sm::string toXml(CVFlags cv);
void fromXml(CVFlags &out, char const *str);

ENUM_BITWISE_OPS(CVFlags, CV_ALL)

// experiment: superset operator
inline bool operator>= (CVFlags cv1, CVFlags cv2)
  { return (cv1 & cv2) == cv2; }


// ----------------------- DeclFlags ----------------------
// These flags tell what keywords were attached to a variable when it
// was declared.  They also reflect classifications of various kinds;
// in particular, they distinguish the various roles that Variables
// can play (variable, type, enumerator, etc.).  This can be used as
// a convenient way to toss a new boolean into Variable, though it's
// close to using all 32 bits, so it might need to be split.
//
// NOTE: Changes to this enumeration must be accompanied by
// updates to 'declFlagNames' in cc_flags.cc.
enum DeclFlags {
  DF_NONE        = 0x00000000,

  // syntactic declaration modifiers (UberModifiers)
  DF_AUTO        = 0x00000001,
  DF_REGISTER    = 0x00000002,
  DF_STATIC      = 0x00000004,
  DF_EXTERN      = 0x00000008,
  DF_MUTABLE     = 0x00000010,
  DF_INLINE      = 0x00000020,
  DF_VIRTUAL     = 0x00000040,
  DF_EXPLICIT    = 0x00000080,
  DF_FRIEND      = 0x00000100,
  DF_TYPEDEF     = 0x00000200,

  DF_NAMESPACE   = 0x00000400,    // names of namespaces
  DF_SOURCEFLAGS = 0x000007FF,    // all flags that come from keywords in the source

  // semantic flags on Variables
  DF_GLOBAL      = 0x00000800,    // set for globals, unset for locals
  DF_INITIALIZED = 0x00001000,    // true if has been declared with an initializer (or, for functions, with code)
  DF_BUILTIN     = 0x00002000,    // true for e.g. __builtin_constant_p -- don't emit later
  DF_ENUMERATOR  = 0x00004000,    // true for values in an 'enum' (enumerators in the terminology of the C++ standard)
  DF_REFERENCED  = 0x00008000,    // true if it's been referenced
  DF_PARAMETER   = 0x00010000,    // true if this is a function parameter or a handler "parameter"
  DF_UNUSED1     = 0x00020000,    // Unused
  DF_UNUSED2     = 0x00040000,    // Unused
  DF_MEMBER      = 0x00080000,    // true for members of classes (data, static data, functions); *not* true for namespace members
  DF_DEFINITION  = 0x00100000,    // set once we've seen this Variable's definition
  DF_INLINE_DEFN = 0x00200000,    // set for inline function definitions on second pass of tcheck
  DF_IMPLICIT    = 0x00400000,    // set for C++ implicit typedefs (if also DF_TYPEDEF),
                                  // and implicit compiler-supplied member decls (if not DF_TYPEDEF)
  DF_FORWARD     = 0x00800000,    // for syntax which only provides a forward declaration
  DF_TEMPORARY   = 0x01000000,    // temporary variable introduced by elaboration
  DF_EXTERN_C    = 0x02000000,    // name is marked extern "C"
  DF_SELFNAME    = 0x04000000,    // section 9 para 2: name of class inside its own scope
  DF_BOUND_TPARAM= 0x08000000,    // template parameter bound to a concrete argument
  DF_TEMPL_PARAM = 0x10000000,    // template parameter; bound iff also DF_BOUND_TPARAM
  DF_USING_ALIAS = 0x20000000,    // this is a 'using' alias
  DF_BITFIELD    = 0x40000000,    // this is a bitfield
  DF_GNU_EXTERN_INLINE            // dsw: was extern inline (record since might be changed to static inline)
                 = 0x80000000,

  ALL_DECLFLAGS  = 0xFFFFFFFF,
  NUM_DECLFLAGS  = 32             // # bits set to 1 in ALL_DECLFLAGS
};

extern char const * const declFlagNames[NUM_DECLFLAGS];      // 0="inline", 1="virtual", 2="friend", ..
sm::string toString(DeclFlags df);
sm::string toXml(DeclFlags df);
void fromXml(DeclFlags &out, char const *str);


ENUM_BITWISE_OPS(DeclFlags, ALL_DECLFLAGS)

inline bool operator>= (DeclFlags df1, DeclFlags df2)
  { return (df1 & df2) == df2; }

// helper of possibly general purpose
sm::string bitmapString(int bitmap, char const * const *names,
                    int numflags, char const *delim);


// -------------------------- ScopeKind ------------------------------
// What kind of (innermost) scope does a variable declaration appear in?
enum ScopeKind {
  SK_UNKNOWN,                     // hasn't been registered in a scope yet
  SK_GLOBAL,                      // toplevel names
  SK_PARAMETER,                   // parameter list
  SK_FUNCTION,                    // includes local variables
  SK_CLASS,                       // class member scope
  SK_TEMPLATE_PARAMS,             // template paramter list (inside the '<' and '>')
  SK_TEMPLATE_ARGS,               // bound template arguments, during instantiation
  SK_NAMESPACE,                   // namespace
  NUM_SCOPEKINDS
};

char const *toString(ScopeKind sk);


// ------------------------- SimpleTypeId ----------------------------
// C's built-in scalar types; the representation deliberately does
// *not* imply any orthogonality of properties (like long vs signed);
// separate query functions can determine such properties, or signal
// when it is meaningless to query a given property of a given type
// (like whether a floating-point type is unsigned)
enum SimpleTypeId {
  // types that exist in C++
  ST_NO_TYPE = ellcc::TargetInfo::NoType,
  ST_BOOL = ellcc::TargetInfo::Bool,
  ST_CHAR = ellcc::TargetInfo::Char,
  ST_SIGNED_CHAR = ellcc::TargetInfo::SignedChar,
  ST_UNSIGNED_CHAR = ellcc::TargetInfo::UnsignedChar,
  ST_WCHAR_T = ellcc::TargetInfo::WChar,
  ST_SHORT_INT = ellcc::TargetInfo::Short,
  ST_UNSIGNED_SHORT_INT = ellcc::TargetInfo::UnsignedShort,
  ST_INT = ellcc::TargetInfo::Int,
  ST_UNSIGNED_INT = ellcc::TargetInfo::UnsignedInt,
  ST_LONG_INT = ellcc::TargetInfo::Long,
  ST_UNSIGNED_LONG_INT = ellcc::TargetInfo::UnsignedLong,
  ST_LONG_LONG = ellcc::TargetInfo::LongLong,
  ST_UNSIGNED_LONG_LONG = ellcc::TargetInfo::UnsignedLongLong,         // GNU/C99
  ST_FLOAT = ellcc::TargetInfo::Float,
  ST_DOUBLE = ellcc::TargetInfo::Double,
  ST_LONG_DOUBLE = ellcc::TargetInfo::LongDouble,
  ST_FLOAT_COMPLEX = ellcc::TargetInfo::FloatComplex,                  // GNU/C99 (see doc/complex.txt)
  ST_DOUBLE_COMPLEX = ellcc::TargetInfo::DoubleComplex,                // GNU/C99
  ST_LONG_DOUBLE_COMPLEX = ellcc::TargetInfo::LongDoubleComplex,       // GNU/C99
  ST_FLOAT_IMAGINARY = ellcc::TargetInfo::FloatImaginary,              // C99
  ST_DOUBLE_IMAGINARY = ellcc::TargetInfo::DoubleImaginary,            // C99
  ST_LONG_DOUBLE_IMAGINARY = ellcc::TargetInfo::LongDoubleImaginary,   // C99
  ST_VOID = ellcc::TargetInfo::Void,                   // last concrete type (see 'isConcreteSimpleType')

  // codes I use as a kind of implementation hack
  ST_ELLIPSIS = ellcc::TargetInfo::TypeIDCount, // used to encode vararg functions
  ST_CDTOR,                                     // "return type" for ctors and dtors
  ST_ERROR,                                     // this type is returned for typechecking errors
  ST_DEPENDENT,                                 // depdenent on an uninstantiated template parameter type
  ST_IMPLINT,                                   // implicit-int for K&R C
  ST_NOTFOUND,                                  // delayed ST_ERROR

  // for polymorphic built-in operators (cppstd 13.6)
  ST_PROMOTED_INTEGRAL,                         // int,uint,long,ulong
  ST_PROMOTED_ARITHMETIC,                       // promoted integral + float,double,longdouble
  ST_INTEGRAL,                                  // has STF_INTEGER
  ST_ARITHMETIC,                                // every simple type except void
  ST_ARITHMETIC_NON_BOOL,                       // every simple type except void & bool
  ST_ANY_OBJ_TYPE,                              // any object (non-function, non-void) type
  ST_ANY_NON_VOID,                              // any type except void
  ST_ANY_TYPE,                                  // any type, including functions and void

  // for polymorphic builtin *return* ("PRET") type algorithms
  ST_PRET_STRIP_REF,                            // strip reference and volatileness from 1st arg
  ST_PRET_PTM,                                  // ptr-to-member: union CVs, 2nd arg atType
  ST_PRET_ARITH_CONV,                           // "usual arithmetic conversions" (5 para 9) on 1st, 2nd arg
  ST_PRET_FIRST,                                // 1st arg type
  ST_PRET_FIRST_PTR2REF,                        // 1st arg ptr type -> ref type
  ST_PRET_SECOND,                               // 2nd arg type
  ST_PRET_SECOND_PTR2REF,                       // 2nd arg ptr type -> ref type

  NUM_SIMPLE_TYPES,
  ST_BITMASK = 0xFF                             // for extraction for OR with CVFlags
};

// some flags that can be set for simple types
enum SimpleTypeFlags {
  STF_NONE       = 0x00,
  STF_INTEGER    = 0x01,     // "integral type" (3.9.1 para 7)
  STF_FLOAT      = 0x02,     // "floating point type" (3.9.1 para 8)
  STF_PROM       = 0x04,     // can be destination of a promotion
  STF_UNSIGNED   = 0x08,     // explicitly unsigned type
  STF_ALL        = 0x0F,
};
//ENUM_BITWISE_OPS(SimpleTypeFlags, STF_ALL)   // wondering about problems with initializers..

// info about each simple type
struct SimpleTypeInfo {
  char const *name;       // e.g. "unsigned char"
  SimpleTypeFlags flags;  // various boolean attributes
};

bool isValid(SimpleTypeId id);                          // bounds check
SimpleTypeInfo const &simpleTypeInfo(SimpleTypeId id);

inline char const *simpleTypeName(SimpleTypeId id)
    { if (id < (SimpleTypeId)ellcc::TargetInfo::TypeIDCount)
          return ellcc::TargetInfo::getTypeName((ellcc::TargetInfo::TypeID)id);
      else return simpleTypeInfo(id).name; }
inline int simpleTypeSizeInBits(ellcc::TargetInfo &TI, SimpleTypeId id)
    { if (id < (SimpleTypeId)ellcc::TargetInfo::TypeIDCount)
          return TI.getTypeSizeInBits((ellcc::TargetInfo::TypeID)id);
      else return 0; }
inline int simpleTypeSizeInBytes(ellcc::TargetInfo &TI, SimpleTypeId id)
    { if (id < (SimpleTypeId)ellcc::TargetInfo::TypeIDCount)
          return TI.getTypeSizeInBytes((ellcc::TargetInfo::TypeID)id);
      else return 0; }
inline int simpleTypeAlignInBits(ellcc::TargetInfo &TI, SimpleTypeId id)
    { if (id < (SimpleTypeId)ellcc::TargetInfo::TypeIDCount)
          return TI.getTypeAlignInBits((ellcc::TargetInfo::TypeID)id);
      else return 0; }
inline int simpleTypeAlignInBytes(ellcc::TargetInfo &TI, SimpleTypeId id)
    { if (id < (SimpleTypeId)ellcc::TargetInfo::TypeIDCount)
          return TI.getTypeAlignInBytes((ellcc::TargetInfo::TypeID)id);
      else return 0; }
inline bool isIntegerType(SimpleTypeId id)
    { return !!(simpleTypeInfo(id).flags & STF_INTEGER); }
inline bool isFloatType(SimpleTypeId id)
    { return !!(simpleTypeInfo(id).flags & STF_FLOAT); }
inline bool isExplicitlyUnsigned(SimpleTypeId id)
    { return !!(simpleTypeInfo(id).flags & STF_UNSIGNED); }

inline bool isArithmeticType(SimpleTypeId id)    // 3.9.1 para 8
  { return !!(simpleTypeInfo(id).flags & (STF_FLOAT | STF_INTEGER)); }

// true if this is not one of the polymorphic types, impl. hacks, etc.
inline bool isConcreteSimpleType(SimpleTypeId id)
  { return id <= ST_VOID; }

bool isComplexOrImaginary(SimpleTypeId id);

inline char const *toString(SimpleTypeId id)        { return simpleTypeName(id); }
char const *toXml(SimpleTypeId id);
void fromXml(SimpleTypeId &out, char const *str);


// ---------------------------- UnaryOp ---------------------------
enum UnaryOp {
  UNY_PLUS,      // +
  UNY_MINUS,     // -
  UNY_NOT,       // !
  UNY_BITNOT,    // ~
  NUM_UNARYOPS
};

inline bool validCode(UnaryOp op)
  { return (unsigned)op < NUM_UNARYOPS; }

extern char const * const unaryOpNames[NUM_UNARYOPS];     // "+", ...
char const *toString(UnaryOp op);
char const *toXml(UnaryOp op);
void fromXml(UnaryOp &out, char const *str);


// ------------------------- EffectOp -------------------------
// unary operator with a side effect
enum EffectOp {
  EFF_POSTINC,   // ++ (postfix)
  EFF_POSTDEC,   // -- (postfix)
  EFF_PREINC,    // ++
  EFF_PREDEC,    // --
  NUM_EFFECTOPS
};

inline bool validCode(EffectOp op)
  { return (unsigned)op < NUM_EFFECTOPS; }

extern char const * const effectOpNames[NUM_EFFECTOPS];   // "++", ...
char const *toString(EffectOp op);
char const *toXml(EffectOp op);
void fromXml(EffectOp &out, char const *str);
bool isPostfix(EffectOp op);
inline bool isPrefix(EffectOp op) { return !isPostfix(op); }


// ------------------------ BinaryOp --------------------------
enum BinaryOp {
  // the relationals come first, and in this order, to correspond
  // to RelationOp in predicate.ast (which is now in another
  // repository entirely...)
  BIN_EQUAL,     // ==
  BIN_NOTEQUAL,  // !=
  BIN_LESS,      // <
  BIN_GREATER,   // >
  BIN_LESSEQ,    // <=
  BIN_GREATEREQ, // >=

  BIN_MULT,      // *
  BIN_DIV,       // /
  BIN_MOD,       // %
  BIN_PLUS,      // +
  BIN_MINUS,     // -
  BIN_LSHIFT,    // <<
  BIN_RSHIFT,    // >>
  BIN_BITAND,    // &
  BIN_BITXOR,    // ^
  BIN_BITOR,     // |
  BIN_AND,       // &&
  BIN_OR,        // ||
  BIN_COMMA,     // ,

  // gcc extensions
  BIN_MINIMUM,   // <?
  BIN_MAXIMUM,   // >?

  // this exists only between parsing and typechecking
  BIN_BRACKETS,  // []

  BIN_ASSIGN,    // = (used to denote simple assignments in AST, as opposed to (say) "+=")

  // C++ operators
  BIN_DOT_STAR,    // .*
  BIN_ARROW_STAR,  // ->*

  // theorem prover extension
  BIN_IMPLIES,     // ==>
  BIN_EQUIVALENT,  // <==>

  NUM_BINARYOPS
};

inline bool validCode(BinaryOp op)
  { return (unsigned)op < NUM_BINARYOPS; }

extern char const * const binaryOpNames[NUM_BINARYOPS];   // "*", ..
char const *toString(BinaryOp op);
char const *toXml(BinaryOp op);
void fromXml(BinaryOp &out, char const *str);

bool isPredicateCombinator(BinaryOp op);     // &&, ||, ==>, <==>
bool isRelational(BinaryOp op);              // == thru >=
bool isInequality(BinaryOp op);              // <, >, <=, >=
bool isOverloadable(BinaryOp op);


// ---------------- access control ------------
// these are listed from least restrictive to most restrictive,
// so < can be used to test for retriction level
enum AccessKeyword {
  AK_PUBLIC,
  AK_PROTECTED,
  AK_PRIVATE,
  AK_UNSPECIFIED,      // not explicitly specified; typechecking changes it later

  NUM_ACCESS_KEYWORDS
};

extern char const * const accessKeywordNames[NUM_ACCESS_KEYWORDS];
char const *toString(AccessKeyword key);
char const *toXml(AccessKeyword key);
void fromXml(AccessKeyword &out, char const *str);

// ---------------- cast keywords -------------
enum CastKeyword {
  CK_DYNAMIC,
  CK_STATIC,
  CK_REINTERPRET,
  CK_CONST,

  NUM_CAST_KEYWORDS
};

extern char const * const castKeywordNames[NUM_CAST_KEYWORDS];
char const *toString(CastKeyword key);
char const *toXml(CastKeyword key);
void fromXml(CastKeyword &out, char const *str);


// --------------- overloadable operators -------------
// This is all of the unary and binary operators that are overloadable
// in C++.  While it repeats operators that are also declared above in
// some form, it makes the design more orthogonal: the operators above
// are for use in *expressions*, while these are for use in operator
// *names*, and there is not a 1-1 correspondence.  In a few places,
// I've deliberately used different names (e.g. OP_STAR) than the name
// of the operator above, to emphasize that there's less intrinsic
// meaning to the operator names here.
enum OverloadableOp {
  // unary only
  OP_NOT,          // !
  OP_BITNOT,       // ~

  // unary, both prefix and postfix; latter is declared as 2-arg function
  OP_PLUSPLUS,     // ++
  OP_MINUSMINUS,   // --

  // unary or binary
  OP_PLUS,         // +
  OP_MINUS,        // -
  OP_STAR,         // *
  OP_AMPERSAND,    // &

  // arithmetic
  OP_DIV,          // /
  OP_MOD,          // %
  OP_LSHIFT,       // <<
  OP_RSHIFT,       // >>
  OP_BITXOR,       // ^
  OP_BITOR,        // |

  // arithmetic+assignment
  OP_ASSIGN,       // =
  OP_PLUSEQ,       // +=
  OP_MINUSEQ,      // -=
  OP_MULTEQ,       // *=
  OP_DIVEQ,        // /=
  OP_MODEQ,        // %=
  OP_LSHIFTEQ,     // <<=
  OP_RSHIFTEQ,     // >>=
  OP_BITANDEQ,     // &=
  OP_BITXOREQ,     // ^=
  OP_BITOREQ,      // |=

  // comparison
  OP_EQUAL,        // ==
  OP_NOTEQUAL,     // !=
  OP_LESS,         // <
  OP_GREATER,      // >
  OP_LESSEQ,       // <=
  OP_GREATEREQ,    // >=

  // logical
  OP_AND,          // &&
  OP_OR,           // ||

  // arrows
  OP_ARROW,        // ->
  OP_ARROW_STAR,   // ->*

  // misc
  OP_BRACKETS,     // []
  OP_PARENS,       // ()
  OP_COMMA,        // ,
  OP_QUESTION,     // ?:  (not overloadable, but resolution used nonetheless)

  // gcc extensions
  OP_MINIMUM,      // <?
  OP_MAXIMUM,      // >?

  NUM_OVERLOADABLE_OPS
};

inline bool validCode(OverloadableOp op)
  { return (unsigned)op < NUM_OVERLOADABLE_OPS; }

extern char const * const overloadableOpNames[NUM_OVERLOADABLE_OPS];    // "!", ...
char const *toString(OverloadableOp op);
char const *toXml(OverloadableOp op);
void fromXml(OverloadableOp &out, char const *str);

// yields things like "operator+"
extern char const * const operatorFunctionNames[NUM_OVERLOADABLE_OPS];

// map from the other operator sets into the operator name that
// would be used to overload it
OverloadableOp toOverloadableOp(UnaryOp op);
OverloadableOp toOverloadableOp(EffectOp op);
OverloadableOp toOverloadableOp(BinaryOp op, bool isAssignment=false);


// -------------------- uber modifiers -----------------
// the uber modifiers are a superset of all the keywords that
// can appear in a type specifier; see cc.gr, nonterm DeclSpecifier
enum UberModifiers {
  UM_NONE         = 0,

  // decl flags
  UM_AUTO         = 0x00000001,
  UM_REGISTER     = 0x00000002,
  UM_STATIC       = 0x00000004,
  UM_EXTERN       = 0x00000008,
  UM_MUTABLE      = 0x00000010,

  UM_INLINE       = 0x00000020,
  UM_VIRTUAL      = 0x00000040,
  UM_EXPLICIT     = 0x00000080,

  UM_FRIEND       = 0x00000100,
  UM_TYPEDEF      = 0x00000200,
  UM_EVENT        = 0x00000400,   // STATE_EXTENSION

  UM_DECLFLAGS    = 0x000007FF,

  UM_UNUSED1      = 0x00000800,
  UM_UNUSED2      = 0x00001000,
 
  // type keywords
  UM_WCHAR_T      = 0x00002000,
  UM_BOOL         = 0x00004000,
  UM_SHORT        = 0x00008000,
  UM_INT          = 0x00010000,
  UM_LONG         = 0x00020000,
  UM_SIGNED       = 0x00040000,
  UM_UNSIGNED     = 0x00080000,
  UM_FLOAT        = 0x00100000,
  UM_DOUBLE       = 0x00200000,
  UM_VOID         = 0x00400000,
  UM_LONG_LONG    = 0x00800000,    // GNU extension
  UM_CHAR         = 0x01000000,    // large value b/c got bumped by UM_RESTRICT
  UM_COMPLEX      = 0x02000000,    // C99/GNU
  UM_IMAGINARY    = 0x04000000,    // C99

  UM_TYPEKEYS     = 0x07FFE000,

  UM_UNUSED3      = 0x08000000,
 
  // cv-qualifier
  UM_CONST        = 0x10000000,
  UM_VOLATILE     = 0x20000000,
  UM_RESTRICT     = 0x40000000,    // C99

  UM_CVFLAGS      = 0x70000000,
  UM_CV_SHIFT_AMOUNT = 28,

  UM_UNUSED4      = 0x80000000,

  UM_ALL_FLAGS    = 0xFFFFFFFF,
  UM_NUM_FLAGS    = 32             // # bits set in UM_ALL_FLAGS
};

// string repr.
extern char const * const uberModifierNames[UM_NUM_FLAGS];
sm::string toString(UberModifiers m);

// select particular subsets
inline DeclFlags uberDeclFlags(UberModifiers m)
  { return (DeclFlags)(m & UM_DECLFLAGS); }
inline CVFlags uberCVFlags(UberModifiers m)
  { return (CVFlags)(((m & UM_CVFLAGS) >> UM_CV_SHIFT_AMOUNT) << CV_SHIFT_AMOUNT); }

// two more related functions, uberSimpleType and uberCombine,
// are declared in ccparse.h

// I do *not* define operators to combine the flags with | and &
// because I want those operations to always be done by dedicated
// functions like 'uberDeclFlags'


// ------------------ special literal expressions ----------------
// some literal expressions get special treatment; this enum
// can be used to identify which kind of special expression
// something is, or that it is not special
enum SpecialExpr {
  SE_NONE,          // not special
  SE_ZERO,          // integer literal 0
  SE_STRINGLIT,     // a string literal
  NUM_SPECIAL_EXPRS
};

char const *toString(SpecialExpr se);


// ----------------- kinds of template parameters ----------------
// the possible kinds of template parameters (cppstd 14.1)
enum TemplateParameterKind {
  TPK_TYPE,         // template <class T>
  TPK_NON_TYPE,     // template <int n>
  TPK_TEMPLATE,     // template <template <class T> class U>
  NUM_TEMPLATE_PARAMETER_KINDS
};

char const *toString(TemplateParameterKind tpk);


#endif // CC_FLAGS_H
