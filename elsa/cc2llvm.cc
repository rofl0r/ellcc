// cc2llvm.cc
// code for cc2llvm.h

#include "datablok.h"
#include "exprloc.h"
#include "cc2llvm.h"            // this module
#include "TargetInfo.h"         // TargetInfo
#include "SourceManager.h"      // SourceManager
#include "LangOptions.h"
#include "ElsaDiagnostic.h"

// LLVM
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Constants.h"
#include "llvm/GlobalVariable.h"
#include "llvm/Function.h"
#include "llvm/CallingConv.h"
#include "llvm/BasicBlock.h"
#include "llvm/Instructions.h"
#include "llvm/InlineAsm.h"
#include "llvm/Intrinsics.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Type.h"

using namespace ellcc;

#define SRET 1

//#define E_FIELDACC_DEBUG
#if 0
// Really verbose debugging.
#define VDEBUG(who, where, what) std::cerr << toString(where) << ": " << who << " "; what; std::cerr << "\n"
#else
#define VDEBUG(who, where, what)
#endif

// -------------------- CC2LLVMEnv ---------------------
CC2LLVMEnv::CC2LLVMEnv(StringTable &s, sm::string name, const TranslationUnit& input,
                       TargetInfo& TI, Diagnostic& diags, llvm::LLVMContext& context,
                       LangOptions& LO)
  : str(s),
    TI(TI),
    diags(diags),
    TD(""),
    targetFolder(&TD, context),
    input(input),
    mod(new llvm::Module(name.c_str(), context)),
    function(NULL),
    functionAST(NULL),
    entryBlock(NULL),
    returnBlock(NULL),
    returnValue(NULL),
    currentBlock(NULL),
    continueBlock(NULL),
    nextBlock(NULL),
    switchInst(NULL),
    switchType(NULL),
    builder(context, targetFolder),
    context(context),
    LO(LO),
    string_main(str("main"))
{ 
    std::string str;
    TI.getTargetDescription(str);
    TD.init(str);
    mod->setDataLayout(str);
    mod->setTargetTriple(TI.getTargetTriple());
    // If debug info generation is enabled, create the CGDebugInfo object.
    // RICH: DebugInfo = CompileOpts.DebugInfo ? new CGDebugInfo(this) : 0;
    DI = new DebugInfo(*this, LO);
}

CC2LLVMEnv::~CC2LLVMEnv()
{
}

/** Send a diagnostic message.
 */
DiagnosticBuilder CC2LLVMEnv::report(SourceLocation loc, unsigned DiagID)
{
    SourceManager SM;
    return diags.Report(FullSourceLoc(loc, SM), DiagID);
}

static llvm::GlobalValue::LinkageTypes getLinkage(DeclFlags flags)
{
    if (flags & DF_INLINE) {
        return llvm::GlobalValue::LinkOnceAnyLinkage;
    } else if (flags & DF_STATIC) {
        return llvm::GlobalValue::InternalLinkage;
    }

    return llvm::GlobalValue::ExternalLinkage;
}

static bool accessValue(const llvm::Value* value)
{
    bool access = false;
    bool isConstantExpr = llvm::isa<llvm::ConstantExpr>(value);
    bool isConstantGEP = isConstantExpr
         && llvm::cast<llvm::ConstantExpr>(value)->getOpcode() == llvm::Instruction::GetElementPtr;
    bool isConstantArray = llvm::isa<llvm::ConstantArray>(value);
    if (!isConstantArray && !isConstantGEP) {
        access = true;
    }

    return access;
}

/** Make sure the current block has been opened.
 */
void CC2LLVMEnv::checkCurrentBlock()
{
    if (currentBlock == NULL && function) {
        // No block is current, make one.
        setCurrentBlock(llvm::BasicBlock::Create("", function, returnBlock));
    }
}

/** Set the current block.
 */
void CC2LLVMEnv::setCurrentBlock(llvm::BasicBlock* block)
{
    if (currentBlock) {
      // Close out the last block.
      builder.CreateBr(block);
    }
    currentBlock = block;
    builder.SetInsertPoint(block);
}

/** Get access to an operand.
 */
llvm::Value* CC2LLVMEnv::access(llvm::Value* value, bool isVolatile, int& deref, int level)
{
    while (deref > level) {
        checkCurrentBlock();
        value = builder.CreateLoad(value, isVolatile);
        --deref;
    }
    return value;
}

/** Check a condition in preparation for a branch.
 */
llvm::Value* CC2LLVMEnv::checkCondition(SourceLocation loc, llvm::Value* value, int deref, bool neg)
{
    value = access(value, false, deref);                 // RICH: Volatile.

    const llvm::Type* ctype = value->getType();
    VDEBUG("checkCondition", loc, ctype->print(std::cerr); std::cerr << " "; value->print(std::cerr));
    if (ctype != llvm::Type::Int1Ty)
    {
        // Not a boolean, check for non-zero.
        checkCurrentBlock();
        llvm::Value* zero = llvm::Constant::getNullValue(value->getType());
	if (ctype->isInteger() || ctype->getTypeID() == llvm::Type::PointerTyID) {
            if (neg) {
                value = builder.CreateICmpEQ(value, zero);
            } else {
                value = builder.CreateICmpNE(value, zero);
            }
	} else if (ctype->isFloatingPoint()) {
	    // RICH: ordered vs. unordered.
            if (neg) {
	        value = builder.CreateFCmpOEQ(value, zero);
            } else {
	        value = builder.CreateFCmpONE(value, zero);
            }
	} else {
            std::cerr << toString(loc) << ": ";
            xunimp("checkCondition");
	}
    } else if (neg) {
        // Complement the result.
        llvm::Value* ones = llvm::Constant::getAllOnesValue(ctype);
        value = builder.CreateXor(value, ones);
    }
    return value;
}

/** Check a condition in preparation for a branch.
 */
llvm::Value* CC2LLVMEnv::checkCondition(Expression* cond)
{
    int deref;
    VDEBUG("checkCondition", cond->loc, std::cerr << cond->asString() << "\n");
    llvm::Value* value = cond->cc2llvm(*this, deref);
    return checkCondition(cond->loc, value, deref);
}

const llvm::Type* CC2LLVMEnv::makeTypeSpecifier(SourceLocation loc, Type *t)
{
    const llvm::Type* type = NULL;

    switch (t->getTag())
    {
    case Type::T_ATOMIC: {
        // No need for a declarator.
	// RICH: Is const, volatile?
        CVAtomicType *at = t->asCVAtomicType();
        type =  makeAtomicTypeSpecifier(loc, at->atomic);
        break;
    }
    case Type::T_POINTER: {
	// RICH: Is const, volatile?
        PointerType *pt = t->asPointerType();
        type = makeTypeSpecifier(loc, pt->atType);
        if (type == NULL || type == llvm::Type::VoidTy) {
            /** If type is NULL, we have a va_list pointer (i.e. *...").
	     *  treat this as a void*.
	     * LLVM doesn't understand void*. Make it into MAUBits*.
	     */
            type = llvm::IntegerType::get(TI.CharWidth());
	}

        VDEBUG("makeTypeSpecifier pointer", loc, type->print(std::cerr));
	xassert(type != NULL && "A NULL type encountered");
        type = llvm::PointerType::get(type, 0);	// RICH: Address space.
        break;
    }
    case Type::T_REFERENCE: {
	// The type of a reference is the underlying type. (RICH: is it?)
        ReferenceType *rt = t->asReferenceType();
        type =  makeTypeSpecifier(loc, rt->atType);
        type =  llvm::PointerType::get(type, 0);	// RICH: Address space.
        break;
    }
    case Type::T_FUNCTION: {
        FunctionType *ft = t->asFunctionType();
        const llvm::Type* returnType = makeTypeSpecifier(loc, ft->retType);
        std::vector<const llvm::Type*>args;
        returnType = makeParameterTypes(ft, returnType, args);
        type = llvm::FunctionType::get(returnType, args, ft->acceptsVarargs() || (ft->flags & FF_NO_PARAM_INFO));
        break;
    }
    case Type::T_ARRAY: {
        ArrayType *at = t->asArrayType();
	int size = at->getSize();
	if (size == ArrayType::NO_SIZE) {
	    size = 0;
	} else if (size == ArrayType::DYN_SIZE) {
            std::cerr << toString(loc) << ": ";
            xunimp("dynamic array type");
	}
        type = llvm::ArrayType::get(makeTypeSpecifier(loc, at->eltType), size);
        break;
    }
    case Type::T_POINTERTOMEMBER:
        std::cerr << toString(loc) << ": ";
        xunimp("pointer to member type");
        break;
    case Type::T_DEPENDENTSIZEDARRAY:
        std::cerr << toString(loc) << ": ";
        xunimp("dependent sized array type");
        break;
    case Type::T_LAST_TYPE_TAG:	// Quell warnings.
        break;
    }

    VDEBUG("makeTypeSpecifier done", loc, type->print(std::cerr));
    return type;
}

const llvm::Type* CC2LLVMEnv::makeAtomicTypeSpecifier(SourceLocation loc, AtomicType *at)
{
    const llvm::Type* type = NULL;
    switch (at->getTag()) {
    case AtomicType::T_SIMPLE: {
        SimpleType *st = at->asSimpleType();
        SimpleTypeId id = st->type;
	switch (id) {
	case ST_CHAR:
	case ST_UNSIGNED_CHAR:
	case ST_SIGNED_CHAR:
	case ST_INT:
	case ST_UNSIGNED_INT:
	case ST_LONG_INT:
        case ST_UNSIGNED_LONG_INT:
        case ST_LONG_LONG:              // GNU/C99 extension
	case ST_UNSIGNED_LONG_LONG:     // GNU/C99 extension
        case ST_SHORT_INT:
        case ST_UNSIGNED_SHORT_INT:
	case ST_WCHAR_T:
	case ST_BOOL:
	    // Define an integer  or boolean type.
            VDEBUG("makeAtomicTypeSpecifier int", loc, std::cerr << "size "
                                                                 << at->sizeInBits(TI)
                                                                 << " type "
                                                                 << at->toString());
            type = llvm::IntegerType::get(at->sizeInBits(TI));
            break;
	case ST_FLOAT:
	    type = llvm::Type::FloatTy;
	    break;
	case ST_DOUBLE:
	    type = llvm::Type::DoubleTy;
	    break;
        case ST_LONG_DOUBLE:
	    type = llvm::Type::FP128Ty;	// RICH: Is this right? This depends on the target processor.
	    type = llvm::Type::DoubleTy; // RICH: Treat as double for now.
	    break;
        case ST_VOID:
            type = llvm::Type::VoidTy;
	    break;
        case ST_ELLIPSIS:
	    /* ... is not a type in LLVM. 
	     * We can ignore this because we use acceptsVarargs().
	     */
            break;

	default:
	case ST_FLOAT_COMPLEX:          // GNU/C99 (see doc/complex.txt)
        case ST_DOUBLE_COMPLEX:         // GNU/C99
        case ST_LONG_DOUBLE_COMPLEX:    // GNU/C99
        case ST_FLOAT_IMAGINARY:        // C99
	case ST_DOUBLE_IMAGINARY:       // C99
        case ST_LONG_DOUBLE_IMAGINARY:  // C99
            std::cerr << toString(loc) << ": ";
            xunimp("simple type");
	    break;
        }
	
        break;
    }

    case AtomicType::T_COMPOUND: {
        CompoundType *ct = at->asCompoundType();
	type = compounds.get(ct);
        VDEBUG("makeAtomicTypeSpecifier compound", loc, type->print(std::cerr));
	if (type) {
	    // We already have this one.
	    break;
	}

	// RICH: struct, union, class, other stuff in a class.
	// Create an opaque type to eliminate recursion.
	llvm::PATypeHolder fwd = llvm::OpaqueType::get();
        // Get the type pointer.
        type = llvm::cast<llvm::Type>(fwd.get());
	// Add this to the compound map now so we don't recurse.
        compounds.add(ct, type);

	std::vector<const llvm::Type*>fields;
        if (ct->keyword == CompoundType::K_UNION) {
	    // Have a union, the first member is used, but
            // the second (unused) member is used to pad
            // to the size of the largest member.
            uint64_t size = 0;
            const llvm::Type* type = NULL;
            SFOREACH_OBJLIST(Variable, ct->dataMembers, iter) {
                Variable const *v = iter.data();
                VDEBUG("union member", v->loc, std::cerr << v->toString());
	        const llvm::Type* mtype = makeTypeSpecifier(v->loc, v->type);
                uint64_t msize = TD.getTypeSizeInBits(mtype);
                if (msize > size) {
                    size = msize;
                }
                const llvm::IntegerType* itype = llvm::IntegerType::get(TI.IntWidth());
                members.add(v, llvm::ConstantInt::get(itype, 0));
                if (type == NULL) {
                    // Remember the first type.
                    type = mtype;
                }
            }

            if (type) {
                // Have the first member.
	        fields.push_back(type);
                size -= TD.getTypeSizeInBits(type);
                if (size) {
                    // Pad for the largest member with a char array.
                    size /= TI.CharWidth();             // Size in bytes.
                    const llvm::Type* etype = llvm::IntegerType::get(TI.CharWidth());
                    const llvm::Type* ptype = llvm::ArrayType::get(etype, size);
	            fields.push_back(ptype);
                }
            }
        } else {
	    // Get the non-static data members.
            int i = 0;
            SFOREACH_OBJLIST(Variable, ct->dataMembers, iter) {
                Variable const *v = iter.data();
                VDEBUG("member", v->loc, std::cerr << v->toString());
                const llvm::IntegerType* itype = llvm::IntegerType::get(TI.IntWidth());
                members.add(v, llvm::ConstantInt::get(itype, i++));
	        fields.push_back(makeTypeSpecifier(v->loc, v->type));
            }
        }

	llvm::StructType* st = llvm::StructType::get(context, fields, false);	// RICH: isPacked
        llvm::cast<llvm::OpaqueType>(fwd.get())->refineAbstractTypeTo(st);
	type = llvm::cast<llvm::Type>(fwd.get());
        VDEBUG("makeAtomicTypeSpecifier compound done", loc, type->print(std::cerr));
        compounds.add(ct, type);

        // Now, look for static members and methods.
        for(StringRefMap<Variable>::Iter iter(ct->getVariableIter()); !iter.isDone(); iter.adv()) {
            Variable *v = iter.value();
            if (members.get(v)) {
                // POD member.
                continue;
            }
            
            if (v->isOverloaded()) {
                // This has been overloaded.
                SFOREACH_OBJLIST(Variable, v->overload->set, iter) {
                    declaration(iter.data(), NULL, 0);
                }
            } else {
                declaration(v, NULL, 0);
            }
        }

        // Now, define function bodies.
        for(StringRefMap<Variable>::Iter iter(ct->getVariableIter()); !iter.isDone(); iter.adv()) {
            Variable *v = iter.value();
            if (v->isOverloaded()) {
                // This has been overloaded.
                SFOREACH_OBJLIST(Variable, v->overload->set, iter) {
                    if (iter.data()->funcDefn) {
                        iter.data()->funcDefn->cc2llvm(*this);
                    }
                }
            } else {
                // A function definition.
                if (v->funcDefn) {
                    v->funcDefn->cc2llvm(*this);
                }
            }
        }

        break;
    }

    case AtomicType::T_ENUM: {
        EnumType* et = at->asEnumType();
        type = llvm::IntegerType::get(et->sizeInBits(TI));
        break;
    }
      
    case AtomicType::T_TYPEVAR:
    case AtomicType::T_PSEUDOINSTANTIATION:
    case AtomicType::T_DEPENDENTQTYPE:
    case AtomicType::T_TEMPLATETYPEVAR:
        std::cerr << toString(loc) << ": ";
        xunimp("template-related");
        break;

    case AtomicType::NUM_TAGS:
        break;
    }

    VDEBUG("makeAtomicTypeSpecifier done", loc, type->print(std::cerr));
    return type;
}

