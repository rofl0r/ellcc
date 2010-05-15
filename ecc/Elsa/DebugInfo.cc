//===--- DebugInfo.cpp - Emit Debug Information for a Module ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This coordinates the debug information generation while generating code.
//
//===----------------------------------------------------------------------===//

#include "DebugInfo.h"
#include "SourceManager.h"
#include "FileManager.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Intrinsics.h"
#include "llvm/Module.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Dwarf.h"
#include "llvm/System/Path.h"
#include "llvm/Target/TargetMachine.h"
using namespace ellcc;

DebugInfo::DebugInfo(CC2LLVMEnv& env, LangOptions& LO)
  : env(env), LO(LO),
    isMainCompileUnitCreated(false), DebugFactory(*env.mod),
    BlockLiteralGenericSet(false)
{
    CreateCompileUnit();
}

DebugInfo::~DebugInfo() {
  assert(RegionStack.empty() && "Region stack mismatch, stack not empty!");
}

/// getContext - Get context info for the decl.
llvm::DIDescriptor DebugInfo::getContext(const Variable *Decl,
                                         llvm::DIDescriptor &CompileUnit)
{
#if RICH
  if (Decl->isFileVarDecl())
    return CompileUnit;
  if (Decl->getDeclContext()->isFunctionOrMethod()) {
    // Find the last subprogram in region stack.
    for (unsigned RI = RegionStack.size(), RE = 0; RI != RE; --RI) {
      llvm::DIDescriptor R = RegionStack[RI - 1];
      if (R.isSubprogram())
        return R;
    }
  }
#endif
    return CompileUnit;
}

void DebugInfo::setLocation(SourceLocation Loc)
{
    if (Loc.isValid()) {
        SourceManager SM;
        CurLoc = SM.getInstantiationLoc(Loc);
    }
}

/// getOrCreateFile - Get the compile unit from the cache or create a new
/// one if necessary. This returns null for invalid source locations.
llvm::DIFile DebugInfo::getOrCreateFile(SourceLocation Loc)
{
#if RICH
    // Get source file information.
    const char *FileName =  "<unknown>";
    SourceManager SM;
    unsigned FID = 0;
    if (Loc.isValid()) {
        PresumedLoc PLoc = SM.getPresumedLoc(Loc);
        FileName = PLoc.getFilename();
        FID = PLoc.getIncludeLoc().getRawEncoding();
    }
   
    // See if this compile unit has been used before.
    llvm::DIFile &Unit = CompileUnitCache[FID];
    if (!Unit.isNull()) return Unit;

    // Get absolute path name.
    llvm::sys::Path AbsFileName(FileName);
    if (!AbsFileName.isAbsolute()) {
        llvm::sys::Path tmp = llvm::sys::Path::GetCurrentDirectory();
        tmp.appendComponent(FileName);
        AbsFileName = tmp;
    }

    // See if thie compile unit is representing main source file. Each source
    // file has corresponding compile unit. There is only one main source
    // file at a time.
    bool isMain = false;
    const char *MainFileName = LO.getMainFileName();
    if (isMainCompileUnitCreated == false) {
        if (MainFileName) {
            if (AbsFileName.getLast().str() == MainFileName)
                isMain = true;
        } else {
            if (Loc.isValid() && SM.isFromMainFile(Loc))
                isMain = true;
        }
        if (isMain)
            isMainCompileUnitCreated = true;
    }

    unsigned LangTag;
    if (LO.CPlusPlus) {
        LangTag = llvm::dwarf::DW_LANG_C_plus_plus;
    } else if (LO.C99) {
        LangTag = llvm::dwarf::DW_LANG_C99;
    } else {
        LangTag = llvm::dwarf::DW_LANG_C89;
    }

    bool isOptimized = LO.Optimize;
    const char *Flags = "";   // FIXME: Encode command line options.

    // Create new compile unit.
    return Unit = DebugFactory.CreateCompileUnit(LangTag, AbsFileName.getLast().str(),
                                                 AbsFileName.getDirname().str(), 
                                                 LO.getProducer(), isMain, isOptimized,
                                                 Flags, /* RuntimeVers */ 0);
#else
 if (!Loc.isValid())
    // If Location is not valid then use main input file.
    return DebugFactory.CreateFile(TheCU.getFilename(), TheCU.getDirectory(),
                                   TheCU);
  SourceManager SM;
  PresumedLoc PLoc = SM.getPresumedLoc(Loc);

  // Cache the results.
  const char *fname = PLoc.getFilename();
  llvm::DenseMap<const char *, llvm::WeakVH>::iterator it =
    DIFileCache.find(fname);

  if (it != DIFileCache.end()) {
    // Verify that the information still exists.
    if (&*it->second)
      return llvm::DIFile(llvm::cast<llvm::MDNode>(it->second));
  }

  // FIXME: We shouldn't even need to call 'makeAbsolute()' in the cases
  // where we can consult the FileEntry.
  llvm::sys::Path AbsFileName(PLoc.getFilename());
  AbsFileName.makeAbsolute();

  llvm::DIFile F = DebugFactory.CreateFile(AbsFileName.getLast(),
                                           AbsFileName.getDirname(), TheCU);

  DIFileCache[fname] = F;
  return F;
#endif
}

