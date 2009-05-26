#include "LangOptions.h"
#include <string.h>

using namespace ellcc;

static void setWarning(bool3 &b, bool enable)
{
  if (enable) {
    if (b == b3_TRUE) {
      b = b3_WARN;       // accept with warning
    }
  }
  else {
    if (b == b3_WARN) {
      b = b3_TRUE;       // accept silently
    }
  }
}

void LangOptions::setAllWarnings(bool enable)
{
  setWarning(allowImplicitFunctionDecls, enable);
  setWarning(allowImplicitIntForOperators, enable);
  setWarning(allowQualifiedMemberDeclarations, enable);
  setWarning(allowModifiersWithTypedefNames, enable);
  setWarning(allowAnonymousStructs, enable);
  setWarning(allowGcc2HeaderSyntax, enable);
  setWarning(allowRepeatedTypeSpecifierKeywords, enable);
  setWarning(allowCVAppliedToFunctionTypes, enable);
  setWarning(allowDefinitionsInWrongScopes, enable);
  setWarning(allowDuplicateParameterNames, enable);
  setWarning(allowExplicitSpecWithoutParams, enable);
  setWarning(allowStaticAfterNonStatic, enable);
}


// ---------------------- ANSI and K&R C ----------------------
void LangOptions::ANSI_C89()
{
  // just in case I forget to initialize something....
  memset(this, 0, sizeof(*this));

  GNUMode = false;
  Trigraphs = true;
  BCPLComment = DollarIdents = AsmPreprocessor = false;
  GNUMode = Digraphs = false;
  HexFloats = false;
  C99 = CPlusPlus = CPlusPlus0x = NoExtensions = false;
  CXXOperatorNames = Boolean = WritableStrings = false;
  Exceptions = Freestanding = NoBuiltin = false;
  LaxVectorConversions = true;
  HeinousExtensions = false;
    
  // FIXME: The default should be 1.
  ThreadsafeStatics = false;
  Blocks = false;
  EmitAllDecls = false;
  MathErrno = true;

    // FIXME: The default should be 1.
  AccessControl = false;
    
  OverflowChecking = false;

  InstantiationDepth = 99;
    
  Optimize = false;
  OptimizeSize = false;

  Static = false;
  PICLevel = 0;

  GNUInline = false;
  NoInline = false;

  MainFileName = NULL;
  declareGNUBuiltins = false;

  tagsAreTypes = false;
  recognizeCppKeywords = false;
  implicitFuncVariable = false;
  gccFuncBehavior = GFB_none;
  noInnerClasses = true;
  uninitializedGlobalDataIsCommon = true;
  emptyParamsMeansNoInfo = true;
  strictArraySizeRequirements = false;
  assumeNoSizeArrayHasSizeOne = false;
  allowOverloading = false;
  compoundSelfName = false;
  allowImplicitFunctionDecls = b3_TRUE;        // C89 does not require prototypes
  allowImplicitInt = true;
  allowDynamicallySizedArrays = false;
  allowIncompleteEnums = false;
  allowMemberWithClassName = true;
  nonstandardAssignmentOperator = false;
  allowExternCThrowMismatch = true;
  allowImplicitIntForMain = false;
  predefined_Bool = false;

  handleExternInlineSpecially = false;
  inlineImpliesStaticLinkage = false;
  stringLitCharsAreConst = false; // Didn't check C89; C99 says they are non-const

  // C99 spec: Section 6.5.4, footnote 85: "A cast does not yield an lvalue".
  lvalueFlowsThroughCast = false;

  restrictIsAKeyword = false;

  allowNewlinesInStringLits = false;
  allowImplicitIntForOperators = b3_FALSE;
  allowQualifiedMemberDeclarations = b3_FALSE;
  allowModifiersWithTypedefNames = b3_FALSE;
  allowAnonymousStructs = b3_FALSE;

  gcc2StdEqualsGlobalHacks = false;
  allowGcc2HeaderSyntax = b3_FALSE;
  allowRepeatedTypeSpecifierKeywords = b3_FALSE;
  allowCVAppliedToFunctionTypes = b3_FALSE;
  allowDefinitionsInWrongScopes = b3_FALSE;
  allowDuplicateParameterNames = b3_FALSE;
  allowExplicitSpecWithoutParams = b3_FALSE;
  allowStaticAfterNonStatic =  b3_WARN;
}

void LangOptions::KandR_C()
{
  ANSI_C89();

  allowImplicitInt = true;
}

void LangOptions::ANSI_C99_extensions()
{
  C99 = 1;
  BCPLComment = true;
  Digraphs = true;
  HexFloats = true;
  implicitFuncVariable = true;
  predefined_Bool = true;
  restrictIsAKeyword = true;
}

void LangOptions::ANSI_C99()
{
  ANSI_C89();

  // new features
  ANSI_C99_extensions();

  // removed C89 features
  allowImplicitInt = false;
  allowImplicitFunctionDecls = b3_FALSE;
}