const char* CC2LLVMEnv::variableName(Variable const *v)
{
    if (v->asmname) {
        return TI.getNormalizedGCCRegisterName(v->asmname);
    }
    return v->name;
}

const llvm::Type* CC2LLVMEnv::makeParameterTypes(FunctionType *ft,
                                                 const llvm::Type* returnType,
                                                 std::vector<const llvm::Type*>& args)
{
#if SRET
    if (returnType->getTypeID() == llvm::Type::StructTyID) {
        // RICH: sret
        // We are returning a structure, turn it into a sret pointer.
        const llvm::Type *ptr = llvm::PointerType::get(returnType, 0);	// RICH: address space.
        args.push_back(ptr);
        returnType = llvm::Type::VoidTy;
    }
#endif
    SFOREACH_OBJLIST(Variable, ft->params, iter) {
        Variable const *param = iter.data();

	const llvm::Type* type = makeTypeSpecifier(param->loc, param->type);
	// type will be NULL if a "..." is encountered in the parameter list.
	if (type) {
            VDEBUG("makeParameters", param->loc, type->print(std::cerr));
            args.push_back(type);
        }
    }

    return returnType;
}

llvm::Value* CC2LLVMEnv::declaration(const Variable* var, llvm::Value* init, int deref)
{
    llvm::Value* value = NULL;
    // Create the full generated declaration.
    const llvm::Type* type = makeTypeSpecifier(var->loc, var->type);
    VDEBUG("declaration", var->loc, std::cerr << toString(var->flags) << " " << var->toString());
    if (var->type->getTag() == Type::T_FUNCTION) {
#if RICH
        if (!var->hasFlag(DF_REFERENCED)) {
            // Never referenced.
            return NULL;
        }
#endif
        llvm::GlobalValue::LinkageTypes linkage = getLinkage(var->flags);

        if (linkage == llvm::GlobalValue::InternalLinkage && var->funcDefn == NULL) {
            // No definition exists for this static function.
            return NULL;
        }
        llvm::GlobalVariable* gv = (llvm::GlobalVariable*)variables.get(var);   // RICH: cast
        if (gv == NULL) {
            llvm::Function* gf = llvm::Function::Create((llvm::FunctionType*)type,
                                                        linkage,
                                                        variableName(var),
                                                        mod);
            gf->setCallingConv(llvm::CallingConv::C); // RICH: Calling convention.
            gf->setDoesNotThrow();                  // RICH: When known.
            variables.add(var, gf);
            value = gf;
        } else {
            value = gv;
        }
    } else if (var->type->getTag() == Type::T_DEPENDENTSIZEDARRAY) {
        std::cerr << toString(var->loc) << ": ";
        xunimp("dependent sized array");
    } else if (var->type->getTag() == Type::T_LAST_TYPE_TAG) {
        std::cerr << toString(var->loc) << ": ";
        xunimp("last type tag");
    } else if (var->flags & (DF_STATIC|DF_GLOBAL|DF_EXTERN)) {
        // A global variable.
        if (!(var->flags & DF_EXTERN) && init == NULL) {
            init = llvm::Constant::getNullValue(type);
        }
        llvm::GlobalVariable* gv = (llvm::GlobalVariable*)variables.get(var);   // RICH: cast
        VDEBUG("global type", var->loc, type->print(std::cerr));
        if (init) {
            VDEBUG("global initializer", var->loc, init->print(std::cerr));
        }
        if (gv == NULL) {
            gv = new llvm::GlobalVariable(*mod, type, false,	// RICH: isConstant
                    getLinkage(var->flags), (llvm::Constant*)init, variableName(var));	// RICH: cast
            variables.add(var, gv);
        } else {
            if (init) {
                gv->setInitializer((llvm::Constant*)init);
            }
        }
        value = gv;
    } else if (var->flags & DF_TYPEDEF) {
        // Nothing.
    } else if (var->flags & (DF_DEFINITION|DF_TEMPORARY)) {
        // A local variable.
        xassert(entryBlock);
        llvm::AllocaInst* lv;
        if (entryBlock == currentBlock) {
            lv = new llvm::AllocaInst(type, variableName(var), entryBlock);
        } else {
            lv = new llvm::AllocaInst(type, variableName(var), entryBlock->getTerminator());
        }

        if (init) {
            doassign(var->loc, lv, 1, var->type, init, deref, var->type);
        }
        variables.add(var, lv);
        value = lv;
    } else {
        xunimp("declaration type");
    }

    return value;
}
        
void CC2LLVMEnv::constructor(llvm::Value* object, Statement* ctorStatement)
{
    // Handle the constructor.
    xassert(ctorStatement->kind() == Statement::S_EXPR);
    Expression* expr = ctorStatement->asS_expr()->expr->expr;
    xassert(expr->kind() == Expression::E_CONSTRUCTOR);
    E_constructor* cons = expr->asE_constructor();
    constructor(object, cons);
}
      
void CC2LLVMEnv::constructor(llvm::Value* object, const E_constructor* cons)
{
    VDEBUG("constructor", cons->loc, std::cerr << cons->asString() << " " << cons->ctorVar->toString());
    VDEBUG("constructor retObj", cons->loc, std::cerr << cons->retObj->asString());
    std::vector<llvm::Value*> parameters;
    // RICH: int deref = 0;
    // RICH: object = access(object, false, deref);                 // RICH: Volatile.
    parameters.push_back(object);
    FAKELIST_FOREACH(ArgExpression, cons->args, arg) {
        int deref;
        llvm::Value* param = arg->expr->cc2llvm(*this, deref);
        VDEBUG("Constructor", arg->expr->loc, param->print(std::cerr));
        if (accessValue(param)) {
            param = access(param, false, deref);                 // RICH: Volatile.
        }
        VDEBUG("Constructor after", arg->expr->loc, param->print(std::cerr));
        if (   param->getType()->getTypeID() == llvm::Type::ArrayTyID
                || (   param->getType()->getTypeID() == llvm::Type::PointerTyID
                    && param->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID)) {
            // This is somewhat of a hack: It should be done in cc_tcheck.cc.
            // Add an implicit cast of &array to *array.
            const llvm::Type* type = llvm::PointerType::get(param->getType()->getContainedType(0)->getContainedType(0), 0); // RICH: Address space.
            param = builder.CreateBitCast(param, type);
        }
        parameters.push_back(param);
        VDEBUG("Param", arg->expr->loc, param->print(std::cerr));
    }

    llvm::Value* function = variables.get(cons->ctorVar);
    xassert(function && "An undeclared constructor has been referenced");
    // RICH: deref = 0;
    // RICH: function = access(function, false, deref);                 // RICH: Volatile.
    VDEBUG("CreateCall constructor", cons->ctorVar->loc, function->print(std::cerr));
    builder.CreateCall(function, parameters.begin(), parameters.end());
}

// -------------------- TopForm --------------------
void TopForm::cc2llvm(CC2LLVMEnv &env) const
{
    ASTSWITCHC(TopForm, this) {
    ASTCASEC(TF_decl, d) {
        d->decl->cc2llvm(env);
    } // end of TF_decl

    ASTNEXTC(TF_func, f) {
        f->f->cc2llvm(env);
    }

    ASTNEXTC(TF_linkage, l) {
        // RICH: linkage
        FOREACH_ASTLIST(TopForm, l->forms->topForms, iter) {
            iter.data()->cc2llvm(env);
        }
    }

    ASTNEXTC(TF_one_linkage, l) {
        l->form->cc2llvm(env);
    }

    ASTNEXTC(TF_asm, a) {
        env.mod->appendModuleInlineAsm((const char*)a->def->text->data->getDataC());
    }

    ASTDEFAULTC {
    }
    
    ASTENDCASE
    }
}

void Function::cc2llvm(CC2LLVMEnv &env) const
{
    // Did we see this declaration?
    env.function = (llvm::Function*)env.variables.get(nameAndParams->var);      // RICH: cast, check if func.
    const llvm::Type* returnType;
    llvm::GlobalValue::LinkageTypes linkage = getLinkage(nameAndParams->var->flags);
    returnType = env.makeTypeSpecifier(nameAndParams->var->loc, funcType->retType);
    std::vector<const llvm::Type*>args;
    returnType = env.makeParameterTypes(funcType, returnType, args);
    bool isMain = env.string_main == nameAndParams->var->name;
    llvm::FunctionType* ft = llvm::FunctionType::get(returnType, args,
        isMain || funcType->acceptsVarargs() || (funcType->flags & FF_NO_PARAM_INFO));
    if (env.function && env.function->getType() != (llvm::Type*)ft) {
        // Make the old one anonymous.
        env.function->setName("");
    }
    llvm::Function* function = llvm::Function::Create(ft, linkage,
                                                      env.variableName(nameAndParams->var), env.mod);
    function->setCallingConv(llvm::CallingConv::C); // RICH: Calling convention.
    function->setDoesNotThrow();                  // RICH: When known.
    if (env.function && env.function->getType() != (llvm::Type*)ft) {
        // A declaration exists.
        env.function->uncheckedReplaceAllUsesWith(function);
        env.function->eraseFromParent();
    }

    env.function = function;
    env.function->setCallingConv(llvm::CallingConv::C); // RICH: Calling convention.
    env.variables.add(nameAndParams->var, env.function);

    VDEBUG("Function", nameAndParams->var->loc, std::cerr << nameAndParams->var->toString() << " "; returnType->print(std::cerr));
    const Function* oldFunctionAST = env.functionAST;	// Handle nested functions.
    env.functionAST = this;
    env.entryBlock = llvm::BasicBlock::Create("entry", env.function, NULL);

    // Set the initial current block.
    env.setCurrentBlock(env.entryBlock);

    llvm::Function::arg_iterator llargs = env.function->arg_begin();
    env.returnValue = NULL;

#if SRET
    if (funcType->retType->isCompoundType()) {
        const llvm::Type *ptr = llvm::PointerType::get(env.makeTypeSpecifier(nameAndParams->var->loc, funcType->retType),
                                                       0);        // RICH: address space.
        llvm::Value* arg = llargs++;
	llvm::AllocaInst* addr = env.builder.CreateAlloca(ptr, NULL, "sret");
        env.builder.CreateStore(arg, addr, false);	// RICH: IsVolatile.
        env.returnValue = addr;
    }
#endif

    // Add the parameter names.
    bool first = true;
    SFOREACH_OBJLIST(Variable, funcType->params, iter) {
        const Variable *param = iter.data();
        llvm::Value* arg = llargs++;
        // Make space for the argument.
        const llvm::Type* type = arg->getType();
        VDEBUG("Function arg", param->loc, type->print(std::cerr));
        if (first && receiver && param != receiver) {
            // Yuck! The receiver is not explicit. I think it should be.
            llvm::AllocaInst* addr = env.builder.CreateAlloca(type, NULL, env.variableName(receiver));
            // Remember where the argument can be retrieved.
            env.variables.add(receiver, addr);
            // Store the argument for later use.
            VDEBUG("Function receiver source", receiver->loc, arg->print(std::cerr));
            VDEBUG("Function receiver destination", receiver->loc, addr->print(std::cerr));
            env.builder.CreateStore(arg, addr, false);	// RICH: IsVolatile.

            // Do the next argument.
            arg = llargs++;
            type = arg->getType();
        }

        first = false;
	// type will be NULL for "...".
	if (type) {
	    llvm::AllocaInst* addr = env.builder.CreateAlloca(type, NULL, env.variableName(param));
	    // Remember where the argument can be retrieved.
            env.variables.add(param, addr);
	    // Store the argument for later use.
            VDEBUG("Function arg source", param->loc, arg->print(std::cerr));
            VDEBUG("Function arg destination", param->loc, addr->print(std::cerr));
	    env.builder.CreateStore(arg, addr, false);	// RICH: IsVolatile.
        }
    }

    if (inits->isNotEmpty()) {
        VDEBUG("member init for", nameAndParams->var->loc, std::cerr << nameAndParams->var->toString());
        FAKELIST_FOREACH(MemberInit, inits, init) {
            if (init->member) {
                VDEBUG("member init", init->member->loc, std::cerr << init->member->toString());
            }
            if(init->base) {
                VDEBUG("base init", nameAndParams->var->loc, std::cerr << init->base->toString());
            }

            Expression *expr = NULL;
            FAKELIST_FOREACH(ArgExpression, init->args, arg) {
                VDEBUG("member init arg", arg->expr->loc, std::cerr << arg->expr->asString());
                expr = arg->expr;
            }

            xassert(receiver && "no receiver");

            // Compute the object address.
            // "this"
            E_this *ths = new E_this(EXPR_LOC1(nameAndParams->var->loc ENDLOCARG(SL_UNKNOWN)));
            ths->receiver = receiver;
            // "*this"
            E_deref *deref = new E_deref(EXPR_LOC(nameAndParams->var->loc ENDLOCARG(SL_UNKNOWN)) ths);
            deref->type = receiver->type;
            // "(*this).member
            E_fieldAcc *efieldacc = new E_fieldAcc(EXPR_LOC(nameAndParams->var->loc ENDLOCARG(SL_UNKNOWN)) deref, new PQ_variable(nameAndParams->var->loc, init->member));
            efieldacc->type = init->member->type;
            efieldacc->field = init->member;

            if (init->ctorStatement) {
                int der;
                llvm::Value* object = efieldacc->cc2llvm(env, der);
                env.constructor(object, init->ctorStatement);
            } else if (init->ctorVar) {
                std::cerr << toString(nameAndParams->var->loc) << ": ";
                xunimp("member ctorVar");
            } else {
                xassert(expr && "no constructor argument");
                // Copy the member via an assignment.
                // RICH: This isn't right. And I don't like it.
                // RICH: Refactor to do the assign directly without building new nodes.

                // "(*this).y = other.y"
                // use the E_assign built-in operator
                Expression* action = new E_assign(EXPR_LOC(expr->loc ENDLOCARG(SL_UNKNOWN)) efieldacc, BIN_ASSIGN, expr);
                action->type = expr->type;
                expr = action;
                int der;
                expr->cc2llvm(env, der);
            }
        }
    }

    if (handlers->isNotEmpty()) {
        std::cerr << toString(nameAndParams->var->loc) << ": ";
        xunimp("exception handlers");
    }

    // Set up the return block.
    env.returnBlock = llvm::BasicBlock::Create("return", env.function, 0);
    // Set up the return value.
    if (returnType == llvm::Type::VoidTy) {
        // A void function.
        llvm::ReturnInst::Create(env.returnBlock);
    } else {
        // Create the return value holder.
	
        env.returnValue = env.builder.CreateAlloca(returnType, NULL, "retval");

        if (isMain) {
            // Default return value.
            llvm::Constant* nullInt = llvm::Constant::getNullValue(returnType);
            VDEBUG("Store4 source", nameAndParams->var->loc, nullInt->print(std::cerr));
            VDEBUG("Store4 destination", nameAndParams->var->loc, env.returnValue->print(std::cerr));
            env.builder.CreateStore(nullInt, env.returnValue, false);
        }

        // Generate the function return.
        llvm::LoadInst* rv = new llvm::LoadInst(env.returnValue, "", false, env.returnBlock);	// RICH: Volatile
        llvm::ReturnInst::Create(rv, env.returnBlock);
    }

    // Clear function specific data.
    env.labels.empty();

    // Translate the body.
    body->cc2llvm(env);

    if (env.currentBlock) {
      // Close the current block.
      env.builder.CreateBr(env.returnBlock);
      env.currentBlock = NULL;
    }

    env.function = NULL;
    env.functionAST = oldFunctionAST;	// Restore any old function context.
}