/// CreateCompileUnit - Create new compile unit.
void DebugInfo::CreateCompileUnit() {

  // Get absolute path name.
  SourceManager SM;
  std::string MainFileName = LO.getMainFileName();
  if (MainFileName.empty())
    MainFileName = "<unknown>";

  llvm::sys::Path AbsFileName(MainFileName);
  AbsFileName.makeAbsolute();

  // The main file name provided via the "-main-file-name" option contains just
  // the file name itself with no path information. This file name may have had
  // a relative path, so we look into the actual file entry for the main
  // file to determine the real absolute path for the file.
  std::string MainFileDir;
  if (const FileEntry *MainFile = SM.getFileEntryForID(SM.getMainFileID()))
    MainFileDir = MainFile->getDir()->getName();
  else
    MainFileDir = AbsFileName.getDirname();

  unsigned LangTag;
  if (LO.CPlusPlus) {
      LangTag = llvm::dwarf::DW_LANG_C_plus_plus;
  } else if (LO.C99) {
      LangTag = llvm::dwarf::DW_LANG_C99;
  } else {
      LangTag = llvm::dwarf::DW_LANG_C89;
  }

  const char *Flags = "";   // FIXME: Encode command line options.

  // Create new compile unit.
  TheCU = DebugFactory.CreateCompileUnit(
    LangTag, AbsFileName.getLast(), MainFileDir, LO.getProducer(), true,
    LO.Optimize, Flags, 0);
}

/// CreateType - Get the Basic type from the cache or create a new
/// one if necessary.
llvm::DIType DebugInfo::CreateType(const SimpleType *ST,
                                   llvm::DIFile Unit)
{
    unsigned Encoding = 0;
    switch (ST->type) {
    default:
    case ST_VOID:
        return llvm::DIType();
    case ST_CHAR:
        if (env.TI.isCharSigned()) {
            Encoding = llvm::dwarf::DW_ATE_signed_char;
        } else {
            Encoding = llvm::dwarf::DW_ATE_unsigned_char;
        }
        break;
    case ST_UNSIGNED_CHAR:                Encoding = llvm::dwarf::DW_ATE_unsigned_char; break;
    case ST_SIGNED_CHAR:                  Encoding = llvm::dwarf::DW_ATE_signed_char; break;
    case ST_UNSIGNED_SHORT_INT:
    case ST_UNSIGNED_INT:
    case ST_UNSIGNED_LONG_INT:
    case ST_UNSIGNED_LONG_LONG:           Encoding = llvm::dwarf::DW_ATE_unsigned; break;
    case ST_SHORT_INT:
    case ST_INT:
    case ST_LONG_INT:
    case ST_LONG_LONG:                    Encoding = llvm::dwarf::DW_ATE_signed; break;
    case ST_BOOL:                         Encoding = llvm::dwarf::DW_ATE_boolean; break;
    case ST_FLOAT:
    case ST_DOUBLE:
    case ST_LONG_DOUBLE:                  Encoding = llvm::dwarf::DW_ATE_float; break;
    case ST_FLOAT_COMPLEX:
    case ST_DOUBLE_COMPLEX:
    case ST_LONG_DOUBLE_COMPLEX:          Encoding = llvm::dwarf::DW_ATE_complex_float; break;
    case ST_FLOAT_IMAGINARY:
    case ST_DOUBLE_IMAGINARY:
    case ST_LONG_DOUBLE_IMAGINARY:        Encoding = llvm::dwarf::DW_ATE_imaginary_float; break;
  } 
  // Bit size, align and offset of the type.
  uint64_t Size, Align;
  ST->sizeInfoInBits(env.TI, Size, Align);
  uint64_t Offset = 0;
  
  return DebugFactory.CreateBasicType(Unit, 
                                      env.TI.getTypeName((TargetInfo::TypeID)ST->type),
                                      Unit, 0, Size, Align,
                                      Offset, /*flags*/ 0, Encoding);
}