// ------------------------ GNU C ------------------------
void LangOptions::GNU_C_extensions()
{
  GNUMode = true;
  Trigraphs = false;
  gccFuncBehavior = GFB_string;
  allowDynamicallySizedArrays = true;
  assumeNoSizeArrayHasSizeOne = true;
  handleExternInlineSpecially = true;
  declareGNUBuiltins = true;

  // http://gcc.gnu.org/onlinedocs/gcc-3.1/gcc/Lvalues.html
  lvalueFlowsThroughCast = true;

  allowNewlinesInStringLits = true;

  // http://gcc.gnu.org/onlinedocs/gcc-3.1/gcc/Incomplete-Enums.html
  allowIncompleteEnums = true;

  allowModifiersWithTypedefNames = b3_TRUE;
  allowAnonymousStructs = b3_TRUE;
  allowRepeatedTypeSpecifierKeywords = b3_TRUE;
  allowCVAppliedToFunctionTypes = b3_TRUE;
}

void LangOptions::GNU_C99()
{
  ANSI_C89();

  ANSI_C99_extensions();
  GNU_C_extensions();
}

void LangOptions::GNU3_KandR_C()
{
  KandR_C();

  GNU_C_extensions();

  // this seems wrong, but Oink's tests want it this way...
  ANSI_C99_extensions();
}

void LangOptions::GNU2_KandR_C()
{
  GNU3_KandR_C();

  // dsw: seems to not be true for gcc 2.96 at least
  predefined_Bool = false;
}


// ---------------------------- C++ ----------------------------
void LangOptions::ANSI_Cplusplus98()
{
  // just in case...
  memset(this, 0, sizeof(*this));

  HexFloats = false;
  C99 = false;
  GNUMode = false;
  BCPLComment = true;
  CPlusPlus = true;
  CXXOperatorNames = true;
  declareGNUBuiltins = false;

  tagsAreTypes = true;
  recognizeCppKeywords = true;
  implicitFuncVariable = false;
  gccFuncBehavior = GFB_none;
  noInnerClasses = false;
  uninitializedGlobalDataIsCommon = false;
  emptyParamsMeansNoInfo = false;

  strictArraySizeRequirements = true;
  assumeNoSizeArrayHasSizeOne = false;

  allowOverloading = true;
  compoundSelfName = true;

  allowImplicitFunctionDecls = b3_FALSE;
  allowImplicitInt = false;
  allowDynamicallySizedArrays = false;
  allowIncompleteEnums = false;
  allowMemberWithClassName = false;

  // indeed this is nonstandard but everyone seems to do it this way ...
  nonstandardAssignmentOperator = true;

  allowExternCThrowMismatch = false;
  allowImplicitIntForMain = false;

  predefined_Bool = false;
  handleExternInlineSpecially = false;
  inlineImpliesStaticLinkage = true;
  stringLitCharsAreConst = true; // Cppstd says they are const.
  lvalueFlowsThroughCast = false;
  restrictIsAKeyword = false;

  allowNewlinesInStringLits = false;
  allowImplicitIntForOperators = b3_FALSE;
  allowQualifiedMemberDeclarations = b3_FALSE;
  allowModifiersWithTypedefNames = b3_FALSE;
  allowAnonymousStructs = b3_FALSE;

  gcc2StdEqualsGlobalHacks = false;
  allowGcc2HeaderSyntax = b3_FALSE;
  allowRepeatedTypeSpecifierKeywords = b3_FALSE;
  allowCVAppliedToFunctionTypes = b3_FALSE;
  allowDefinitionsInWrongScopes = b3_FALSE;
  allowDuplicateParameterNames = b3_FALSE;
  allowExplicitSpecWithoutParams = b3_FALSE;
  allowStaticAfterNonStatic =  b3_WARN;
}

void LangOptions::GNU_Cplusplus98()
{
  ANSI_Cplusplus98();

  HexFloats = true;
  implicitFuncVariable = true;
  gccFuncBehavior = GFB_variable;

  // is this really right?  Oink tests it like it is ...
  allowDynamicallySizedArrays = true;
  strictArraySizeRequirements = false;

  allowMemberWithClassName = true;
  allowExternCThrowMismatch = true;
  allowImplicitIntForMain = true;

  declareGNUBuiltins = true;

  allowQualifiedMemberDeclarations = b3_TRUE;
  allowAnonymousStructs = b3_TRUE;

  gcc2StdEqualsGlobalHacks = true;
  allowGcc2HeaderSyntax = b3_TRUE;
  allowDefinitionsInWrongScopes = b3_TRUE;
  allowDuplicateParameterNames = b3_TRUE;
  allowExplicitSpecWithoutParams = b3_TRUE;
}

void LangOptions::ANSI_Cplusplus0x()
{
  ANSI_Cplusplus98();
  CPlusPlus0x = true;
}

void LangOptions::GNU_Cplusplus0x()
{
  GNU_Cplusplus98();
  CPlusPlus0x = true;
}

// -------------------------- MSVC ---------------------
void LangOptions::MSVC_bug_compatibility()
{
  allowImplicitIntForOperators = b3_TRUE;
  allowAnonymousStructs = b3_TRUE;
}