// ----------------------- Declaration -------------------------
void Declaration::cc2llvm(CC2LLVMEnv &env) const
{
    // No need to explicitly process 'spec'.  Even if it introduces
    // new types, they will be defined in the output on demand when
    // something later uses them.

    // If 'typedef' is specified, then the declarators just
    // introduce types, which we have already taken care of and
    // incorporated into the Type system during type checking.
    if (dflags & DF_TYPEDEF) {
        return;
    }

    FAKELIST_FOREACH(Declarator, decllist, declarator) {
        // At this point we exclusively consult the Type and Variable
        // system; syntax is irrelevant, only semantics matters.
        Variable *var = declarator->var;

        // Get any initializer.        
        int deref;
        llvm::Value* init = env.initializer(declarator->init, var->type, deref, true);
        // Process the declaration.
        llvm::Value* object = env.declaration(var, init, deref);
        if (object && declarator->ctorStatement) {
            // Handle a constructor.
            env.constructor(object, declarator->ctorStatement);
        }

        // Elaborated statements are relevant; for now, fail if
        // they are present.
        if (declarator->ctorStatement || declarator->dtorStatement) {
            // RICH: declarator->ctorStatement->cc2llvm(env);
            // RICH: declarator->dtorStatement->cc2llvm(env);
            // RICH: std::cerr << toString(var->loc) << ": ";
            // RICH: xunimp("ctorStatement or dtorStatement");
        }
    }
}

// -------------------- Statement --------------------
void S_skip::cc2llvm(CC2LLVMEnv &env) const
{ }

void S_label::cc2llvm(CC2LLVMEnv &env) const
{
    StringRef label = env.str(name);
    llvm::BasicBlock* block = env.labels.get(label);
    if (block == NULL) {
        // Create the basic block here.
        block = llvm::BasicBlock::Create(label, env.function, env.returnBlock);
        // Remember the label.
        env.labels.add(label, block);
    } else {
        // This was forward referenced.
        block->moveBefore(env.returnBlock);
    }

    env.setCurrentBlock(block);
    s->cc2llvm(env);
}

void S_case::cc2llvm(CC2LLVMEnv &env) const
{
    xassert(env.switchInst);
    llvm::BasicBlock* block = llvm::BasicBlock::Create("case", env.function, env.returnBlock);
    env.setCurrentBlock(block);
    int deref;
    llvm::Value* value = expr->cc2llvm(env, deref);
    value = env.access(value, false, deref);                 // RICH: Volatile.

    env.makeCast(loc, expr->type, value, env.switchType);
    // LLVM will complain if the value is not a constant integer.
    // Not clearly. The verifier complains about dominators.
    env.switchInst->addCase((llvm::ConstantInt*)value, block);
    s->cc2llvm(env);
}

void S_default::cc2llvm(CC2LLVMEnv &env) const
{
    xassert(env.switchInst);
    llvm::BasicBlock* block = llvm::BasicBlock::Create("default", env.function, env.returnBlock);
    env.setCurrentBlock(block);
    env.switchInst->setSuccessor(0, block);
    s->cc2llvm(env);
}

void S_expr::cc2llvm(CC2LLVMEnv &env) const
{
    VDEBUG("Expr", loc, std::cerr << expr->expr->asString());
    int deref;
    expr->cc2llvm(env, deref);
    // RICH: deref? volatile?
}

void S_compound::cc2llvm(CC2LLVMEnv &env) const
{
    FOREACH_ASTLIST(Statement, stmts, iter) {
      iter.data()->cc2llvm(env);
    }
}

void S_if::cc2llvm(CC2LLVMEnv &env) const
{
    const CN_expr* condition = cond->asCN_exprC();
    llvm::Value* value = env.checkCondition(condition->expr->expr);
    llvm::BasicBlock* ifTrue = llvm::BasicBlock::Create("ifTrue", env.function, env.returnBlock);
    llvm::BasicBlock* ifFalse = llvm::BasicBlock::Create("ifFalse", env.function, env.returnBlock);
    llvm::BasicBlock* next = llvm::BasicBlock::Create("next", env.function, env.returnBlock);

    env.checkCurrentBlock();
    env.builder.CreateCondBr(value, ifTrue, ifFalse);
    env.currentBlock = NULL;

    env.setCurrentBlock(ifTrue);
    thenBranch->cc2llvm(env);
    if (env.currentBlock) {
        // Close the current block.
        llvm::BranchInst::Create(next, env.currentBlock);
        env.currentBlock = NULL;
    }


    env.setCurrentBlock(ifFalse);
    elseBranch->cc2llvm(env);

    env.setCurrentBlock(next);
}

void S_switch::cc2llvm(CC2LLVMEnv &env) const
{
    // Save the current switch context.
    llvm::SwitchInst* oldSwitchInst = env.switchInst;
    Type* oldSwitchType = env.switchType;
    llvm::BasicBlock* oldNextBlock = env.nextBlock;
    // Create a block to follow the switch statement.
    env.nextBlock = llvm::BasicBlock::Create("next", env.function, env.returnBlock);
    // Generate the condition.
    const CN_expr* condition = cond->asCN_exprC();
    int deref;
    llvm::Value* value = condition->expr->expr->cc2llvm(env, deref);
    value = env.access(value, false, deref);                 // RICH: Volatile.

    /* Not much to do here. We create the switch instruction and place it in the environment.
     * Subsequent case statements will add to the switch instruction.
     * Subsequent break statements will branch to the next block.
     * A subsequent default statement will change the default target (setSuccessor(0, <default block>).
     */
    env.checkCurrentBlock();
    // We'll use the next block as the default for now. May be overridden by a default:.
    env.switchInst = env.builder.CreateSwitch(value, env.nextBlock);
    env.switchType = condition->expr->expr->type;
    // Close the current block.
    env.currentBlock = NULL;

    // Do the body.
    branches->cc2llvm(env);

    env.setCurrentBlock(env.nextBlock);

    // Restore the old switch context;
    env.switchInst = oldSwitchInst;
    env.switchType = oldSwitchType;
    env.nextBlock = oldNextBlock;
}

void S_while::cc2llvm(CC2LLVMEnv &env) const
{
    // Save the old loop context.
    llvm::BasicBlock* oldContinueBlock = env.continueBlock;
    llvm::BasicBlock* oldNextBlock = env.nextBlock;

    env.continueBlock = llvm::BasicBlock::Create("continue", env.function, env.returnBlock);
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create("body", env.function, env.returnBlock);
    env.nextBlock = llvm::BasicBlock::Create("next", env.function, env.returnBlock);

    // Set the current block.
    env.setCurrentBlock(env.continueBlock);

    // Generate the test.
    const CN_expr* condition = cond->asCN_exprC();
    llvm::Value* value = env.checkCondition(condition->expr->expr);
    env.builder.CreateCondBr(value, bodyBlock, env.nextBlock);
    env.currentBlock = NULL;

    env.setCurrentBlock(bodyBlock);
    body->cc2llvm(env);
    if (env.currentBlock) {
        env.builder.CreateBr(env.continueBlock);
        env.currentBlock = NULL;
    }
    env.setCurrentBlock(env.nextBlock);

    // Restore the old loop context.
    env.continueBlock = oldContinueBlock;
    env.nextBlock = oldNextBlock;
}

void S_doWhile::cc2llvm(CC2LLVMEnv &env) const
{
    // Save the old loop context.
    llvm::BasicBlock* oldContinueBlock = env.continueBlock;
    llvm::BasicBlock* oldNextBlock = env.nextBlock;

    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create("body", env.function, env.returnBlock);
    env.continueBlock = llvm::BasicBlock::Create("continue", env.function, env.returnBlock);
    env.nextBlock = llvm::BasicBlock::Create("next", env.function, env.returnBlock);
    if (env.currentBlock) {
        // Close the current block.
        env.builder.CreateBr(bodyBlock);
    }
    env.currentBlock = NULL;

    // Set the current block.
    env.setCurrentBlock(bodyBlock);
    body->cc2llvm(env);

    // Generate the test.
    env.setCurrentBlock(env.continueBlock);
    llvm::Value* value = env.checkCondition(expr->expr);
    env.builder.CreateCondBr(value, bodyBlock, env.nextBlock);
    env.currentBlock = NULL;

    env.setCurrentBlock(env.nextBlock);

    // Restore the old loop context.
    env.continueBlock = oldContinueBlock;
    env.nextBlock = oldNextBlock;
}

void S_for::cc2llvm(CC2LLVMEnv &env) const
{
    // Save the old loop context.
    llvm::BasicBlock* oldContinueBlock = env.continueBlock;
    llvm::BasicBlock* oldNextBlock = env.nextBlock;

    llvm::BasicBlock* testBlock = llvm::BasicBlock::Create("test", env.function, env.returnBlock);
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create("body", env.function, env.returnBlock);
    env.continueBlock = llvm::BasicBlock::Create("continue", env.function, env.returnBlock);
    env.nextBlock = llvm::BasicBlock::Create("next", env.function, env.returnBlock);

    env.checkCurrentBlock();
    // Handle the for() initialization.
    init->cc2llvm(env);

    // Set the current block.
    env.setCurrentBlock(testBlock);

    // Generate the test.
    const CN_expr* condition = cond->asCN_exprC();
    VDEBUG("S_for cond", loc, std::cerr << condition->expr->expr->asString());
    llvm::Value* value = env.checkCondition(condition->expr->expr);
    env.builder.CreateCondBr(value, bodyBlock, env.nextBlock);
    env.currentBlock = NULL;

    env.setCurrentBlock(bodyBlock);
    body->cc2llvm(env);

    // Handle the continue block.
    env.setCurrentBlock(env.continueBlock);
    int deref;
    after->cc2llvm(env, deref);
    // RICH: deref? volatile?
    env.builder.CreateBr(testBlock);
    env.currentBlock = NULL;

    env.setCurrentBlock(env.nextBlock);

    // Restore the old loop context.
    env.continueBlock = oldContinueBlock;
    env.nextBlock = oldNextBlock;
}

void S_break::cc2llvm(CC2LLVMEnv &env) const
{
    xassert(env.nextBlock && "break not in a loop or switch");
    env.checkCurrentBlock();
    env.builder.CreateBr(env.nextBlock);
    env.currentBlock = NULL;
}

void S_continue::cc2llvm(CC2LLVMEnv &env) const
{
    xassert(env.continueBlock && "continue not in a loop");
    env.checkCurrentBlock();
    env.builder.CreateBr(env.continueBlock);
    env.currentBlock = NULL;
}

void S_return::cc2llvm(CC2LLVMEnv &env) const
{
    env.checkCurrentBlock();
    if (expr) {
        // A return value is specified.
        VDEBUG("Return", loc, std::cerr << expr->expr->asString());
        xassert(env.returnValue && "return a value in a function returning void");
        int deref;
        llvm::Value* value = expr->cc2llvm(env, deref);
        VDEBUG("Return type", loc, std::cerr << expr->expr->type->toString() << " deref " << deref);
        if (   !expr->expr->type->isPointer()
            && value->getType()->getTypeID() == llvm::Type::PointerTyID
            && (   value->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID
                || value->getType()->getContainedType(0)->getTypeID() == llvm::Type::StructTyID)) {
            if (!llvm::Constant::classof(value)) {
                ++deref;
            }
            value = env.access(value, false, deref, 0); // RICH: Volatile.
        }
        if (deref >= 2 && expr->expr->type->isReference()) {
            // Return a reference as a pointer.
            value = env.access(value, false, deref, 1); // RICH: Volatile.
        } else {
            value = env.access(value, false, deref, 0); // RICH: Volatile.
        }
        VDEBUG("S_return source", loc, value->print(std::cerr));
        if (value->getType()->getTypeID() != llvm::Type::StructTyID) {
            env.makeCast(loc, expr->expr->type, value, env.functionAST->funcType->retType);
        }
        llvm::Value* where = env.returnValue;
#if SRET
        if (env.functionAST->funcType->retType->isCompoundType()) {
            where = env.builder.CreateLoad(where, false);     // RICH: Is volatile.
        }
#endif
        VDEBUG("S_return destination", loc, where->print(std::cerr));
        env.builder.CreateStore(value, where, false);	// RICH: isVolatile
    } else {
        xassert(env.returnValue == NULL && "no return value in a function not returning void");
    }

    env.builder.CreateBr(env.returnBlock);
    env.currentBlock = NULL;
}