/// CreateCVRType - Get the CVR qualified type from the cache or create 
/// a new one if necessary.
llvm::DIType DebugInfo::CreateCVRType(const CVAtomicType* Ty, CVFlags cv, llvm::DIFile Unit)
{
    cv = Ty->cv & (CV_CONST | CV_VOLATILE | CV_RESTRICT);
    unsigned Tag = 0;
    if (Ty->isConst()) {
        Tag = llvm::dwarf::DW_TAG_const_type;
        cv &= ~CV_CONST; 
    } else if (Ty->isVolatile()) {
        Tag = llvm::dwarf::DW_TAG_volatile_type;
        cv &= ~CV_VOLATILE; 
    } else {
        assert(Ty->isRestrict() && "Unknown type qualifier for debug info");
        Tag = llvm::dwarf::DW_TAG_restrict_type;
        cv &= ~CV_RESTRICT; 
    }
  
    llvm::DIType FromTy;
    if (cv) {
        // More qualifiers.
        FromTy = CreateCVRType(Ty, cv, Unit);
    } else {
        FromTy = getOrCreateType(Ty, Unit, true);
    }
 
    // No need to fill in the Name, Line, Size, Alignment, Offset in case of
    // CVR derived types.
    return DebugFactory.CreateDerivedType(Tag, Unit, "", llvm::DIFile(),
                                          0, 0, 0, 0, 0, FromTy);
}

llvm::DIType DebugInfo::CreateType(const PointerType *Ty,
                                     llvm::DIFile Unit)
{
  llvm::DIType EltTy = getOrCreateType(Ty->getAtType(), Unit);
 
  // Bit size, align and offset of the type.
  uint64_t Size, Align;
  Ty->sizeInfoInBits(env.TI, Size, Align);
                                                                               
  return DebugFactory.CreateDerivedType(llvm::dwarf::DW_TAG_pointer_type, Unit,
                                        "", llvm::DIFile(),
                                        0, Size, Align, 0, 0, EltTy);
}

#if RICH
llvm::DIType DebugInfo::CreateType(const TypedefType *Ty,
                                     llvm::DIFile Unit)
{
  // Typedefs are derived from some other type.  If we have a typedef of a
  // typedef, make sure to emit the whole chain.
  llvm::DIType Src = getOrCreateType(Ty->getDecl()->getUnderlyingType(), Unit);
  
  // We don't set size information, but do specify where the typedef was
  // declared.
  std::string TyName = Ty->getDecl()->getNameAsString();
  SourceLocation DefLoc = Ty->getDecl()->loc;
  llvm::DIFile DefUnit = getOrCreateFile(DefLoc);

  SourceManager &SM = M->getContext().getSourceManager();
  PresumedLoc PLoc = SM.getPresumedLoc(DefLoc);
  unsigned Line = PLoc.isInvalid() ? 0 : PLoc.getLine();

  return DebugFactory.CreateDerivedType(llvm::dwarf::DW_TAG_typedef, Unit,
                                        TyName, DefUnit, Line, 0, 0, 0, 0, Src);
}
#endif

llvm::DIType DebugInfo::CreateType(const FunctionType *Ty,
                                     llvm::DIFile Unit)
{
    llvm::SmallVector<llvm::DIDescriptor, 16> EltTys;

    // Add the result type at least.
    EltTys.push_back(getOrCreateType(Ty->retType, Unit));
  
    // Set up remainder of arguments if there is a prototype.
    SFOREACH_OBJLIST(Variable, Ty->params, iter) {
        Variable const *param = iter.data();

        EltTys.push_back(getOrCreateType(param->type, Unit));
    }

    llvm::DIArray EltTypeArray = DebugFactory.GetOrCreateArray(EltTys.data(), EltTys.size());
    return DebugFactory.CreateCompositeType(llvm::dwarf::DW_TAG_subroutine_type,
                                            Unit, "", llvm::DIFile(),
                                            0, 0, 0, 0, 0,
                                            llvm::DIType(), EltTypeArray);
}


#if RICH
/// CreateType - get structure or union type.
llvm::DIType DebugInfo::CreateType(const CompoundType *Ty,
                                     llvm::DIFile Unit)
{
    const CompoundType *ct = Ty->asCompoundTypeC();
  
    unsigned Tag;
  if (ct->keyword == CompoundType::K_STRUCT) {
    Tag = llvm::dwarf::DW_TAG_structure_type;
  } else if (ct->keyword == CompoundType::K_UNION) {
    Tag = llvm::dwarf::DW_TAG_union_type;
  } else {
    assert(ct->keyword == CompoundType::K_CLASS && "Unknown CompoundType!");
    Tag = llvm::dwarf::DW_TAG_class_type;
  }

  SourceManager SM;

  // Get overall information about the record type for the debug info.
  std::string Name = ct->name;

  PresumedLoc PLoc = SM.getPresumedLoc(Ty->loc);
  llvm::DIFile DefUnit;
  unsigned Line = 0;
  if (!PLoc.isInvalid()) {
    DefUnit = getOrCreateFile(Ty->loc);
    Line = PLoc.getLine();
  }
  
  // Records and classes and unions can all be recursive.  To handle them, we
  // first generate a debug descriptor for the struct as a forward declaration.
  // Then (if it is a definition) we go through and get debug info for all of
  // its members.  Finally, we create a descriptor for the complete type (which
  // may refer to the forward decl if the struct is recursive) and replace all
  // uses of the forward declaration with the final definition.
  llvm::DICompositeType FwdDecl =
    DebugFactory.CreateCompositeType(Tag, Unit, Name, DefUnit, Line, 0, 0, 0, 0,
                                     llvm::DIType(), llvm::DIArray());
  
  // If this is just a forward declaration, return it.
  if (!Decl->getDefinition(M->getContext()))
    return FwdDecl;

  // Otherwise, insert it into the TypeCache so that recursive uses will find
  // it.
  TypeCache[QualType(Ty, 0).getAsOpaquePtr()] = FwdDecl;

  // Convert all the elements.
  llvm::SmallVector<llvm::DIDescriptor, 16> EltTys;

  const ASTRecordLayout &RL = M->getContext().getASTRecordLayout(Decl);

  unsigned FieldNo = 0;
  for (RecordDecl::field_iterator I = Decl->field_begin(),
                                  E = Decl->field_end(); 
       I != E; ++I, ++FieldNo) {
    FieldDecl *Field = *I;
    llvm::DIType FieldTy = getOrCreateType(Field->getType(), Unit);

    std::string FieldName = Field->getNameAsString();

    // Ignore unnamed fields.
    if (FieldName.empty())
      continue;

    // Get the location for the field.
    SourceLocation FieldDefLoc = Field->loc;
    PresumedLoc PLoc = SM.getPresumedLoc(FieldDefLoc);
    llvm::DIFile FieldDefUnit;
    unsigned FieldLine = 0;
    
    if (!PLoc.isInvalid()) {
      FieldDefUnit = getOrCreateFile(FieldDefLoc);
      FieldLine = PLoc.getLine();
    }

    QualType FType = Field->getType();
    uint64_t FieldSize = 0;
    unsigned FieldAlign = 0;
    if (!FType->isIncompleteArrayType()) {
    
      // Bit size, align and offset of the type.
      FieldSize = M->getContext().getTypeSize(FType);
      Expr *BitWidth = Field->getBitWidth();
      if (BitWidth)
        FieldSize = BitWidth->EvaluateAsInt(M->getContext()).getZExtValue();
      
      FieldAlign =  M->getContext().getTypeAlign(FType);
    }

    uint64_t FieldOffset = RL.getFieldOffset(FieldNo);    
    
    // Create a DW_TAG_member node to remember the offset of this field in the
    // struct.  FIXME: This is an absolutely insane way to capture this
    // information.  When we gut debug info, this should be fixed.
    FieldTy = DebugFactory.CreateDerivedType(llvm::dwarf::DW_TAG_member, Unit,
                                             FieldName, FieldDefUnit,
                                             FieldLine, FieldSize, FieldAlign,
                                             FieldOffset, 0, FieldTy);
    EltTys.push_back(FieldTy);
  }
  
  llvm::DIArray Elements =
    DebugFactory.GetOrCreateArray(EltTys.data(), EltTys.size());

  // Bit size, align and offset of the type.
  uint64_t Size = M->getContext().getTypeSize(Ty);
  uint64_t Align = M->getContext().getTypeAlign(Ty);
  
  llvm::DICompositeType RealDecl =
    DebugFactory.CreateCompositeType(Tag, Unit, Name, DefUnit, Line, Size,
                                     Align, 0, 0, llvm::DIType(), Elements);

  // Now that we have a real decl for the struct, replace anything using the
  // old decl with the new one.  This will recursively update the debug info.
  FwdDecl.replaceAllUsesWith(RealDecl);

  // Update TypeCache.
  TypeCache[QualType(Ty, 0).getAsOpaquePtr()] = RealDecl;  
  return RealDecl;
}