void S_goto::cc2llvm(CC2LLVMEnv &env) const
{
    StringRef label = env.str(target);

    env.checkCurrentBlock();
    llvm::BasicBlock* block = env.labels.get(label);
    if (block == NULL) {
        // The label has not been encountered, yet.
        block = llvm::BasicBlock::Create(label, env.function, env.returnBlock);
        // Remember the label.
        env.labels.add(label, block);
    }

    // Close the current block with the goto target.
    env.builder.CreateBr(block);
    env.currentBlock = NULL;
}

void S_decl::cc2llvm(CC2LLVMEnv &env) const
{
    decl->cc2llvm(env);
}

void S_try::cc2llvm(CC2LLVMEnv &env) const
{
    std::cerr << toString(loc) << ": ";
    xunimp("try");
}

void S_asm::cc2llvm(CC2LLVMEnv &env) const
{
    llvm::Value* returnTarget = NULL;           // The return type, if any.
    const llvm::Type* returnType = llvm::Type::VoidTy;
    std::vector<llvm::Value*> args;             // Asm arguments.
    std::vector<const llvm::Type*> argTypes;
    std::vector<llvm::Value*> rwargs;
    std::vector<const llvm::Type*> rwargTypes;
    stringBuilder constraints;
    bool first = true;
    Asm& d = *def;

    if (d.constraints) {
        // Go through the constraints and gather the arguments and types.
        FOREACH_ASTLIST_NC(Constraint, d.constraints->outputs, c) {
            Constraint* constraint = c.data();
            Expression*& expr = constraint->e;
            int deref;
            E_cast* cexpr = expr->ifE_cast();
            if (cexpr) {
                // Handle a GNU lvalue cast.
                expr = cexpr->expr;
            }
            llvm::Value* value = expr->cc2llvm(env, deref);
            value = env.access(value, false, deref, 1);                 // RICH: Volatile.
            if (!first) {
                constraints << ',';
            }
            VDEBUG("S_asm output", loc, value->print(std::cerr));
            if (   first
                && !(constraint->info & TargetInfo::CI_AllowsMemory)
                && value->getType()->isSingleValueType()) {
                // Use this first output constraint as the return type.
                returnTarget = value;
                returnType = llvm::cast<llvm::PointerType>(value->getType())->getElementType();
                --d.numOutputs;
                constraints << "=" << constraint->string.c_str();
            } else {
                args.push_back(value);
                argTypes.push_back(value->getType());
                constraints << "=*" << constraint->string.c_str();
            }
            if (constraint->info & TargetInfo::CI_ReadWrite) {
                value = env.access(value, false, deref);                 // RICH: Volatile.
                VDEBUG("S_asm rw", loc, value->print(std::cerr));
                rwargs.push_back(value);
                rwargTypes.push_back(value->getType());
            }
            first = false;
        }
        if (d.rwInputs.length()) {
           if (!first) {
                constraints << ',';
            } else {
                first = false;
            }
            constraints << d.rwInputs.c_str();
        }
        args.insert(args.end(), rwargs.begin(), rwargs.end());
        argTypes.insert(argTypes.end(), rwargTypes.begin(), rwargTypes.end());
        FOREACH_ASTLIST_NC(Constraint, d.constraints->inputs, c) {
            Constraint* constraint = c.data();
            Expression*& expr = constraint->e;
            int deref;
            llvm::Value* value = expr->cc2llvm(env, deref);
            value = env.access(value, false, deref);                 // RICH: Volatile.
            VDEBUG("S_asm input", loc, value->print(std::cerr));
            args.push_back(value);
            argTypes.push_back(value->getType());
            if (!first) {
                constraints << ',';
            } else {
                first = false;
            }
            constraints << constraint->string.c_str();
        }

        FOREACH_ASTLIST_NC(Constraint, d.constraints->clobbers, c) {
            Constraint* constraint = c.data();
            VDEBUG("S_asm clobber", loc, std::cerr << constraint->string.c_str());
            if (!first) {
                constraints << ',';
            } else {
                first = false;
            }
            constraints << constraint->string.c_str();
        }

        std::string machineClobbers = env.TI.getClobbers();
        if (!machineClobbers.empty()) {
            if (!first) {
                constraints << ',';
            }
            constraints << machineClobbers.c_str();
        }
    }

    stringBuilder asmstr;       // The built-up asm string.
    const char* cp = (const char*)d.text->data->getDataC();
    // Now we can walk through the assembly language string translating it.
    while (*cp) {
        if (*cp == '$') {
            asmstr << "$$";
            ++cp;
            continue;
        }

        if (*cp != '%') {
            // Anything but '%'.
            asmstr << *cp++;
            continue;
        }
            
        // '%'
        ++cp;
        if (*cp == '\0') {
            env.report(d.text->loc, diag::err_asm_end_in_percent);
            continue;
        }

        if (*cp == '%') {
            asmstr << '%';              // Escaped '%' ('%%') becomes '%'.
            ++cp;
            continue;
        }

        if (*cp == '@') {
            asmstr << '@';             // Escaped '@' ('%@') becomes '@', the start of an ARM comment.
            ++cp;
            continue;
        }
        if (*cp == '=') {
            asmstr << "${:uid}";        // Generate a unique ID.
            ++cp;
            continue;
        }

        char modifier = '\0';
        if (isalpha(*cp)) {
            modifier = *cp++;
        }

        if (isdigit(*cp)) {
            unsigned index = 0;
            while (isdigit(*cp)) {
                index = index * 10 + (*cp - '0');
                ++cp;
            }

            if (index < d.numOutputs) {
                if (modifier) {
                    asmstr << "${" << index << ':' << modifier << "}";
                } else {
                    asmstr << '$' << index;
                }
            } else if (d.constraints == NULL || index > d.constraints->inputs.count() + d.numOutputs) {
                env.report(d.text->loc, diag::err_asm_index_has_no_matching_constraint)
                    << index;
            } else {
                // Adjust for '+' output constraints.
                if (modifier) {
                    asmstr << "${" << index + d.rwConstraints - d.numOutputs << ':' << modifier << "}";
                } else {
                    asmstr << '$' << index + d.rwConstraints - d.numOutputs ;
                }
            }
            continue;
        }

        if (*cp == '[') {
            // Translate a position by name.
            const char* p = strchr(++cp, ']') ;
            if (p == NULL) {
                env.report(d.text->loc, diag::err_asm_string_missing_rbracket);
                continue;
            } 
            unsigned index = 0;
            std::string name(cp, p - cp);
            if (d.constraints) {
                FOREACH_ASTLIST_NC(Constraint, d.constraints->outputs, oc) {
                    if (oc.data()->name && name == oc.data()->name) {
                        break;
                    }
                    ++index;
                }
            }
            if (index < d.numOutputs) {
                // An output constraint matched.
                if (modifier) {
                    asmstr << "${" << index << ':' << modifier << "}";
                } else {
                    asmstr << '$' << index;
                }
            } else {
                index = 0;
                if (d.constraints) {
                    FOREACH_ASTLIST_NC(Constraint, d.constraints->inputs, ic) {
                        if (ic.data()->name && name == ic.data()->name) {
                            break;
                        }
                        ++index;
                    }
                }
                if (d.constraints && index < d.constraints->inputs.count() + d.numOutputs) {
                    if (modifier) {
                        asmstr << "${" << index - d.numOutputs + d.rwConstraints << ':' << modifier << "}";
                    } else {
                        asmstr << '$' << index - d.numOutputs + d.rwConstraints;
                    }
                } else {
                    env.report(d.text->loc, diag::err_asm_name_has_no_matching_constraint)
                        << name;
                }
            }

            cp = p + 1;
            continue;
        }
    
        // Bad asm construct.
        env.report(d.text->loc, diag::err_asm_unrecognized_percent);
        ++cp;
    }

    llvm::FunctionType* type = llvm::FunctionType::get(returnType, argTypes, false);
    VDEBUG("S_asm function type", loc, type->print(std::cerr));
    llvm::InlineAsm* function = llvm::InlineAsm::get(type,
                                                     asmstr.c_str(),
                                                     constraints.c_str(),
                                                     !!(d.qualifiers & CV_VOLATILE));
    VDEBUG("S_asm CreateCall call", loc, function->print(std::cerr));
    llvm::CallInst *result = env.builder.CreateCall(function, args.begin(), args.end());
    result->addAttribute(~0, llvm::Attribute::NoUnwind);
    if (returnTarget) {
        env.builder.CreateStore(result, returnTarget);
    }
}

void S_namespaceDecl::cc2llvm(CC2LLVMEnv &env) const 
{
    // should be able to just drop these
}


void S_computedGoto::cc2llvm(CC2LLVMEnv &env) const
{
    std::cerr << toString(loc) << ": ";
    xunimp("computed goto");
}

void S_rangeCase::cc2llvm(CC2LLVMEnv &env) const
{
    std::cerr << toString(loc) << ": ";
    xunimp("range case");
}

void S_function::cc2llvm(CC2LLVMEnv &env) const
{
    xunimp("nested function");
}

// ------------------- Expression --------------------
llvm::Value *E_boolLit::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    if (b)
        return llvm::ConstantInt::getTrue(env.context);
    else
        return llvm::ConstantInt::getFalse(env.context);
}

llvm::Value *E_intLit::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    int radix = 10;
    const char* p = text;
    if (p[0] == '0') {
        // Octal or hex.
        if (tolower(p[1]) == 'x') {
	    radix = 16;
            p = p + 2;
        } else {
	    radix = 8;
        }
    }

    const char* endp = p;
    while (isxdigit(*endp)) ++endp;

    VDEBUG("IntLit", loc, std::cerr << text << " radix " << radix);
    return llvm::ConstantInt::get(env.context,
                                  llvm::APInt(type->sizeInBits(env.TI), p, endp - p, radix));
}

llvm::Value *E_floatLit::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    const llvm::Type* ftype = env.makeTypeSpecifier(loc, type);
    const llvm::fltSemantics* semantics;
    deref = 0;
    switch (ftype->getTypeID()) {
    case llvm::Type::FloatTyID:
        semantics = &llvm::APFloat::IEEEsingle;
        break;
    case llvm::Type::DoubleTyID:
        semantics = &llvm::APFloat::IEEEdouble;
        break;
    case llvm::Type::X86_FP80TyID:
        semantics = &llvm::APFloat::x87DoubleExtended;
        break;
    case llvm::Type::FP128TyID:
        semantics = &llvm::APFloat::IEEEquad;
        break;
    case llvm::Type::PPC_FP128TyID:
        semantics = &llvm::APFloat::PPCDoubleDouble;
        break;
    default:
        std::cerr << toString(loc) << ": ";
        xunimp("floating point type");
        break;
    }
    return llvm::ConstantFP::get(env.context, llvm::APFloat(*semantics, text));
}

llvm::Value *E_stringLit::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::string value((const char*)data->getDataC(), data->getDataLen());	// RICH: cast
    llvm::Constant* c = llvm::ConstantArray::get(value, false);	// Don't add a nul character.
    const llvm::Type* at = env.makeTypeSpecifier(loc, type->asReferenceType()->atType);
    VDEBUG("E_stringLit", loc, at->print(std::cerr));
    // RICH: Non-constant strings?
    llvm::Value* result = new llvm::GlobalVariable(*env.mod, at, true,
                                                   llvm::GlobalValue::InternalLinkage,
                                                   c, ".str");
    return result;
}

llvm::Value *E_charLit::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    return llvm::ConstantInt::get(env.context, llvm::APInt(type->sizeInBits(env.TI), c));
}

llvm::Value *E_this::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 1;
    llvm::Value* value = env.variables.get(receiver);
    xassert(value && "'this' was not defined");
    VDEBUG("E_this", loc, value->getType()->print(std::cerr));
    return value;
}

/** Get a variable used in an expression.
 */
llvm::Value *E_variable::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    if (var->isEnumerator()) {
        // This is an enumerator constant. Return it's value.
        deref = 0;
        return llvm::ConstantInt::get(env.context, llvm::APInt(type->sizeInBits(env.TI),
                                      var->getEnumeratorValue()));
    }

    // The variable will have been previously seen in a declaration.
    llvm::Value* value = env.variables.get(var);
    if (value == NULL) {
        // Declare the variable.
        env.declaration(var, NULL, 0);
        value = env.variables.get(var);
    }
    xassert(value && "An undeclared variable has been referenced");
    xassert(value->getType()->getTypeID() == llvm::Type::PointerTyID && "expected pointer type");
    bool first = value->getType()->getContainedType(0)->isFirstClassType();

    VDEBUG("E_variable ID", loc, std::cerr << value->getType()->getContainedType(0)->getTypeID());

    deref = 0;
    if (!first) {
        // The object is not a first class object.
        deref = 0;
    } else {
        if (   value->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID
            || value->getType()->getContainedType(0)->getTypeID() == llvm::Type::StructTyID) {
            // Arrays are their value.
            deref = 0;
        } else {
            // Need one dereference to get the actual object.
            deref = 1;
        }
    }

    if (var->type->isReference() && (var->flags & DF_PARAMETER)) {
        ++deref;
    }
    VDEBUG("E_variable deref", loc, std::cerr << deref << " " << var->getType()->toString());
    return value;
}

llvm::Value *E_funCall::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    env.checkCurrentBlock();
    std::vector<llvm::Value*> parameters;

    VDEBUG("E_funCall function", loc, std::cerr << func->asString());
    VDEBUG("E_funCall func type", loc, std::cerr << func->type->toString());
    FunctionType *ft;
    if (func->type->isPtrOrRef()) {
        ft = func->type->getAtType()->getAtType()->asFunctionType();
    } else {
        ft = func->type->asFunctionType();
    }

    llvm::Value* object = NULL;			// Non-NULL if a method call.
    // Check for a method call.
    llvm::Value* function = NULL;
    if (func->kind() == E_FIELDACC) {
        VDEBUG("E_funCall method", loc, std::cerr << func->asString());
        E_fieldAcc* fa = func->asE_fieldAcc();
        function = env.variables.get(fa->field);
        if (function) {
            object = fa->obj->cc2llvm(env, deref);
        }
    }

    deref = 0;
    if (!function) {
        // This is not a method call.
        function = func->cc2llvm(env, deref);
    }