llvm::DIType DebugInfo::CreateType(const EnumType *Ty,
                                     llvm::DIFile Unit) {
  EnumDecl *Decl = Ty->getDecl();

  llvm::SmallVector<llvm::DIDescriptor, 32> Enumerators;

  // Create DIEnumerator elements for each enumerator.
  for (EnumDecl::enumerator_iterator 
         Enum = Decl->enumerator_begin(), EnumEnd = Decl->enumerator_end();
       Enum != EnumEnd; ++Enum) {
    Enumerators.push_back(DebugFactory.CreateEnumerator(Enum->getNameAsString(),
                                            Enum->getInitVal().getZExtValue()));
  }
  
  // Return a CompositeType for the enum itself.
  llvm::DIArray EltArray =
    DebugFactory.GetOrCreateArray(Enumerators.data(), Enumerators.size());

  std::string EnumName = Decl->getNameAsString();
  SourceLocation DefLoc = Decl->loc;
  llvm::DIFile DefUnit = getOrCreateFile(DefLoc);
  SourceManager &SM = M->getContext().getSourceManager();
  PresumedLoc PLoc = SM.getPresumedLoc(DefLoc);
  unsigned Line = PLoc.isInvalid() ? 0 : PLoc.getLine();

  
  // Size and align of the type.
  uint64_t Size = 0;
  unsigned Align = 0;
  if (!Ty->isIncompleteType()) {
    Size = M->getContext().getTypeSize(Ty);
    Align = M->getContext().getTypeAlign(Ty);
  }
  
  return DebugFactory.CreateCompositeType(llvm::dwarf::DW_TAG_enumeration_type,
                                          Unit, EnumName, DefUnit, Line,
                                          Size, Align, 0, 0,
                                          llvm::DIType(), EltArray);
}

llvm::DIType DebugInfo::CreateType(const TagType *Ty,
                                     llvm::DIFile Unit) {
  if (const RecordType *RT = dyn_cast<RecordType>(Ty))
    return CreateType(RT, Unit);
  else if (const EnumType *ET = dyn_cast<EnumType>(Ty))
    return CreateType(ET, Unit);
  
  return llvm::DIType();
}