#if SRET
    llvm::Value* sret = NULL;			// Non-NULL if a structure return.
    if (ft->retType->isCompoundType()) {
        // We need an implicit first parameter that points to the return value area.
        const llvm::Type *type = env.makeTypeSpecifier(loc, ft->retType);
        VDEBUG("E_funCall sret type", loc, type->print(std::cerr));
        if (env.entryBlock == env.currentBlock) {
            sret = new llvm::AllocaInst(type, "sret", env.entryBlock);
        } else {
            sret = new llvm::AllocaInst(type, "sret", env.entryBlock->getTerminator());
        }
        VDEBUG("E_funCall sret", loc, sret->print(std::cerr));
        parameters.push_back(sret);
    }
#endif

    if (object) {
        // Send the "this" pointer.
        int deref = 0;
        object = env.access(object, false, deref, 1);                 // RICH: Volatile.
        parameters.push_back(object);
    }

    /* I really don't like having to do this, but...
     * We walk both the argument list and the function's parameter list so that we can
     * determine if a given argument is passed by reference.
     */
    
    SObjListIter<Variable> parms(ft->params);
    FAKELIST_FOREACH(ArgExpression, args, arg) {
        const Variable* parameter = NULL;
        if (!parms.isDone()) {
            parameter = parms.data();
            parms.adv();
        }
        bool ref = parameter && parameter->type && parameter->type->isReference();
        int deref = 0;
        llvm::Value* param = arg->expr->cc2llvm(env, deref);
        VDEBUG("Param", loc, std::cerr << (arg->expr->type->isReference() ? "&" : "") << arg->expr->asString() << " "; param->print(std::cerr));
        if (accessValue(param) || arg->expr->isE_fieldAcc()) {
            param = env.access(param, false, deref, ref ? 1 : 0);                 // RICH: Volatile.
        }
        VDEBUG("Param after", loc, param->print(std::cerr));
        if (   param->getType()->getTypeID() == llvm::Type::ArrayTyID
            || (   param->getType()->getTypeID() == llvm::Type::PointerTyID
                && param->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID)) {
            // This is somewhat of a hack: It should be done in cc_tcheck.cc.
            // Add an implicit cast of &array to *array.
            const llvm::Type* type = llvm::PointerType::get(param->getType()->getContainedType(0)->getContainedType(0), 0); // RICH: Address space.
	    param = env.builder.CreateBitCast(param, type);
        } else if (   parameter && parameter->getTypeC()->isCompoundType()
                   && param->getType()->getTypeID() == llvm::Type::PointerTyID
                   && param->getType()->getContainedType(0)->getTypeID() == llvm::Type::StructTyID) {
            param = env.builder.CreateLoad(param, false);     // RICH: Is volatile.
            VDEBUG("Param struct", loc, param->print(std::cerr));
        }
        parameters.push_back(param);
        VDEBUG("Param", loc, param->print(std::cerr));
    }

    if (function->getType()->getContainedType(0)->getTypeID() == llvm::Type::PointerTyID && deref == 0) {
        ++deref;
    }
    function = env.access(function, false, deref);                 // RICH: Volatile.
    VDEBUG("CreateCall call", loc, function->print(std::cerr));
    llvm::Value* result = env.builder.CreateCall(function, parameters.begin(), parameters.end());
#if SRET
    // RICH: sret
    if (sret) {
        // The result is the saved return value.
        result = sret;
    }
#endif
    return result;
}

llvm::Value *E_constructor::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    VDEBUG("E_constructor", loc, std::cerr << ctorVar->toString());
    VDEBUG("E_constructor retObj", loc, std::cerr << retObj->asString());
    FAKELIST_FOREACH(ArgExpression, args, arg) {
        VDEBUG("E_constructor arg", arg->expr->loc, std::cerr << arg->expr->asString());
    }
    
    llvm::Value* object = retObj->cc2llvm(env, deref);
    env.constructor(object, this);
    return object;
}

#ifdef E_FIELDACC_DEBUG
#undef VDEBUG
#define VDEBUG(who, where, what) std::cerr << toString(where) << ": " << who << " "; what; std::cerr << "\n"
#endif
llvm::Value *E_fieldAcc::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    // 'field' is the member variable.
    // The member will have been previously seen in a declaration.
    bool isPointer = false;
    bool isUnion = false;
    isPointer = obj->type->isPtrOrRef();
    if (isPointer) {
        isUnion = obj->type->getAtType()->isUnionType();
    } else {
        isUnion = obj->type->isUnionType();
    }

    VDEBUG("E_field obj", loc, std::cerr << obj->asString());
    VDEBUG("E_field obj type ", loc, std::cerr << obj->type->toString(); std::cerr << "isUnion = " << isUnion);
    llvm::Value* object = obj->cc2llvm(env, deref);
    object = env.access(object, false, deref, 1);                 // RICH: Volatile.
    VDEBUG("E_field field", loc, std::cerr << field->toString());
    llvm::Value* value = env.members.get(field);
    if (value == NULL) {
        // Check for a static member.
        value = env.variables.get(field);
        xassert(value && "An undeclared member has been referenced");
        xassert(value->getType()->getTypeID() == llvm::Type::PointerTyID && "expected pointer type");
        bool first = value->getType()->getContainedType(0)->isFirstClassType();

        VDEBUG("E_fieldAcc ID", loc, std::cerr << value->getType()->getContainedType(0)->getTypeID());

        deref = 0;
        if (!first) {
            // The object is not a first class object.
            deref = 0;
        } else {
            if (value->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID) {
                // Arrays are their value.
                deref = 0;
            } else {
                // Need one dereference to get the actual object.
                deref = 1;
            }
        }

        return value;
    }

    llvm::Value* result;
    if (!isUnion) {
        VDEBUG("E_field object", loc, std::cerr << "ID " << object->getType()->getContainedType(0)->getTypeID() << " ";
            object->print(std::cerr));
        VDEBUG("E_field value", loc, value->print(std::cerr));
        std::vector<llvm::Value*> index;
        if (   object->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID
            || object->getType()->getContainedType(0)->getTypeID() == llvm::Type::StructTyID) {
            index.push_back(llvm::Constant::getNullValue(value->getType()));
        }
        index.push_back(value);
        env.checkCurrentBlock();
        result = env.builder.CreateGEP(object, index.begin(), index.end());
        VDEBUG("E_field  after GEP", loc, result->print(std::cerr));

        bool first = result->getType()->getContainedType(0)->isFirstClassType();
        deref = 0;
        if (!first) {
            // The object is not a first class object.
            deref = 0;
        } else {
            if (llvm::isa<llvm::ArrayType>(result->getType()->getContainedType(0))) {
                deref = 0;
            } else {
                // Need one dereference to get the actual object.
                deref = 1;
            }
        }
    } else {
        const llvm::Type* rtype = env.makeTypeSpecifier(loc, type);
        const llvm::Type* ftype = env.makeTypeSpecifier(loc, field->type);
        bool first = ftype->isFirstClassType();
        if (!first) {
            // The object is not a first class object.
            deref = 0;
        } else {
            if (llvm::isa<llvm::ArrayType>(ftype)) {
                deref = 0;
            } else {
                // Need one dereference to get the actual object.
                deref = 1;
            }
        }
        if (!llvm::isa<llvm::PointerType>(rtype)) {
            // RICH: ++deref;
            rtype = llvm::PointerType::get(rtype, 0);	// RICH: Address space.
        }
        VDEBUG("E_field  rtype", loc, rtype->print(std::cerr));
        VDEBUG("E_field  object", loc, object->print(std::cerr));
        result = env.builder.CreateBitCast(object, rtype);
    }

    VDEBUG("E_field deref", loc, std::cerr << "deref " << deref << " "; result->print(std::cerr));
    return result;
}
#ifdef E_FIELDACC_DEBUG
#undef VDEBUG
#define VDEBUG(who, where, what)
#endif

llvm::Value *E_sizeof::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* value = llvm::ConstantInt::get(env.TD.getIntPtrType(), size);
    const llvm::Type* rtype = env.makeTypeSpecifier(loc, type);
    value = env.builder.CreateIntCast(value, rtype, false);
    deref = 0;
    return value;
}


llvm::Value *E_unary::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* value = expr->cc2llvm(env, deref);
    value = env.access(value, false, deref);                 // RICH: Volatile.

    switch (op) {
    case UNY_PLUS:      // +
        break;		// NO-OP.

    case UNY_MINUS: {   // -
	// Negate the value.
	env.checkCurrentBlock();
        llvm::Value* zero = llvm::Constant::getNullValue(value->getType());
	value = env.builder.CreateSub(zero, value);
        break;
    }

    case UNY_NOT: {     // !
	value = env.checkCondition(loc, value, deref, true);
        break;
    }

    case UNY_BITNOT: {  // ~
        // Negate the integer value.
	env.checkCurrentBlock();
        llvm::Value* ones = llvm::Constant::getAllOnesValue(value->getType());
        value = env.builder.CreateXor(value, ones);
	break;
    }

    case NUM_UNARYOPS:
        break;
    }

    return value;
}

/** Unary expression with side effects.
 */
llvm::Value *E_effect::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* value = expr->cc2llvm(env, deref);
    llvm::Value* result = NULL;
    llvm::Value* temp = NULL;
    
    // Make sure we have the address.
    value = env.access(value, false, deref, 1);                 // RICH: Volatile.
    VDEBUG("E_effect value", loc, std::cerr << "deref " << deref; value->print(std::cerr));

    if (isPostfix(op)) {
        // A postfix operator: return the result from before the operation.
        int myderef = deref;
        result = env.access(value, false, myderef);                 // RICH: Volatile.
        temp = env.access(value, false, deref);                 // RICH: Volatile.
    } else {
        temp = env.access(value, false, deref);                 // RICH: Volatile.
    }

    VDEBUG("E_effect temp", loc, std::cerr << "deref " << deref; temp->print(std::cerr));
    if (temp->getType()->getTypeID() == llvm::Type::PointerTyID) {
        // This is a pointer increment/decrement.
	std::vector<llvm::Value*> index;
	llvm::Value* one;
	if (op == EFF_POSTDEC || op == EFF_PREDEC) {
	    // Get a negative one.
        one = llvm::ConstantInt::get(env.context,
                                     llvm::APInt(env.TD.getTypeSizeInBits(temp->getType()), -1));
	} else {
	    // Get a positive one.
        one = llvm::ConstantInt::get(env.context,
                                     llvm::APInt(env.TD.getTypeSizeInBits(temp->getType()), 1));
	}
	index.push_back(one);
        VDEBUG("GEP1", loc, temp->print(std::cerr); );
	temp = env.builder.CreateGEP(temp, index.begin(), index.end());
        VDEBUG("Store7 source", loc, temp->print(std::cerr));
        VDEBUG("Store7 destination", loc, value->print(std::cerr));
        env.builder.CreateStore(temp, value, false);	// RICH: Volatile
    } else if (temp->getType()->isInteger()) {
        llvm::ConstantInt* one = llvm::ConstantInt::get(env.context,
                                        llvm::APInt(env.TD.getTypeSizeInBits(temp->getType()), 1));
	if (op == EFF_POSTDEC || op == EFF_PREDEC) {
	    temp = env.builder.CreateSub(temp, one);
	} else {
	    temp = env.builder.CreateAdd(temp, one);
	}
        VDEBUG("Store8 source", loc, temp->print(std::cerr));
        VDEBUG("Store8 destination", loc, value->print(std::cerr));
        env.builder.CreateStore(temp, value, false);	// RICH: Volatile
    } else {
        std::cerr << toString(loc) << ": ";
        xunimp("++,--");
    }

    if (!isPostfix(op)) {
        result = temp;
    }
    deref = 0;
    return result;
}

/** Make two operand the same type or cast the first operand to the second type.
 * We use both the AST and LLVM information to do this.
 */
CC2LLVMEnv::OperatorClass CC2LLVMEnv::makeCast(SourceLocation loc, Type* leftType,
    llvm::Value*& leftValue, Type* rightType, llvm::Value** rightValue)
{
    // For operators, we need simple types or pointers.
    // At this point we can treat a reference as its inderlying type.
    // RICH: This may be wrong.
    if (leftType->isReference()) {
	leftType = leftType->asReferenceType()->atType;
    }
    if (rightType->isReference()) {
	rightType = rightType->asReferenceType()->atType;
    }

    /** This structure lets us gather information about an operand.
     * The information is used to determine how to cast and classify
     * the operand.
     */
    struct Data {
        bool isPointer;
        bool isSimple;
	    bool isInteger;
	    bool isUnsigned;
	    bool isFloat;
	    bool isVoid;
	    int size;
	    Type * type;
        const llvm::Type* llvmType;
	    llvm::Value** value;
        TargetInfo& TI;
        Data(TargetInfo& TI, Type* type, llvm::Value** value, const llvm::Type* llvmType = NULL) 
            : type(type), llvmType(llvmType), value(value), TI(TI) {
            // Information needed convert and classify.
            if (type->isReference()) {
                type = type->getAtType();
            }
            isPointer = type->isPtrOrRef() || type->isArrayType() || type->isFunctionType();
            isSimple = type->isSimpleType();
	    isInteger = false; isUnsigned = false; isFloat = false; isVoid = false; size = 0;
	    if (type->isEnumType()) {
	      // We have to handle enumerated constants specially.
	      isInteger = true;
	      EnumType* etype = type->asCVAtomicType()->atomic->asEnumType();
	      isUnsigned = !etype->hasNegativeValues;
	      size = etype->sizeInBits(TI);
	      type = NULL;	// See special handling below.
	    } else {
                xassert(isSimple || isPointer);
	    }
            if (isSimple) {
	        const SimpleType* st = type->asSimpleTypeC();
                isInteger = ::isIntegerType(st->type);
                isUnsigned = ::isExplicitlyUnsigned(st->type);
                isFloat = ::isFloatType(st->type);
                isVoid = type->isVoid();
                if (isInteger && value) {
                    // Use the LLVM integer size.
                    size = (*value)->getType()->getPrimitiveSizeInBits();
                } else {
                    size = simpleTypeSizeInBits(TI, st->type);
                }
            } else if (isPointer) {
                size = type->sizeInBits(TI);
            }
	}
    };

    const llvm::Type* llvmType = NULL;
    if (leftType->isArrayType()) {
        // An array becomes a pointer to the first element.
        std::vector<llvm::Value*> indices;
        indices.push_back(llvm::Constant::getNullValue(TD.getIntPtrType()));
        leftValue = builder.CreateGEP(leftValue, indices.begin(), indices.end(), "");
        VDEBUG("makeCast array type left ", loc, leftValue->print(std::cerr));
        llvmType = leftValue->getType();
    } else if (leftType->isFunctionType()) {
        // A function becomes a pointer to the function.
        VDEBUG("makeCast function type left ", loc, leftValue->print(std::cerr));
        llvmType = leftValue->getType();
        llvmType = llvm::PointerType::get(leftValue->getType(), 0);       // RICH: Address space.
    }
    Data left(TI, leftType, &leftValue, llvmType);

    llvmType = NULL;
    if (rightType->isArrayType()) {
        if (rightValue) {
            // An array becomes a pointer to the first element.
            std::vector<llvm::Value*> indices;
            indices.push_back(llvm::Constant::getNullValue(TD.getIntPtrType()));
            *rightValue = builder.CreateGEP(*rightValue, indices.begin(), indices.end(), "");
            VDEBUG("makeCast array type right ", loc, (*rightValue)->print(std::cerr));
            llvmType = (*rightValue)->getType();
        } else {
            // Build the type as pointer to element type.
            ArrayType *at = rightType->asArrayType();
            const llvm::Type* elttype = makeTypeSpecifier(loc, at->eltType);
            llvmType = llvm::PointerType::get(elttype, 0);       // RICH: Address space.
        }
    } else if (rightType->isFunctionType()) {
        // A function becomes a pointer to the function.
        if (rightValue) {
            VDEBUG("makeCast function type right ", loc, (*rightValue)->print(std::cerr));
            llvmType = (*rightValue)->getType();
            llvmType = llvm::PointerType::get((*rightValue)->getType(), 0);       // RICH: Address space.
         } else {
            llvmType = llvm::PointerType::get(makeTypeSpecifier(loc, rightType), 0);       // RICH: Address space.
         }
    }
    Data right(TI, rightType, rightValue, llvmType);
    Data* source = NULL;	// This will remain NULL if no cast is needed.
    Data* target = &right;

    VDEBUG("makeCast types", loc, std::cerr << "left " << left.type->toString() << " right " << right.type->toString());
    VDEBUG("makeCast left value", loc, if (left.value) (*left.value)->print(std::cerr); else std::cerr << "NULL");
    VDEBUG("makeCast right value", loc, if (right.value) (*right.value)->print(std::cerr); else std::cerr << "NULL");
    if (right.value == NULL) {
        // This is a cast of the left value to the right type.
        target = &right;
        source = &left;
    } else if (left.isFloat) {
	// Cast to the wider of the two operands.
        if (right.isFloat) {
	    // Both are float, make sure they are the same size.
	    if (right.size > left.size) {
	        source = &left;
		target = &right;
	    } else if (left.size > right.size) {
	        source = &right;
		target = &left;
	    }
	} else {
	    // Need to convert the right side to floating point.
	    source = &right;
	    target = &left;
	}
    } else if (right.isFloat) {
	// Need to convert the left side to floating point.
	source = &left;
	target = &right;
    } else if (left.isPointer) {
        VDEBUG("makeCast ptr types", loc, std::cerr << "left " << left.type->toString() << " right " << right.type->toString());
	if (right.isPointer) {
	    // Check type, may need a bit cast.
	    if (right.type != left.type) {
                if (left.llvmType) {
	            source = &left;
		    target = &right;
                } else {
	            source = &right;
		    target = &left;
                }
	    }
	} else {
	    // The right side is an integer.
	    source = &right;
	    target = &left;
	}
    } else if (right.isPointer) {
	// The left side is an integer.
	source = &left;
	target = &right;
    } else if (right.isVoid) {
        // Do nothing.
    } else {
        // Both sides are integers.
        VDEBUG("makeCast int size", loc, std::cerr << "left " << left.size << " right " << right.size);
        VDEBUG("makeCast int isUnsigned", loc, std::cerr << "left " << left.isUnsigned << " right " << right.isUnsigned);
	if (left.size > right.size) {
	    // Promote the right side.
	    source = &right;
	    target = &left;
	} else if (right.size > left.size) {
	    // Promote the left side.
	    source = &left;
	    target = &right;
	} else {
	    // The sizes are the same.
            if (left.isUnsigned && !right.isUnsigned) {
	        // Promote the right side.
	        source = &right;
	        target = &left;
            } else if (!left.isUnsigned && right.isUnsigned) {
	        // Promote the left side.
	        source = &left;
	        target = &right;
	    }
	}
    }

    if (right.value && (*right.value)->getType() == (*left.value)->getType()) {
        // No conversion is needed.
        source = NULL;
    }

    // Classify the result.
    OperatorClass c;
    if (target->isFloat) {
        c = OC_FLOAT;
    } else if (target->isInteger) {
        if (target->isUnsigned) {
	    c = OC_UINT;
        } else {
	    c = OC_SINT;
	}
    } else if (target->isPointer) {
        c = OC_POINTER;
    } else if (target->isVoid) {
        c = OC_VOID;
    } else {
        c = OC_OTHER;
        std::cerr << toString(loc) << ": ";
	xunimp("cast");
    }

    if (source) {
        // We may need to generate a cast instruction.
	// We may be here with an enumeration constant. Handle no type.
	const llvm::Type* type;
	if (target->type == NULL) {
	    type = llvm::IntegerType::get(target->size);
	} else {
            if (target->llvmType) {
                type = target->llvmType;
                VDEBUG("makeCast has type", loc, type->print(std::cerr));
	    } else {
                VDEBUG("makeCast making type", loc, std::cerr << target->type->toString());
	        type = makeTypeSpecifier(loc, target->type);
                VDEBUG("makeCast made type", loc, type->print(std::cerr));
	    }
	}

        VDEBUG("makeCast from type", loc, (*source->value)->getType()->print(std::cerr));
        VDEBUG("makeCast to type", loc, type->print(std::cerr));

	switch (c)
	{
	case OC_UINT:
	case OC_SINT:
            if (source->isInteger) {
		// Convert integer to integer.
		if (source->size == target->size) {
		    // Do nothing.
		} else if (source->size > target->size) {
		    // Truncate the source value.
	            checkCurrentBlock();
	            *source->value = builder.CreateTrunc(*source->value, type);
		} else if (source->size == 1 || source->isUnsigned) {
		    // Zero extend the source value.
	            checkCurrentBlock();
	            *source->value = builder.CreateZExt(*source->value, type);
		} else {
		    // Sign extend the source value.
	            checkCurrentBlock();
                    VDEBUG("SExt1 source", loc, std::cerr << "size " << source->size << " "; (*source->value)->print(std::cerr));
                    VDEBUG("SExt1 destination", loc, std::cerr << "size " << target->size << " "; type->print(std::cerr));
	            *source->value = builder.CreateSExt(*source->value, type);
		}
	    } else if (source->isPointer) {
	        // Convert pointer to integer.
	        checkCurrentBlock();
	        *source->value = builder.CreatePtrToInt(*source->value, type);
	    } else if (source->isFloat) {
	        // Convert Float to integer.
	        checkCurrentBlock();
		if (c == OC_UINT) {
		    // Float to unsigned int.
	            *source->value = builder.CreateFPToUI(*source->value, type);
		} else {
		    // Float to signed int.
	            *source->value = builder.CreateFPToSI(*source->value, type);
		}
	    }
            break;

	case OC_POINTER:
            if (source->isInteger) {
		// Convert integer to pointer.
	        checkCurrentBlock();
	        *source->value = builder.CreateIntToPtr(*source->value, type);
	    } else if (source->isPointer) {
	        // Convert pointer to pointer.
	        checkCurrentBlock();
	        *source->value = builder.CreateBitCast(*source->value, type);
	    } else if (source->isFloat) {
	        // Convert Float to pointer.
	        checkCurrentBlock();
		// Float to unsigned int.
                const llvm::Type* itype = llvm::IntegerType::get(TD.getPointerSize());
	        *source->value = builder.CreateFPToUI(*source->value, itype);
	        // Unsigned int to pointer.
	        *source->value = builder.CreateIntToPtr(*source->value, type);
	    }
            break;

        case OC_FLOAT:
            if (source->isInteger) {
		// Convert integer to float.
	        checkCurrentBlock();
		if (source->isUnsigned) {
		    // Unsigned int to float.
	            *source->value = builder.CreateUIToFP(*source->value, type);
		} else {
		    // Signed int to float.
	            *source->value = builder.CreateSIToFP(*source->value, type);
		}
	    } else if (source->isPointer) {
	        // Convert pointer to float.
	        checkCurrentBlock();
		// Pointer to unsigned int.
                const llvm::Type* itype = llvm::IntegerType::get(TD.getPointerSize());
	        *source->value = builder.CreatePtrToInt(*source->value, itype);
	        // Unsigned int to float.
	        *source->value = builder.CreateUIToFP(*source->value, type);
	    } else if (source->isFloat) {
	        // Convert Float to Float.
		if (source->size == target->size) {
		    // Do nothing.
		} else if (source->size > target->size) {
		    // Truncate the source value.
	            checkCurrentBlock();
	            *source->value = builder.CreateFPTrunc(*source->value, type);
		} else {
		    // Extend the source value.
	            checkCurrentBlock();
	            *source->value = builder.CreateFPExt(*source->value, type);
		}
	    }
            break;

	case OC_VOID:
	case OC_OTHER:
	    break;
	}
    }

    return c;
}

/** Create a value from an initializer.
 */
llvm::Value* CC2LLVMEnv::initializer(const Initializer* init, Type* type,
                                     int& deref, bool top)
{
    deref = 0;
    if (init == NULL) {
        // No initializer.
	return NULL;
    }

    llvm::Value* value = NULL;

    // Handle an initializer.
    ASTSWITCHC(Initializer, init) {
    ASTCASEC(IN_expr, e) {
	if (type->isArrayType()) {
            const E_stringLit* s = e->e->ifE_stringLit();
            if (s) {
                // Initialize an array with a string.
                ArrayType *at = type->asArrayType();
                const llvm::Type* elttype = makeTypeSpecifier(init->loc, at->eltType);
                int size = at->getSize();
                if (size == ArrayType::NO_SIZE) {
                    size = s->data->getDataLen();
                } else if (size == ArrayType::DYN_SIZE) {
                    std::cerr << toString(init->loc) << ": ";
                    xunimp("dynamic array type in initializer");
                }

	        std::vector<llvm::Constant*> elements;
                for (int i = 0; i < size; ++i) {
                    elements.push_back(llvm::ConstantInt::get(elttype, i < s->data->getDataLen() ? s->data->getData()[i] : 0));
                }
	        value = llvm::ConstantArray::get((llvm::ArrayType*)makeTypeSpecifier(init->loc, type),
                                                 elements);
                break;
            } else {
                xunimp("initializing an array with an expression");
            }
        } else {
            value = e->e->cc2llvm(*this, deref);
            const llvm::Type* totype = makeTypeSpecifier(init->loc, type);
            if (!top || (totype->getTypeID() == llvm::Type::PointerTyID && value->getType() != totype)) {
                value = access(value, false, deref);                 // RICH: Volatile.
                makeCast(e->e->loc, e->e->type, value, type);
            }
        }
    }

    ASTNEXTC(IN_compound, c) {
	if (type->isArrayType()) {
            ArrayType *at = type->asArrayType();
	    int size = at->getSize();
	    if (size == ArrayType::NO_SIZE) {
	        size = 0;
	    } else if (size == ArrayType::DYN_SIZE) {
                std::cerr << toString(init->loc) << ": ";
                xunimp("dynamic array type in initializer");
	    }

	    xassert(size >= c->inits.count());
	    std::vector<llvm::Constant*> elements;
            int count = 0;
            const llvm::Type* etype = makeTypeSpecifier(init->loc, at->eltType);
            FOREACH_ASTLIST(Initializer, c->inits, iter) {
                llvm::Value* value = initializer(iter.data(), at->eltType, deref);
                VDEBUG("Init element", init->loc, value->print(std::cerr));
                elements.push_back((llvm::Constant*)value);
                ++count;
            }
            while (count < size) {
                elements.push_back(llvm::Constant::getNullValue(etype));
                ++count;
            }
            
            const llvm::Type* artype = makeTypeSpecifier(init->loc, type);
            VDEBUG("Init type", init->loc, artype->print(std::cerr));
	    value = llvm::ConstantArray::get((llvm::ArrayType*)artype, elements);
	    break;
	} else if (type->isCompoundType()) {
            CompoundType *ct = type->asCompoundType();
            const llvm::Type* sttype = makeTypeSpecifier(init->loc, type);
	    std::vector<llvm::Constant*> members;
            if (ct->keyword == CompoundType::K_UNION) {
                // Initialize the first member.
                unsigned elements = 0;
                ASTListIter<Initializer> iiter(c->inits);
                SFOREACH_OBJLIST(Variable, ct->dataMembers, iter) {
                    Variable const *v = iter.data();
                    value = initializer(iiter.data(), v->type, deref);
                    members.push_back(llvm::cast<llvm::Constant>(value));
                    ++elements;
                    // This loop should run exactly once for a union.
                    break;
                }
                const llvm::StructType* stype = llvm::cast<llvm::StructType>(sttype);
                while (elements < stype->getNumElements()) {
                    // Null values for the first member (if missing)
                    // and the pad (if present).
                    const llvm::Type* type = stype->getTypeAtIndex(elements);
                    members.push_back(llvm::Constant::getNullValue(type));
                    ++elements;
                }
            } else {
                ASTListIter<Initializer> iiter(c->inits);
                SFOREACH_OBJLIST(Variable, ct->dataMembers, iter) {
                    Variable const *v = iter.data();
                    if (!iiter.isDone()) {
                        // Have an initializer for this.
                        value = initializer(iiter.data(), v->type, deref);
                        members.push_back(llvm::cast<llvm::Constant>(value));
                        iiter.adv();
                    } else {
                        // No initializer present.
                        const llvm::Type* type = makeTypeSpecifier(v->loc, v->type);
                        members.push_back(llvm::Constant::getNullValue(type));
                    }
                }
            }

            value = llvm::ConstantStruct::get((llvm::StructType*)sttype, members);
	}
    }

    ASTNEXTC(IN_ctor, c) {
        // RICH: std::cerr << toString(init->loc) << ": ";
        // RICH: xunimp("ctor initializer");
    }

    ASTNEXTC(IN_designated, d) {
        std::cerr << toString(init->loc) << ": ";
        xunimp("designated initializer");
    }

    ASTENDCASED
    }

    return value;
}