llvm::DIType DebugInfo::CreateType(const ArrayType *Ty,
                                     llvm::DIFile Unit) {
  uint64_t Size;
  uint64_t Align;
  
  
  // FIXME: make getTypeAlign() aware of VLAs and incomplete array types
  if (const VariableArrayType *VAT = dyn_cast<VariableArrayType>(Ty)) {
    Size = 0;
    Align =
      M->getContext().getTypeAlign(M->getContext().getBaseElementType(VAT));
  } else if (Ty->isIncompleteArrayType()) {
    Size = 0;
    Align = M->getContext().getTypeAlign(Ty->getElementType());
  } else {
    // Size and align of the whole array, not the element type.
    Size = M->getContext().getTypeSize(Ty);
    Align = M->getContext().getTypeAlign(Ty);
  }
  
  // Add the dimensions of the array.  FIXME: This loses CV qualifiers from
  // interior arrays, do we care?  Why aren't nested arrays represented the
  // obvious/recursive way?
  llvm::SmallVector<llvm::DIDescriptor, 8> Subscripts;
  QualType EltTy(Ty, 0);
  while ((Ty = dyn_cast<ArrayType>(EltTy))) {
    uint64_t Upper = 0;
    if (const ConstantArrayType *CAT = dyn_cast<ConstantArrayType>(Ty)) 
      if (CAT->getSize().getZExtValue())
	Upper = CAT->getSize().getZExtValue() - 1;
    // FIXME: Verify this is right for VLAs.
    Subscripts.push_back(DebugFactory.GetOrCreateSubrange(0, Upper));
    EltTy = Ty->getElementType();
  }
  
  llvm::DIArray SubscriptArray =
    DebugFactory.GetOrCreateArray(Subscripts.data(), Subscripts.size());

  return DebugFactory.CreateCompositeType(llvm::dwarf::DW_TAG_array_type,
                                          Unit, "", llvm::DIFile(),
                                          0, Size, Align, 0, 0,
                                          getOrCreateType(EltTy, Unit),
                                          SubscriptArray);
}

#endif

/// getOrCreateType - Get the type from the cache or create a new
/// one if necessary.
llvm::DIType DebugInfo::getOrCreateType(const Type* Ty, llvm::DIFile Unit, bool cvDone)
{

    if (Ty == NULL)
        return llvm::DIType();
  
    // Check for existing entry.
    llvm::DenseMap<void *, llvm::WeakVH>::iterator it =
      TypeCache.find((void*)Ty);
    if (it != TypeCache.end()) {
      // Verify that the debug info still exists.
      if (&*it->second)
        return llvm::DIType(llvm::cast<llvm::MDNode>(it->second));
    }

    // Handle CVR qualifiers, which recursively handles what they refer to.
    if (!cvDone && Ty->isCVAtomicType()) {
        const CVAtomicType* cvt = Ty->asCVAtomicTypeC();
        if (cvt->cv & (CV_CONST | CV_VOLATILE | CV_RESTRICT)) {
            return CreateCVRType(cvt, cvt->cv, Unit);
        }
    }

    // Work out details of type.
    if (Ty->isSimpleType()) {
        return CreateType(Ty->asSimpleTypeC(), Unit);
    }
    if (Ty->isPointerType()) {
        return CreateType(Ty->asPointerTypeC(), Unit);
    }
    if (Ty->isFunctionType()) {
        return CreateType(Ty->asFunctionTypeC(), Unit);
    }

    return llvm::DIType();      // RICH
#if RICH
    switch (Ty->getTag()) {
    default:
        return llvm::DIType();      // RICH
    case BaseType::T_ATOMIC: return Slot = CreateType(cast<AtomicType>(Ty), Unit);
  case Type::LValueReference:
  case Type::RValueReference:
  case Type::Vector:
  case Type::ExtVector:
  case Type::ExtQual:
  case Type::FixedWidthInt:
  case Type::MemberPointer:
  case Type::TemplateSpecialization:
  case Type::QualifiedName:
    // Unsupported types
    return llvm::DIType();
  case Type::Builtin: return Slot = CreateType(cast<BuiltinType>(Ty), Unit);
  case Type::Complex: return Slot = CreateType(cast<ComplexType>(Ty), Unit);
  case Type::Typedef: return Slot = CreateType(cast<TypedefType>(Ty), Unit);
  case Type::Record:
  case Type::Enum:
    return Slot = CreateType(cast<TagType>(Ty), Unit); 
  case Type::ConstantArray:
  case Type::ConstantArrayWithExpr:
  case Type::ConstantArrayWithoutExpr:
  case Type::VariableArray:
  case Type::IncompleteArray:
    return Slot = CreateType(cast<ArrayType>(Ty), Unit);
  case Type::TypeOfExpr:
    return Slot = getOrCreateType(cast<TypeOfExprType>(Ty)->getUnderlyingExpr()
                                  ->getType(), Unit);
  case Type::TypeOf:
    return Slot = getOrCreateType(cast<TypeOfType>(Ty)->getUnderlyingType(),
                                  Unit);
  case Type::Decltype:
    return Slot = getOrCreateType(cast<DecltypeType>(Ty)->getUnderlyingType(),
                                  Unit);
  }
  
  return Slot;
#endif
}

/// EmitFunctionStart - Constructs the debug code for entering a function -
/// "llvm.dbg.func.start.".
/// EmitFunctionStart - Constructs the debug code for entering a function -
/// "llvm.dbg.func.start.".
void DebugInfo::EmitFunctionStart(const char *Name, Type* ReturnType,
                                    llvm::Function *Fn,
                                    BuilderTy &Builder)
{
    const char *LinkageName = Name;
  
    // Skip the asm prefix if it exists.
    //
    // FIXME: This should probably be the unmangled name?
    if (Name[0] == '\01')
        ++Name;
  
    // FIXME: Why is this using CurLoc???
    llvm::DIFile Unit = getOrCreateFile(CurLoc);
    SourceManager SM;
    unsigned LineNo = SM.getPresumedLoc(CurLoc).getLine();
  
    llvm::DISubprogram SP =
        DebugFactory.CreateSubprogram(Unit, Name, Name, LinkageName, Unit, LineNo,
                                      getOrCreateType(ReturnType, Unit),
                                  Fn->hasInternalLinkage(), true/*definition*/);
  
    // Push function on region stack.
    llvm::MDNode *SPN = SP;
    RegionStack.push_back(SPN);
    RegionMap[Fn] = llvm::WeakVH(SP);
}


void DebugInfo::EmitStopPoint(llvm::Function *Fn, BuilderTy &Builder)
{
    if (CurLoc.isInvalid() || CurLoc.isMacroID()) return;
  
    // Don't bother if things are the same as last time.
    SourceManager SM;
    if (   CurLoc == PrevLoc 
        || (SM.getInstantiationLineNumber(CurLoc)
            == SM.getInstantiationLineNumber(PrevLoc)
        && SM.isFromSameFile(CurLoc, PrevLoc)))
    return;

    // Update last state.
    PrevLoc = CurLoc;

    // Get the appropriate compile unit.
    llvm::DIFile Unit = getOrCreateFile(CurLoc);
    PresumedLoc PLoc = SM.getPresumedLoc(CurLoc);

    llvm::MDNode *Scope = RegionStack.back();
    Builder.SetCurrentDebugLocation(llvm::DebugLoc::get(PLoc.getLine(),
                                                        PLoc.getColumn(),
                                                        Scope));
}

/// EmitRegionStart- Constructs the debug code for entering a declarative
/// region - "llvm.dbg.region.start.".
void DebugInfo::EmitRegionStart(llvm::Function *Fn, BuilderTy &Builder)
{
    SourceManager SM;
  PresumedLoc PLoc = SM.getPresumedLoc(CurLoc);
  llvm::DIDescriptor D =
    DebugFactory.CreateLexicalBlock(RegionStack.empty() ?
                                    llvm::DIDescriptor() :
                                    llvm::DIDescriptor(RegionStack.back()),
                                    PLoc.getLine(), PLoc.getColumn());
  llvm::MDNode *DN = D;
  RegionStack.push_back(DN);
}