static bool isArray(Type* type)
{
    if (type->isReference()) {
        type = type->getAtType();
    }
    return type->isArrayType();
}
static bool isPtr(Type* type)
{
    if (type->isReference()) {
        type = type->getAtType();
    }
    return type->isPtrOrRef() || type->isArrayType();
}
static bool isInt(Type* type)
{
    if (type->isReference()) {
        type = type->getAtType();
    }
    return type->isIntegerType() || type->isBool() || type->isEnumType();
}

llvm::Value *E_binary::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    int deref1;
    int deref2;
    llvm::Value* left;
    llvm::Value* right;
    if (op == BIN_AND || op == BIN_OR) {
        // Logicals get evaluated depending on flow.
        deref1 = deref2 = 0;
        left = NULL;
        right = NULL;
    } else {
        left = e1->cc2llvm(env, deref1);
        VDEBUG("E_binary left", e1->loc, std::cerr <<  " deref " << deref1 << " "; left->print(std::cerr));
        right = e2->cc2llvm(env, deref2);
        VDEBUG("E_binary right", e1->loc, right->print(std::cerr));
    }
    llvm::Value* result = env.binop(loc, op, e1, left, deref1, e2, right, deref2);
    deref = 0;
    return result;
}

llvm::Value* CC2LLVMEnv::binop(SourceLocation loc, BinaryOp op, Expression* e1, llvm::Value* left, int deref1,
                                                           Expression* e2, llvm::Value* right, int deref2)
{
    llvm::Value* result = NULL;
    OperatorClass c = OC_OTHER;

    switch (op)
    {
    case BIN_EQUAL:	// ==
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = builder.CreateICmpEQ(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = builder.CreateFCmpOEQ(left, right);
            break;
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("==");
	    break;
	}
        break;

    case BIN_NOTEQUAL:	// !=
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = builder.CreateICmpNE(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = builder.CreateFCmpONE(left, right);
            break;
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("!=");
	    break;
	}
        break;

    case BIN_LESS:	// < 
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = builder.CreateICmpSLT(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = builder.CreateICmpULT(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = builder.CreateFCmpOLT(left, right);
            break;
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("<");
	    break;
	}
        break;

    case BIN_LESSEQ:	// <= 
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = builder.CreateICmpSLE(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = builder.CreateICmpULE(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = builder.CreateFCmpOLE(left, right);
            break;
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("<=");
	    break;
	}
        break;

    case BIN_GREATER:   // >
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = builder.CreateICmpSGT(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = builder.CreateICmpUGT(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = builder.CreateFCmpOGT(left, right);
            break;
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp(">");
	    break;
	}
        break;

    case BIN_GREATEREQ: // >=
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = builder.CreateICmpSGE(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = builder.CreateICmpUGE(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = builder.CreateFCmpOGE(left, right);
            break;
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp(">=");
	    break;
	}
        break;

    case BIN_MULT:      // *
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
            result = builder.CreateMul(left, right);
            break;
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("*");
	    break;
	}
        break;

    case BIN_DIV:       // /
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = builder.CreateSDiv(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = builder.CreateUDiv(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
            result = builder.CreateFDiv(left, right);
            break;
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("/");
	    break;
	}
        break;

    case BIN_MOD:       // %
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = builder.CreateSRem(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = builder.CreateURem(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
            result = builder.CreateFRem(left, right);
            break;
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("%");
	    break;
	}
        break;

    case BIN_PLUS:      // +
    case BIN_MINUS: {    // -
        VDEBUG("Plus left", loc, std::cerr << e1->type->toString());
        VDEBUG("Plus right", loc, std::cerr << e2->type->toString());
        Expression* te1 = e1;
        Expression* te2 = e2;
	if (op == BIN_PLUS) {
            if ((isPtr(e1->type) && isInt(e2->type)) || (isInt(e1->type) && isPtr(e2->type))) {
                // This could be an array reference *(a + i).
                VDEBUG("+/- checking",  loc, std::cerr << e1->type->toString() << " " << e2->type->toString());
                if (isInt(e1->type) && isPtr(e2->type)) {
                    // Place the integer on the right.
                    te1 = e2;
                    te2 = e1;
                   
                    llvm::Value* temp = left;
                    left = right;
                    right = temp;
                    VDEBUG("swapping i + p", loc, );
                
                    right = access(right, false, deref1);                 // RICH: Volatile.

                    VDEBUG("after swapping left", loc, left->print(std::cerr));
                    VDEBUG("after swapping right", loc, right->print(std::cerr));
                    deref1 = deref2;
                    deref2 = 0;
                } else {
                    right = access(right, false, deref2);                 // RICH: Volatile.
                }
            }
        }

        if (isPtr(te1->type) && isInt(te2->type)) {
            // If the left size is a pointer and the right side is an integer, calculate the result.

            // Get the value of the left side.
            const llvm::Value* before = left;
            VDEBUG("before left", loc, left->print(std::cerr));
            if (accessValue(left) && !isArray(te1->type)) {
                left = access(left, false, deref1);                 // RICH: Volatile.
            }

            if (op == BIN_MINUS) {
                // Negate the integer value.
                right = access(right, false, deref2);                 // RICH: Volatile.
                VDEBUG("BIN_MINUS right", loc, right->getType()->print(std::cerr); std::cerr << " "; right->print(std::cerr));
                llvm::Value* zero = llvm::Constant::getNullValue(right->getType());
                right = builder.CreateSub(zero, right);
            }

            // Make sure the pointer and index sizes match.
            uint64_t lsize = TD.getTypeSizeInBits(left->getType());
            uint64_t rsize = TD.getTypeSizeInBits(right->getType());
            if (lsize != rsize) {
                if (lsize > rsize) {
                    // The pointer is bigger, check for signed vs. unsigned.
                    if (   right->getType()->getPrimitiveSizeInBits() == 1
                        || te2->type->isBool()
                        || te2->type->isEnumType()
                        || (   te2->type->isReference()
                            && (   te2->type->asReferenceTypeC()->getAtType()->isEnumType()
                                || ::isExplicitlyUnsigned(te2->type->asReferenceTypeC()
                                        ->getAtType()->asSimpleTypeC()->type)))) {
                        right = builder.CreateZExt(right, TD.getIntPtrType());
                    } else {
                        VDEBUG("SExt2 source", loc, right->print(std::cerr));
                        VDEBUG("SExt2 destination ", loc, TD.getIntPtrType()->print(std::cerr));
                        right = builder.CreateSExt(right, TD.getIntPtrType());
                    }
                } else {
                    // The integer is bigger.
                    right = builder.CreateTrunc(right, TD.getIntPtrType());
                }
            }

            std::vector<llvm::Value*> index;
            if (before == left) {
                if (   left->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID
                    || left->getType()->getContainedType(0)->getTypeID() == llvm::Type::StructTyID) {
                    VDEBUG("NullValue2", loc, right->getType()->print(std::cerr));
                    index.push_back(llvm::Constant::getNullValue(right->getType()));
                }
            }
            index.push_back(right);
            VDEBUG("GEP2 left", loc, left->print(std::cerr));
            VDEBUG("GEP2 right", loc, right->print(std::cerr));
            result = builder.CreateGEP(left, index.begin(), index.end());
            VDEBUG("GEP2 result", loc, result->print(std::cerr));
            return result;
        }

        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
        VDEBUG("makeCast left", loc, std::cerr << te1->type->toString() << " "; left->print(std::cerr));
        VDEBUG("makeCast right", loc, std::cerr << te2->type->toString() << " "; right->print(std::cerr));
	c = makeCast(loc, te1->type, left, te2->type, &right);
        VDEBUG("PlusOrMinus left", loc, left->print(std::cerr));
        VDEBUG("PlusOrMinus right", loc, right->print(std::cerr));
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_FLOAT:
	    if (op == BIN_PLUS) {
                result = builder.CreateAdd(left, right);
	    } else {
                result = builder.CreateSub(left, right);
	    }
            break;
	case CC2LLVMEnv::OC_POINTER: {
            xassert(op == BIN_MINUS);
            xassert(right->getType() == left->getType());
            // To subtract two pointers we cast them to integers, and divide by
            // the size of the (matching) dereference types.
            const llvm::Type* ptype = llvm::PointerType::get(left->getType()->getContainedType(0), 0);       // RICH: Address space.
            llvm::Value* size = builder.CreateGEP(
                llvm::Constant::getNullValue(ptype),
                llvm::ConstantInt::get(TD.getIntPtrType(), 1));
            size = builder.CreatePtrToInt(size, TD.getIntPtrType());
	    left = builder.CreatePtrToInt(left, TD.getIntPtrType());
	    right = builder.CreatePtrToInt(right, TD.getIntPtrType());
            result = builder.CreateSub(left, right);
            result = builder.CreateUDiv(result, size);
	    break;
        }
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("+/-");
	    break;
	}
        break;
    }

    case BIN_LSHIFT:    // <<
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
            result = builder.CreateShl(left, right);
            break;
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("<<");
	    break;
	}
        break;

    case BIN_RSHIFT:    // >>
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = builder.CreateAShr(left, right);
            break;
	case CC2LLVMEnv::OC_UINT:
            result = builder.CreateLShr(left, right);
            break;
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp(">>");
	    break;
	}
        break;

    case BIN_BITAND:    // &
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
            result = builder.CreateAnd(left, right);
            break;
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("&");
	    break;
	}
        break;

    case BIN_BITXOR:    // ^
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
            result = builder.CreateXor(left, right);
            break;
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("^");
	    break;
	}
        break;

    case BIN_BITOR:     // |
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	c = makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
            result = builder.CreateOr(left, right);
            break;
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            std::cerr << toString(loc) << ": ";
	    xunimp("|");
	    break;
	}
        break;

    case BIN_AND: {     // &&
        llvm::Value* value = checkCondition(e1);
        llvm::BasicBlock* doRight = llvm::BasicBlock::Create("doRight", function, returnBlock);
        llvm::BasicBlock* ifFalse = llvm::BasicBlock::Create("condFalse", function, returnBlock);
        llvm::BasicBlock* ifTrue = llvm::BasicBlock::Create("condTrue", function, returnBlock);
        llvm::BasicBlock* next = llvm::BasicBlock::Create("next", function, returnBlock);
        checkCurrentBlock();
        llvm::BranchInst::Create(doRight, ifFalse, value, currentBlock);
        currentBlock = NULL;

        setCurrentBlock(doRight);
        value = checkCondition(e2);
        checkCurrentBlock();
	builder.CreateCondBr(value, ifTrue, ifFalse);
        currentBlock = NULL;

        setCurrentBlock(ifTrue);
        llvm::Value* tValue = llvm::ConstantInt::getTrue(context);
	builder.CreateBr(next);
        currentBlock = NULL;

        setCurrentBlock(ifFalse);
        llvm::Value* fValue = llvm::ConstantInt::getFalse(context);

        setCurrentBlock(next);
        llvm::PHINode* phi = builder.CreatePHI(tValue->getType());
	phi->addIncoming(tValue, ifTrue);
	phi->addIncoming(fValue, ifFalse);
        result = phi;
        break;
    }

    case BIN_OR: {      // ||
        llvm::Value* value = checkCondition(e1);
        llvm::BasicBlock* doRight = llvm::BasicBlock::Create("doRight", function, returnBlock);
        llvm::BasicBlock* ifFalse = llvm::BasicBlock::Create("condFalse", function, returnBlock);
        llvm::BasicBlock* ifTrue = llvm::BasicBlock::Create("condTrue", function, returnBlock);
        llvm::BasicBlock* next = llvm::BasicBlock::Create("next", function, returnBlock);
        checkCurrentBlock();
        llvm::BranchInst::Create(ifTrue, doRight, value, currentBlock);
        currentBlock = NULL;

        setCurrentBlock(doRight);
        value = checkCondition(e2);
        checkCurrentBlock();
	builder.CreateCondBr(value, ifTrue, ifFalse);
        currentBlock = NULL;

        setCurrentBlock(ifTrue);
        llvm::Value* tValue = llvm::ConstantInt::getTrue(context);
	builder.CreateBr(next);
        currentBlock = NULL;

        setCurrentBlock(ifFalse);
        llvm::Value* fValue = llvm::ConstantInt::getFalse(context);

        setCurrentBlock(next);
        llvm::PHINode* phi = builder.CreatePHI(tValue->getType());
	phi->addIncoming(tValue, ifTrue);
	phi->addIncoming(fValue, ifFalse);
        result = phi;
        break;
    }

    case BIN_COMMA:     // ,
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
	result = right;
        break;

    // gcc extensions
    case BIN_MINIMUM:   // <?
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
        std::cerr << toString(loc) << ": ";
        xunimp("<?");
        break;
    case BIN_MAXIMUM:   // >?
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
        std::cerr << toString(loc) << ": ";
        xunimp(">?");
        break;

    // this exists only between parsing and typechecking
    case BIN_BRACKETS:  // []
        break;		// Never get here.

    case BIN_ASSIGN:    // = (used to denote simple assignments in AST, as opposed to (say) "+=")
        break;		// Never get here, see E_assign.

    // C++ operators
    case BIN_DOT_STAR:    // .*
        std::cerr << toString(loc) << ": ";
        xunimp(".*");
        break;
    case BIN_ARROW_STAR:  // ->*
        std::cerr << toString(loc) << ": ";
        xunimp("->*");
        break;

    // theorem prover extension
    case BIN_IMPLIES:     // ==>
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
        std::cerr << toString(loc) << ": ";
        xunimp("==>");
        break;
    case BIN_EQUIVALENT:  // <==>
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
        std::cerr << toString(loc) << ": ";
        xunimp("<==>");
        break;
    case NUM_BINARYOPS:
        break;
    }

    return result;
}

llvm::Value *E_addrOf::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value * result = expr->cc2llvm(env, deref);
    if (deref > 0) {
        --deref;
    }
    return result;
}