/// EmitRegionEnd - Constructs the debug code for exiting a declarative
/// region - "llvm.dbg.region.end."
void DebugInfo::EmitRegionEnd(llvm::Function *Fn, BuilderTy &Builder)
{
    assert(!RegionStack.empty() && "Region stack mismatch, stack empty!");

    // Provide a region stop point.
    EmitStopPoint(Fn, Builder);
    RegionStack.pop_back();
}

/// EmitDeclare - Emit local variable declaration debug info.
void DebugInfo::EmitDeclare(const Variable *Decl, unsigned Tag,
                            llvm::Value *Storage, BuilderTy &Builder)
{
    assert(!RegionStack.empty() && "Region stack mismatch, stack empty!");

#if RICH
    // Do not emit variable debug information while generating optimized code.
    // The llvm optimizer and code generator are not yet ready to support
    // optimized code debugging.
    const CompileOptions &CO = M->getCompileOpts();
    if (CO.OptimizationLevel)
        return;
#endif

    llvm::DIFile Unit = getOrCreateFile(Decl->loc);
    llvm::DIType Ty = getOrCreateType(Decl->getTypeC(), Unit);

    // Get location information.
    SourceManager SM;
    PresumedLoc PLoc = SM.getPresumedLoc(Decl->loc);
    unsigned Line = 0;
    if (!PLoc.isInvalid())
        Line = PLoc.getLine();
    else
        Unit = llvm::DIFile();
  
    // Create the descriptor for the variable.
    llvm::DIVariable D = 
        DebugFactory.CreateVariable(Tag, llvm::DIDescriptor(RegionStack.back()),
                                 Decl->Name(env.TI),
                                    Unit, Line, Ty);
    // Insert an llvm.dbg.declare into the current block .
    DebugFactory.InsertDeclare(Storage, D, Builder.GetInsertBlock());
}

void DebugInfo::EmitDeclareOfAutoVariable(const Variable *Decl,
                                          llvm::Value *Storage,
                                          BuilderTy &Builder) {
  EmitDeclare(Decl, llvm::dwarf::DW_TAG_auto_variable, Storage, Builder);
}

/// EmitDeclareOfArgVariable - Emit call to llvm.dbg.declare for an argument
/// variable declaration.
void DebugInfo::EmitDeclareOfArgVariable(const Variable *Decl, llvm::Value *AI,
                                         BuilderTy &Builder) {
  EmitDeclare(Decl, llvm::dwarf::DW_TAG_arg_variable, AI, Builder);
}

/// EmitGlobalVariable - Emit information about a global variable.
void DebugInfo::EmitGlobalVariable(llvm::GlobalVariable *Var, 
                                     const Variable *Decl) {

#if RICH
  // Do not emit variable debug information while generating optimized code.
  // The llvm optimizer and code generator are not yet ready to support
  // optimized code debugging.
  const CompileOptions &CO = M->getCompileOpts();
  if (CO.OptimizationLevel)
    return;
#endif

  // Create global variable debug descriptor.
  llvm::DIFile Unit = getOrCreateFile(Decl->loc);
  SourceManager SM;
  PresumedLoc PLoc = SM.getPresumedLoc(Decl->loc);
  unsigned LineNo = PLoc.isInvalid() ? 0 : PLoc.getLine();

  const Type* T = Decl->getTypeC();
#if RICH
  if (T->isIncompleteArrayType()) {
    
    // CodeGen turns int[] into int[1] so we'll do the same here.
    llvm::APSInt ConstVal(32);
    
    ConstVal = 1;
    QualType ET = M->getContext().getAsArrayType(T)->getElementType();
    
    T = M->getContext().getConstantArrayType(ET, ConstVal, 
                                           ArrayType::Normal, 0);
  }
#endif

    llvm::DIType Ty = getOrCreateType(T, Unit);;
#if RICH
    if (0 && !Ty.isNull()) {    // RICH
        const char* Name = Decl->Name(env.TI);
        DebugFactory.CreateGlobalVariable(getContext(Decl, Unit), Name, Name, NULL,
                                          Unit, LineNo, Ty,
                                          Var->hasInternalLinkage(),
                                          true/*definition*/, Var);
    }
#endif
}