llvm::Value *E_deref::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* source = ptr->cc2llvm(env, deref);
    VDEBUG("E_deref", loc, std::cerr << "deref " << deref << " "; source->print(std::cerr));
    VDEBUG("E_deref", loc, std::cerr << "deref " << deref << " "; source->getType()->getContainedType(0)->print(std::cerr));
    /** @TODO I'm not pleased with this mess. The parser should help us more here.
     */
    if (source->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID) {
        std::vector<llvm::Value*> indices;
        indices.push_back(llvm::Constant::getNullValue(env.TD.getIntPtrType()));
        indices.push_back(llvm::Constant::getNullValue(env.TD.getIntPtrType()));
        source = env.builder.CreateGEP(source, indices.begin(), indices.end(), "");
        VDEBUG("E_deref GEP", loc, source->print(std::cerr));
    }
    bool first = source->getType()->getContainedType(0)->isFirstClassType();
    if (   first
        && source->getType()->getContainedType(0)->getTypeID() != llvm::Type::ArrayTyID
        && source->getType()->getContainedType(0)->getTypeID() != llvm::Type::StructTyID) {
        if (   source->getType()->getContainedType(0)->getTypeID() != llvm::Type::PointerTyID
            || source->getType()->getContainedType(0)->getContainedType(0)->getTypeID() != llvm::Type::FunctionTyID) {
            ++deref;
        }
    }
    VDEBUG("E_deref", loc, std::cerr << "deref " << deref << " "; source->print(std::cerr));
    return source;
}

llvm::Value *E_cast::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* result = expr->cc2llvm(env, deref);
    result = env.access(result, false, deref);                 // RICH: Volatile.
    VDEBUG("cast from", loc, result->print(std::cerr); std::cerr << " to " << type->toString());
    env.makeCast(loc, expr->type, result, type);
    deref = 0;
    return result;
}

llvm::Value *E_stdConv::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* result = expr->cc2llvm(env, deref);
    result = env.access(result, false, deref);                 // RICH: Volatile.
    VDEBUG("stdConv from", loc, result->print(std::cerr); std::cerr << " to " << type->toString());
    env.makeCast(loc, expr->type, result, type);
    deref = 0;
    return result;
}

llvm::Value *E_cond::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    /* This one is kind of tricky. Quoting the C standard (ISO/IEC 9899:1999) 6.5.15:
     * Constraints
     * * The first operand shall have scalar type.
     * * One of the following shall hold for the second and third operands:
     *  - both operands have arithmetic type;
     *  - both operands have the same structure or union type;
     *  - both operands have void type;
     *  - both operands are pointers to qualified or unqualified versions of compatible types;
     *  - one operand is a pointer and the other is a null pointer constant; or
     *  - one operand is a pointer to an object or incomplete type and the other is a pointer to
     *    a qualified or unqualified version of void.
     */

    // This starts out looking very much like an if statement.
    llvm::Value* value = env.checkCondition(cond);
    llvm::BasicBlock* ifTrue = llvm::BasicBlock::Create("condTrue", env.function, env.returnBlock);
    llvm::BasicBlock* ifFalse = llvm::BasicBlock::Create("condFalse", env.function, env.returnBlock);
    llvm::BasicBlock* next = llvm::BasicBlock::Create("next", env.function, env.returnBlock);
    env.checkCurrentBlock();
    llvm::BranchInst::Create(ifTrue, ifFalse, value, env.currentBlock);
    env.currentBlock = NULL;

    env.setCurrentBlock(ifTrue);
    llvm::Value* trueValue = th->cc2llvm(env, deref);
    trueValue = env.access(trueValue, false, deref);                 // RICH: Volatile.
    VDEBUG("E_conv true", loc, trueValue->print(std::cerr); std::cerr << " is " << th->type->toString());
    if (!type->isVoid()) {
        env.makeCast(loc, th->type, trueValue, type);
    }
    ifTrue = env.currentBlock;
    if (env.currentBlock) {
        // Close the current block.
        llvm::BranchInst::Create(next, env.currentBlock);
        env.currentBlock = NULL;
    }

    env.setCurrentBlock(ifFalse);
    llvm::Value* falseValue = el->cc2llvm(env, deref);
    falseValue = env.access(falseValue, false, deref);                 // RICH: Volatile.
    VDEBUG("E_conv false", loc, falseValue->print(std::cerr); std::cerr << " is " << el->type->toString());
    if (!type->isVoid()) {
        env.makeCast(loc, el->type, falseValue, type);
    }
    ifFalse = env.currentBlock;

    env.setCurrentBlock(next);
    llvm::Value* result = NULL;
    if (trueValue->getType()->isFirstClassType() && falseValue->getType()->isFirstClassType()) {
        llvm::PHINode* phi = env.builder.CreatePHI(trueValue->getType());
	phi->addIncoming(trueValue, ifTrue);
	phi->addIncoming(falseValue, ifFalse);
        result = phi;
    } else if (type->isVoid()) {
        // Do nothing.
    } else {
        std::cerr << toString(loc) << ": ";
        xunimp("?:");
    }
    return result;
}

llvm::Value *E_sizeofType::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    const llvm::Type* etype = env.makeTypeSpecifier(loc, atype->getType());
    VDEBUG("GEP5", loc, etype->print(std::cerr));
    const llvm::Type* ptype = llvm::PointerType::get(etype, 0);       // RICH: Address space.
    llvm::Value* value = env.builder.CreateGEP(
        llvm::Constant::getNullValue(ptype),
        llvm::ConstantInt::get(env.TD.getIntPtrType(), 1));
    value = env.builder.CreatePtrToInt(value, env.TD.getIntPtrType());
    deref = 0;
    return value;
}

llvm::Value* CC2LLVMEnv::doassign(SourceLocation loc, llvm::Value* destination, int deref1, Type* dtype,
                                                 llvm::Value* source, int deref2, Type* stype)
{
    checkCurrentBlock();
    if (llvm::ConstantArray::classof(source)) {
        --deref2;
    }

    VDEBUG("doassign", loc, source->print(std::cerr));
    source = access(source, false, deref2);                 // RICH: Volatile.

    VDEBUG("doassign get", loc, destination->print(std::cerr));
    destination = access(destination, false, deref1, 1);                 // RICH: Volatile.
    VDEBUG("doassign cast", loc, std::cerr <<  stype->toString() << " -> " << dtype->toString());
    if (   destination->getType()->getContainedType(0)->getTypeID() != llvm::Type::ArrayTyID
        && destination->getType()->getContainedType(0)->getTypeID() != llvm::Type::StructTyID) {
        makeCast(loc, stype, source, dtype);
    } else if (   destination->getType()->getContainedType(0)->getTypeID() == llvm::Type::StructTyID
               && source->getType()->getTypeID() != llvm::Type::StructTyID) {
        VDEBUG("doassign struct source", loc, std::cerr << " deref2 " << deref2; source->print(std::cerr));
        source = builder.CreateLoad(source, false);     // RICH: Is volatile.
    }
    VDEBUG("doassign source", loc, source->print(std::cerr));
    VDEBUG("doassign destination", loc, destination->print(std::cerr));
    new llvm::StoreInst(source, destination, false, currentBlock);	// RICH: isVolatile
    return source;
}

llvm::Value* E_assign::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    
    int deref1;
    llvm::Value* destination = target->cc2llvm(env, deref1);
    int deref2;
    llvm::Value* source = src->cc2llvm(env, deref2);		// Evaluate the source expression as an rvalue.

    env.checkCurrentBlock();
    if (op == BIN_ASSIGN) {
	// Assign is simple. Get it out of the way.
        source = env.doassign(loc, destination, deref1, target->type, source, deref2, src->type);
        deref = deref1;
        return destination;
    }

    // Handle all other forms of assignment operators.
    destination = env.access(destination, false, deref1, 1);                 // RICH: Volatile.
    llvm::Value* temp = NULL;					// A place to store the temporary.
    temp = env.binop(loc, op, target, destination, deref1, src, source, deref2);

    VDEBUG("E_assign", loc, std::cerr << "result "; temp->getType()->print(std::cerr);
                            std::cerr << " destination "; destination->getType()->getContainedType(0)->print(std::cerr));
    if (temp->getType() != destination->getType()->getContainedType(0)) {
        // Cast the result to the destination type.
        env.makeCast(loc, src->type, temp, target->type);
    }
    VDEBUG("Store2 source", loc, temp->print(std::cerr));
    VDEBUG("Store2 destination", loc, destination->print(std::cerr));
    deref = deref1;
    new llvm::StoreInst(temp, destination, false, env.currentBlock);	// RICH: Volatile
    return destination;
}

llvm::Value *E_new::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("new");
    return NULL;
}

llvm::Value *E_delete::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("delete");
    return NULL;
}

llvm::Value *E_throw::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("throw");
    return NULL;
}

llvm::Value *E_keywordCast::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("keyword cast");
    return NULL;
}

llvm::Value *E_typeidExpr::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("typeid");
    return NULL;
}

llvm::Value *E_typeidType::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("typeid");
    return NULL;
}

llvm::Value *E_grouping::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("grouping");
    return NULL;
}

llvm::Value *E_arrow::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("->");
    return NULL;
}


llvm::Value *E_addrOfLabel::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("label address");
    return NULL;
}

llvm::Value *E_gnuCond::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("gnu conditional");
    return NULL;
}

llvm::Value *E_alignofExpr::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("alignof");
    return NULL;
}

llvm::Value *E_alignofType::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("alignof");
    return NULL;
}

llvm::Value *E___builtin_va_start::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* value = expr->cc2llvm(env, deref);
    value = env.access(value, false, deref, 1);                 // RICH: Volatile.
    deref = 0;
    const llvm::Type* type = llvm::IntegerType::get(env.TI.CharWidth());
    type =  llvm::PointerType::get(type, 0);	// RICH: address space.
    env.checkCurrentBlock();
    value = env.builder.CreateBitCast(value, type);
    llvm::Function* function = llvm::Intrinsic::getDeclaration(env.mod, llvm::Intrinsic::vastart);
    std::vector<llvm::Value*> parameters;
    parameters.push_back(value);
    return env.builder.CreateCall(function, parameters.begin(), parameters.end());
}

llvm::Value *E___builtin_va_copy::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::cerr << toString(loc) << ": ";
    xunimp("__builtin_va_copy");
    return NULL;
}

llvm::Value *E___builtin_va_arg::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* value = expr->cc2llvm(env, deref);
    value = env.access(value, false, deref, 1);                 // RICH: Volatile.
    deref = 0;
    const llvm::Type* type = env.makeTypeSpecifier(loc, atype->getType());
    env.checkCurrentBlock();
    return new llvm::VAArgInst(value, type, "", env.currentBlock);
}

llvm::Value *E___builtin_va_end::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* value = expr->cc2llvm(env, deref);
    value = env.access(value, false, deref, 1);                 // RICH: Volatile.
    deref = 0;
    const llvm::Type* type = llvm::IntegerType::get(env.TI.CharWidth());
    type =  llvm::PointerType::get(type, 0);	// RICH: address space.
    env.checkCurrentBlock();
    value = env.builder.CreateBitCast(value, type);
    llvm::Function* function = llvm::Intrinsic::getDeclaration(env.mod, llvm::Intrinsic::vaend);
    std::vector<llvm::Value*> parameters;
    parameters.push_back(value);
    return env.builder.CreateCall(function, parameters.begin(), parameters.end());
}

llvm::Value *E___builtin_constant_p::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* value = expr->cc2llvm(env, deref);
    bool isConstantExpr = llvm::isa<llvm::ConstantExpr>(value);
    if (isConstantExpr) {
        value = llvm::ConstantInt::get(env.context, llvm::APInt(expr->type->sizeInBits(env.TI), 1));
    } else {
        value = llvm::ConstantInt::get(env.context, llvm::APInt(expr->type->sizeInBits(env.TI), 0));
    }
    env.makeCast(loc, expr->type, value, type);
    deref = 0;
    return value;
}

llvm::Value *E___builtin_alloca::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* value = expr->cc2llvm(env, deref);
    value = env.access(value, false, deref);                 // RICH: Volatile.
    VDEBUG("E_builtin_alloca", loc, std::cerr << "value "; value->getType()->print(std::cerr));
    deref = 0;
    xassert(env.entryBlock);
    const llvm::Type* type = llvm::IntegerType::get(env.TI.CharWidth());
    env.checkCurrentBlock();
    llvm::AllocaInst* lv;
    if (env.entryBlock == env.currentBlock) {
        lv = new llvm::AllocaInst(type, value, "alloca", env.entryBlock);
    } else {
        lv = new llvm::AllocaInst(type, value, "alloca", env.entryBlock->getTerminator());
    }

    return lv;
}

llvm::Value *E_compoundLit::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    return env.initializer(init, stype->getType(), deref, true);
}

llvm::Value *E_statement::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    if (type->isVoid()) {
        // No value from this statement group.
        s->cc2llvm(env);
        return NULL;
    }

    // Generate all but the last statement.
    Statement* last = s->stmts.last();
    FOREACH_ASTLIST(Statement, s->stmts, iter) {
      VDEBUG("E_statement statement", loc, s->debugPrint(std::cerr, 0));
      
      if (iter.data() == last) {
          break;
      }
      iter.data()->cc2llvm(env);
    }

    Expression* expr = last->asS_expr()->expr->expr;
    VDEBUG("E_statement expr", loc, std::cerr << expr->asString());
    return expr->cc2llvm(env, deref);
}



// ------------------ FullExpression -----------------
llvm::Value *FullExpression::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
  return expr->cc2llvm(env, deref);
}

// ------------------ Translate ----------------------
llvm::Module* CC2LLVMEnv::doit()
{
    FOREACH_ASTLIST(TopForm, input.topForms, iter) {
        iter.data()->cc2llvm(*this);
    }

    // Verify the generated module.
    llvm::verifyModule(*mod, llvm::PrintMessageAction);
    // llvm::verifyModule(*mod, llvm::AbortProcessAction);

    return mod;
}

// ------------------- entry point -------------------
llvm::Module* cc_to_llvm(sm::string name, StringTable &str, TranslationUnit const &input,
                         TargetInfo& TI, Diagnostic& diags,
                         llvm::LLVMContext& context, LangOptions& LO)
{
    CC2LLVMEnv env(str, name, input, TI, diags, context, LO);
    return env.doit();
}

// EOF
