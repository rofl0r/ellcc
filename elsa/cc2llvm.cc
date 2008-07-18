// cc2llvm.cc
// code for cc2llvm.h

#include "cc2llvm.h"         // this module

#include "datablok.h"

// LLVM
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Function.h>
#include <llvm/CallingConv.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>
#include <llvm/InlineAsm.h>
#include <llvm/Intrinsics.h>
#include <llvm/ParameterAttributes.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Analysis/Verifier.h>

#define BITS_PER_BYTE	8	// RICH: Temporary.

#if 1
// Really verbose debugging.
#define VDEBUG(who, where, what) cout << toString(where) << ": " << who << " "; what; cout << "\n"
#else
#define VDEBUG(who, where, what)
#endif

// -------------------- CC2LLVMEnv ---------------------
CC2LLVMEnv::CC2LLVMEnv(StringTable &s, string name, const TranslationUnit& input,
                       string targetData, string targetTriple,
		       llvm::IRBuilder& builder)
  : str(s),
    targetData(targetData.c_str()),
    input(input),
    mod(new llvm::Module(name.c_str())),
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
    builder(builder)
{ 
    mod->setDataLayout("e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-s0:0:64-f80:32:32");
    mod->setTargetTriple("i686-pc-linux-gnu");
}

CC2LLVMEnv::~CC2LLVMEnv()
{
}

static llvm::GlobalValue::LinkageTypes getLinkage(DeclFlags flags)
{
    if (flags & DF_INLINE) {
        return llvm::GlobalValue::LinkOnceLinkage;
    } else if (flags & DF_STATIC) {
        return llvm::GlobalValue::InternalLinkage;
    }

    return llvm::GlobalValue::ExternalLinkage;
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
llvm::Value* CC2LLVMEnv::checkCondition(SourceLoc loc, llvm::Value* value, int deref, bool neg)
{
    value = access(value, false, deref);                 // RICH: Volatile.

    const llvm::Type* ctype = value->getType();
    VDEBUG("checkCondition", loc, ctype->print(cout); cout << " "; value->print(cout));
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
            cerr << toString(loc) << ": ";
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
    VDEBUG("checkCondition", cond->loc, cout << cond->asString() << "\n");
    llvm::Value* value = cond->cc2llvm(*this, deref);
    return checkCondition(cond->loc, value, deref);
}

const llvm::Type* CC2LLVMEnv::makeTypeSpecifier(SourceLoc loc, Type *t)
{
    const llvm::Type* type = types.get(t);

    if (type) {
        // This type has already been seen.
	return type;
    }

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
	     * LLVM doesn't understand void*. Make it into iBITS_PER_BYTE*.
	     */
            type = llvm::IntegerType::get(BITS_PER_BYTE);
	}

	xassert(type != NULL && "A NULL type encountered");
        type =  llvm::PointerType::get(type, 0);	// RICH: Address space.
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
        makeParameterTypes(ft, args);
        type = llvm::FunctionType::get(returnType, args, ft->acceptsVarargs() || (ft->flags & FF_NO_PARAM_INFO));
        break;
    }
    case Type::T_ARRAY: {
        ArrayType *at = t->asArrayType();
	int size = at->getSize();
	if (size == ArrayType::NO_SIZE) {
	    size = 0;
	} else if (size == ArrayType::DYN_SIZE) {
            cerr << toString(loc) << ": ";
            xunimp("dynamic array type");
	}
        type = llvm::ArrayType::get(makeTypeSpecifier(loc, at->eltType), size);
        break;
    }
    case Type::T_POINTERTOMEMBER:
        cerr << toString(loc) << ": ";
        xunimp("pointer to member type");
        break;
    case Type::T_DEPENDENTSIZEDARRAY:
        cerr << toString(loc) << ": ";
        xunimp("dependent sized array type");
        break;
    case Type::T_LAST_TYPE_TAG:	// Quell warnings.
        break;
    }

    // Remember the mapping of this type.
    types.add(t, type);
    return type;
}

const llvm::Type* CC2LLVMEnv::makeAtomicTypeSpecifier(SourceLoc loc, AtomicType *at)
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
	    // Define an integer type.
            type = llvm::IntegerType::get(simpleTypeReprSize(id) * BITS_PER_BYTE);
            break;
	case ST_BOOL:
            type = llvm::IntegerType::get(1);
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
            cerr << toString(loc) << ": ";
            xunimp("simple type");
	    break;
        }
	
        break;
    }

    case AtomicType::T_COMPOUND: {
        CompoundType *ct = at->asCompoundType();
	type = compounds.get(ct);
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

	// Get the non-static data members.
	std::vector<const llvm::Type*>fields;
        int i = 0;
        SFOREACH_OBJLIST(Variable, ct->dataMembers, iter) {
            Variable const *v = iter.data();
            VDEBUG("member", v->loc, cout << v->toString());
            members.add(v, llvm::ConstantInt::get(targetData.getIntPtrType(), i++));
	    fields.push_back(makeTypeSpecifier(v->loc, v->type));
        }

	llvm::StructType* st = llvm::StructType::get(fields, false);	// RICH: isPacked
        llvm::cast<llvm::OpaqueType>(fwd.get())->refineAbstractTypeTo(st);
	type = llvm::cast<llvm::Type>(fwd.get());
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
        type = llvm::IntegerType::get(et->reprSize() * BITS_PER_BYTE);
        break;
    }
      
    case AtomicType::T_TYPEVAR:
    case AtomicType::T_PSEUDOINSTANTIATION:
    case AtomicType::T_DEPENDENTQTYPE:
    case AtomicType::T_TEMPLATETYPEVAR:
        cerr << toString(loc) << ": ";
        xunimp("template-related");
        break;

    case AtomicType::NUM_TAGS:
        break;
    }

    return type;
}

StringRef CC2LLVMEnv::getVariableName(Variable const *v)
{
    // for now
    return str(v->name);
}

PQ_name *CC2LLVMEnv::makeName(Variable const *v)
{
    return makePQ_name(getVariableName(v));
}

PQ_name *CC2LLVMEnv::makePQ_name(StringRef name)
{
    return new PQ_name(SL_UNKNOWN, name);		// RICH: Source location.
}

void CC2LLVMEnv::makeParameterTypes(FunctionType *ft, std::vector<const llvm::Type*>& args)
{
    SFOREACH_OBJLIST(Variable, ft->params, iter) {
        Variable const *param = iter.data();

	const llvm::Type* type = makeTypeSpecifier(param->loc, param->type);
	// type will be NULL if a "..." is encountered in the parameter list.
	if (type) {
            VDEBUG("makeParameters", param->loc, type->print(cout));
            args.push_back(type);
        }
    }
}

llvm::Value* CC2LLVMEnv::declaration(const Variable* var, llvm::Value* init, int deref)
{
    llvm::Value* value = NULL;
    // Create the full generated declaration.
    const llvm::Type* type = makeTypeSpecifier(var->loc, var->type);
    VDEBUG("declaration", var->loc, cout << toString(var->flags) << " " << var->toString());
    if (var->type->getTag() == Type::T_FUNCTION) {
        llvm::GlobalValue::LinkageTypes linkage = getLinkage(var->flags);
        llvm::Function* gf = llvm::Function::Create((llvm::FunctionType*)type, linkage, makeName(var)->name, mod);
        gf->setCallingConv(llvm::CallingConv::C); // RICH: Calling convention.
        variables.add(var, gf);
        value = gf;
    } else if (var->type->getTag() == Type::T_DEPENDENTSIZEDARRAY) {
        cerr << toString(var->loc) << ": ";
        xunimp("dependent sized array");
    } else if (var->type->getTag() == Type::T_LAST_TYPE_TAG) {
        cerr << toString(var->loc) << ": ";
        xunimp("last type tag");
    } else if (var->flags & (DF_STATIC|DF_GLOBAL)) {
        // A global variable.
        if (!(var->flags & DF_EXTERN) && init == NULL) {
            init = llvm::Constant::getNullValue(type);
        }
        llvm::GlobalVariable* gv = (llvm::GlobalVariable*)variables.get(var);   // RICH: cast
        if (gv == NULL) {
            gv = new llvm::GlobalVariable(type, false,	// RICH: isConstant
                    getLinkage(var->flags), (llvm::Constant*)init, makeName(var)->name, mod);	// RICH: cast
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
            lv = new llvm::AllocaInst(type, makeName(var)->name, entryBlock);
        } else {
            lv = new llvm::AllocaInst(type, makeName(var)->name, entryBlock->getTerminator());
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
    VDEBUG("constructor", cons->loc, cout << cons->asString() << " " << cons->ctorVar->toString());
    VDEBUG("constructor retObj", cons->loc, cout << cons->retObj->asString());
    std::vector<llvm::Value*> parameters;
    // RICH: int deref = 0;
    // RICH: object = access(object, false, deref);                 // RICH: Volatile.
    parameters.push_back(object);
    FAKELIST_FOREACH(ArgExpression, cons->args, arg) {
        int deref;
        llvm::Value* param = arg->expr->cc2llvm(*this, deref);
        VDEBUG("Param", arg->expr->loc, param->print(cout));
        param = access(param, false, deref);                 // RICH: Volatile.
        VDEBUG("Param after", arg->expr->loc, param->print(cout));
        if (   param->getType()->getTypeID() == llvm::Type::ArrayTyID
                || (   param->getType()->getTypeID() == llvm::Type::PointerTyID
                    && param->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID)) {
            // This is somewhat of a hack: It should be done in cc_tcheck.cc.
            // Add an implicit cast of &array to *array.
            const llvm::Type* type = llvm::PointerType::get(param->getType()->getContainedType(0)->getContainedType(0), 0); // RICH: Address space.
            param = builder.CreateBitCast(param, type);
        }
        parameters.push_back(param);
        VDEBUG("Param", arg->expr->loc, param->print(cout));
    }

    llvm::Value* function = variables.get(cons->ctorVar);
    xassert(function && "An undeclared constructor has been referenced");
    // RICH: deref = 0;
    // RICH: function = access(function, false, deref);                 // RICH: Volatile.
    VDEBUG("CreateCall", cons->ctorVar->loc, function->print(cout));
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

    ASTDEFAULTC {
        cerr << toString(loc) << ": ";
        xunimp("TopForm");
    }
    
    ASTENDCASE
    }
}

void Function::cc2llvm(CC2LLVMEnv &env) const
{
    // Did we see this declaration?
    env.function = (llvm::Function*)env.variables.get(nameAndParams->var);      // RICH: cast, check if func.
    const llvm::Type* returnType;
    if (env.function == NULL) {
        // No, define it.
        llvm::GlobalValue::LinkageTypes linkage = getLinkage(nameAndParams->var->flags);
        returnType = env.makeTypeSpecifier(nameAndParams->var->loc, funcType->retType);
        std::vector<const llvm::Type*>args;
        env.makeParameterTypes(funcType, args);
        llvm::FunctionType* ft = llvm::FunctionType::get(returnType, args, funcType->acceptsVarargs());
        env.function = llvm::Function::Create(ft, linkage, nameAndParams->var->name, env.mod);
        env.function->setCallingConv(llvm::CallingConv::C); // RICH: Calling convention.
        env.variables.add(nameAndParams->var, env.function);
    } else {
        returnType = env.function->getReturnType();
    }

    VDEBUG("Function", nameAndParams->var->loc, cout << nameAndParams->var->toString() << " "; returnType->print(cout));
    const Function* oldFunctionAST = env.functionAST;	// Handle nested functions.
    env.functionAST = this;
    env.entryBlock = llvm::BasicBlock::Create("entry", env.function, NULL);

    // Set the initial current block.
    env.setCurrentBlock(env.entryBlock);

    // Add the parameter names.
    llvm::Function::arg_iterator llargs = env.function->arg_begin();
    bool first = true;
    SFOREACH_OBJLIST(Variable, funcType->params, iter) {
        const Variable *param = iter.data();
        llvm::Value* arg = llargs++;
        // Make space for the argument.
        const llvm::Type* type = arg->getType();
        if (first && receiver && param != receiver) {
            // Yuck! The receiver is not explicit. I think it should be.
            llvm::AllocaInst* addr = env.builder.CreateAlloca(type, NULL, receiver->name);
            // Remember where the argument can be retrieved.
            env.variables.add(receiver, addr);
            // Store the argument for later use.
            VDEBUG("Store3 source", receiver->loc, arg->print(cout));
            VDEBUG("Store3 destination", receiver->loc, addr->print(cout));
            env.builder.CreateStore(arg, addr, false);	// RICH: IsVolatile.

            // Do the next argument.
            arg = llargs++;
            type = arg->getType();
        }

        first = false;
	// type will be NULL for "...".
	if (type) {
	    llvm::AllocaInst* addr = env.builder.CreateAlloca(type, NULL, param->name);
	    // Remember where the argument can be retrieved.
            env.variables.add(param, addr);
	    // Store the argument for later use.
            VDEBUG("Store3 source", param->loc, arg->print(cout));
            VDEBUG("Store3 destination", param->loc, addr->print(cout));
	    env.builder.CreateStore(arg, addr, false);	// RICH: IsVolatile.
        }
    }

    if (inits->isNotEmpty()) {
        VDEBUG("member init for", nameAndParams->var->loc, cout << nameAndParams->var->toString());
        FAKELIST_FOREACH(MemberInit, inits, init) {
            if (init->member) {
                VDEBUG("member init", init->member->loc, cout << init->member->toString());
            }
            if(init->base) {
                VDEBUG("base init", nameAndParams->var->loc, cout << init->base->toString());
            }

            Expression *expr = NULL;
            FAKELIST_FOREACH(ArgExpression, init->args, arg) {
                VDEBUG("member init arg", arg->expr->loc, cout << arg->expr->asString());
                expr = arg->expr;
            }

            xassert(receiver && "no receiver");

            // Compute the object address.
            // "this"
            E_this *ths = new E_this(nameAndParams->var->loc);
            ths->receiver = receiver;
            // "*this"
            E_deref *deref = new E_deref(nameAndParams->var->loc, ths);
            deref->type = receiver->type;
            // "(*this).member
            E_fieldAcc *efieldacc = new E_fieldAcc(nameAndParams->var->loc, deref, new PQ_variable(nameAndParams->var->loc, init->member));
            efieldacc->type = init->member->type;
            efieldacc->field = init->member;

            if (init->ctorStatement) {
                int der;
                llvm::Value* object = efieldacc->cc2llvm(env, der);
                env.constructor(object, init->ctorStatement);
            } else if (init->ctorVar) {
                cerr << toString(nameAndParams->var->loc) << ": ";
                xunimp("member ctorVar");
            } else {
                xassert(expr && "no constructor argument");
                // Copy the member via an assignment.
                // RICH: This isn't right. And I don't like it.
                // RICH: Refactor to do the assign directly without building new nodes.

                // "(*this).y = other.y"
                // use the E_assign built-in operator
                Expression* action = new E_assign(expr->loc, efieldacc, BIN_ASSIGN, expr);
                action->type = expr->type;
                expr = action;
                int der;
                expr->cc2llvm(env, der);
            }
        }
    }

    if (handlers->isNotEmpty()) {
        cerr << toString(nameAndParams->var->loc) << ": ";
        xunimp("exception handlers");
    }

    // Set up the return block.
    env.returnBlock = llvm::BasicBlock::Create("return", env.function, 0);
    // Set up the return value.
    if (returnType == llvm::Type::VoidTy) {
        // A void function.
        env.returnValue = NULL;
        llvm::ReturnInst::Create(env.returnBlock);
    } else {
        // Create the return value holder.
	
        env.returnValue = env.builder.CreateAlloca(returnType, NULL, "retval");

        // RICH: This should happen in main() only: Default return value.
        llvm::Constant* nullInt = llvm::Constant::getNullValue(returnType);
        VDEBUG("Store4 source", nameAndParams->var->loc, nullInt->print(cout));
        VDEBUG("Store4 destination", nameAndParams->var->loc, env.returnValue->print(cout));
        env.builder.CreateStore(nullInt, env.returnValue, false);	// RICH: main() only.

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
        if (declarator->ctorStatement) {
            // Handle a constructor.
            env.constructor(object, declarator->ctorStatement);
        }

        // Elaborated statements are relevant; for now, fail if
        // they are present.
        if (declarator->ctorStatement || declarator->dtorStatement) {
            // RICH: declarator->ctorStatement->cc2llvm(env);
            // RICH: declarator->dtorStatement->cc2llvm(env);
            // RICH: cerr << toString(var->loc) << ": ";
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
    VDEBUG("Expr", loc, cout << expr->expr->asString());
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
    env.builder.CreateBr(env.continueBlock);
    env.currentBlock = NULL;
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
    VDEBUG("S_for cond", loc, cout << condition->expr->expr->asString());
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
        VDEBUG("Return", loc, cout << expr->expr->asString());
        xassert(env.returnValue && "return a value in a function returning void");
        int deref;
        llvm::Value* value = expr->cc2llvm(env, deref);
        VDEBUG("Return type", loc, cout << expr->expr->type->toString() << " deref " << deref);
        if (   value->getType()->getTypeID() == llvm::Type::PointerTyID
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
        VDEBUG("S_return source", loc, value->print(cout));
        VDEBUG("S_return destination", loc, env.returnValue->print(cout));
        env.builder.CreateStore(value, env.returnValue, false);	// RICH: isVolatile
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
    cerr << toString(loc) << ": ";
    xunimp("try");
}

void S_asm::cc2llvm(CC2LLVMEnv &env) const
{
    cerr << toString(loc) << ": ";
    xunimp("asm");
#if RICH
    env.addStatement(
      new S_asm(SL_GENERATED, text->cc2llvm(env)->asE_stringLit()));
#endif
}

void S_namespaceDecl::cc2llvm(CC2LLVMEnv &env) const 
{
    // should be able to just drop these
}


void S_computedGoto::cc2llvm(CC2LLVMEnv &env) const
{
    cerr << toString(loc) << ": ";
    xunimp("computed goto");
}

void S_rangeCase::cc2llvm(CC2LLVMEnv &env) const
{
    cerr << toString(loc) << ": ";
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
    return llvm::ConstantInt::get(llvm::APInt(1, (int)b));
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

    VDEBUG("IntLit", loc, cout << text << " radix " << radix);
    return llvm::ConstantInt::get(llvm::APInt(type->reprSize() * BITS_PER_BYTE, p, endp - p, radix));
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
        cerr << toString(loc) << ": ";
        xunimp("floating point type");
        break;
    }
    return llvm::ConstantFP::get(llvm::APFloat(*semantics, text));
}

llvm::Value *E_stringLit::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    std::string value((const char*)data->getDataC(), data->getDataLen());	// RICH: cast
    llvm::Constant* c = llvm::ConstantArray::get(value, false);	// Don't add a nul character.
    // RICH: Sizeof char.
    llvm::ArrayType* at = llvm::ArrayType::get(llvm::IntegerType::get(BITS_PER_BYTE), data->getDataLen());
    // RICH: Non-constant strings?
    llvm::GlobalVariable* gv = new llvm::GlobalVariable(at, true, llvm::GlobalValue::InternalLinkage, c, ".str", env.mod);

    // Get the address of the string as an open array.
    env.checkCurrentBlock();
    std::vector<llvm::Value*> indices;
    indices.push_back(llvm::Constant::getNullValue(env.targetData.getIntPtrType()));
    indices.push_back(llvm::Constant::getNullValue(env.targetData.getIntPtrType()));
    VDEBUG("GEP3", loc, );
    llvm::Value* result = env.builder.CreateGEP(gv, indices.begin(), indices.end(), "");
    VDEBUG("GEP3", loc, result->print(cout));
    return result;
}

llvm::Value *E_charLit::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    return llvm::ConstantInt::get(llvm::APInt(type->reprSize() * BITS_PER_BYTE, c));
}

llvm::Value *E_this::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 1;
    llvm::Value* value = env.variables.get(receiver);
    xassert(value && "'this' was not defined");
    VDEBUG("E_this", loc, value->getType()->print(cout));
    return value;
}

/** Get a variable used in an expression.
 */
llvm::Value *E_variable::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    if (var->isEnumerator()) {
        // This is an enumerator constant. Return it's value.
        deref = 0;
        return llvm::ConstantInt::get(llvm::APInt(type->reprSize() * BITS_PER_BYTE, var->getEnumeratorValue()));
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

    VDEBUG("E_variable ID", loc, cout << value->getType()->getContainedType(0)->getTypeID());

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
    VDEBUG("E_variable deref", loc, cout << deref);
    return value;
}

llvm::Value *E_funCall::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    env.checkCurrentBlock();
    std::vector<llvm::Value*> parameters;
    // Check for a method call.
    llvm::Value* function = NULL;
    if (func->kind() == E_FIELDACC) {
        VDEBUG("E_funCall method", loc, cout << func->asString());
        E_fieldAcc* fa = func->asE_fieldAcc();
        function = env.variables.get(fa->field);
        if (function) {
            llvm::Value* object = fa->obj->cc2llvm(env, deref);
            object = env.access(object, false, deref, 1);                 // RICH: Volatile.
            parameters.push_back(object);
        }
    }

    /* I really don;t like having to do this, but...
     * We walk both the argument list and the function's parameter list so that we can
     * determine if a given argument is passed by reference.
     */
    
    VDEBUG("E_funCall function", loc, cout << func->asString());
    VDEBUG("E_funCall func type", loc, cout << func->type->toString());
    FunctionType *ft;
    if (func->type->isPtrOrRef()) {
        ft = func->type->getAtType()->getAtType()->asFunctionType();
    } else {
        ft = func->type->asFunctionType();
    }
    SObjListIter<Variable> parms(ft->params);
    FAKELIST_FOREACH(ArgExpression, args, arg) {
        Variable const *parameter = NULL;
        if (!parms.isDone()) {
            parameter = parms.data();
            parms.adv();
        }
        bool ref = parameter && parameter->type && parameter->type->isReference();
        llvm::Value* param = arg->expr->cc2llvm(env, deref);
        VDEBUG("Param", loc, cout << (arg->expr->type->isReference() ? "&" : "") << arg->expr->asString() << " "; param->print(cout));
        param = env.access(param, false, deref, ref ? 1 : 0);                 // RICH: Volatile.
        // RICH: param = env.access(param, false, deref);                 // RICH: Volatile.
        VDEBUG("Param after", loc, param->print(cout));
        if (   param->getType()->getTypeID() == llvm::Type::ArrayTyID
            || (   param->getType()->getTypeID() == llvm::Type::PointerTyID
                && param->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID)) {
            // This is somewhat of a hack: It should be done in cc_tcheck.cc.
            // Add an implicit cast of &array to *array.
            const llvm::Type* type = llvm::PointerType::get(param->getType()->getContainedType(0)->getContainedType(0), 0); // RICH: Address space.
	    param = env.builder.CreateBitCast(param, type);
        }
        parameters.push_back(param);
        VDEBUG("Param", loc, param->print(cout));
    }

    deref = 0;
    if (!function) {
        // This is not a method call.
        function = func->cc2llvm(env, deref);
    }
    function = env.access(function, false, deref);                 // RICH: Volatile.
    VDEBUG("CreateCall", loc, function->print(cout));
    return env.builder.CreateCall(function, parameters.begin(), parameters.end());
}

llvm::Value *E_constructor::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    VDEBUG("E_constructor", loc, cout << ctorVar->toString());
    VDEBUG("E_constructor retObj", loc, cout << retObj->asString());
    FAKELIST_FOREACH(ArgExpression, args, arg) {
        VDEBUG("E_constructor arg", arg->expr->loc, cout << arg->expr->asString());
    }
    
    llvm::Value* object = retObj->cc2llvm(env, deref);
    env.constructor(object, this);
    return object;
}

llvm::Value *E_fieldAcc::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    // 'field' is the member variable.
    // The member will have been previously seen in a declaration.
    VDEBUG("E_field obj", loc, cout << obj->asString());
    llvm::Value* object = obj->cc2llvm(env, deref);
    object = env.access(object, false, deref, 1);                 // RICH: Volatile.
    VDEBUG("E_field field", loc, cout << field->toString());
    llvm::Value* value = env.members.get(field);
    if (value == NULL) {
        // Check for a static member.
        value = env.variables.get(field);
        xassert(value && "An undeclared member has been referenced");
        xassert(value->getType()->getTypeID() == llvm::Type::PointerTyID && "expected pointer type");
        bool first = value->getType()->getContainedType(0)->isFirstClassType();

        VDEBUG("E_fieldAcc ID", loc, cout << value->getType()->getContainedType(0)->getTypeID());

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

    VDEBUG("E_field object", loc, cout << "ID " << object->getType()->getContainedType(0)->getTypeID() << " ";
        object->print(cout));
    VDEBUG("E_field value", loc, value->print(cout));
    std::vector<llvm::Value*> index;
    if (   object->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID
        || object->getType()->getContainedType(0)->getTypeID() == llvm::Type::StructTyID) {
        index.push_back(llvm::Constant::getNullValue(value->getType()));
    }
    index.push_back(value);
    env.checkCurrentBlock();
    llvm::Value *result = env.builder.CreateGEP(object, index.begin(), index.end());
    VDEBUG("E_field  after GEP", loc, result->print(cout));

    bool first = result->getType()->getContainedType(0)->isFirstClassType();
    deref = 0;
    if (!first) {
        // The object is not a first class object.
        deref = 0;
    } else {
        if (result->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID) {
            deref = 0;
        } else {
            // Need one dereference to get the actual object.
            deref = 1;
        }
    }

    VDEBUG("E_field deref", loc, cout << "deref " << deref << " "; result->print(cout));
    return result;
}

llvm::Value *E_sizeof::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    const llvm::Type* etype = env.makeTypeSpecifier(expr->loc, expr->type);

    VDEBUG("GEP6", loc, etype->print(cout));
    const llvm::Type* ptype = llvm::PointerType::get(etype, 0);       // RICH: Address space.
    llvm::Value* value = env.builder.CreateGEP(
        llvm::Constant::getNullValue(ptype),
        llvm::ConstantInt::get(env.targetData.getIntPtrType(), 1));
    value = env.builder.CreatePtrToInt(value, env.targetData.getIntPtrType());
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
    VDEBUG("E_effect value", loc, cout << "deref " << deref; value->print(cout));

    if (isPostfix(op)) {
        // A postfix operator: return the result from before the operation.
        int myderef = deref;
        result = env.access(value, false, myderef);                 // RICH: Volatile.
        temp = env.access(value, false, deref);                 // RICH: Volatile.
    } else {
        temp = env.access(value, false, deref);                 // RICH: Volatile.
    }

    VDEBUG("E_effect temp", loc, cout << "deref " << deref; temp->print(cout));
    if (temp->getType()->getTypeID() == llvm::Type::PointerTyID) {
        // This is a pointer increment/decrement.
	std::vector<llvm::Value*> index;
	llvm::Value* one;
	if (op == EFF_POSTDEC || op == EFF_PREDEC) {
	    // Get a negative one.
            one = llvm::ConstantInt::get(llvm::APInt(env.targetData.getTypeSizeInBits(temp->getType()), -1));
	} else {
	    // Get a positive one.
            one = llvm::ConstantInt::get(llvm::APInt(env.targetData.getTypeSizeInBits(temp->getType()), 1));
	}
	index.push_back(one);
        VDEBUG("GEP1", loc, temp->print(cout); );
	temp = env.builder.CreateGEP(temp, index.begin(), index.end());
        VDEBUG("Store7 source", loc, temp->print(cout));
        VDEBUG("Store7 destination", loc, value->print(cout));
        env.builder.CreateStore(temp, value, false);	// RICH: Volatile
    } else if (temp->getType()->isInteger()) {
        llvm::ConstantInt* one = llvm::ConstantInt::get(llvm::APInt(env.targetData.getTypeSizeInBits(temp->getType()), 1));
	if (op == EFF_POSTDEC || op == EFF_PREDEC) {
	    temp = env.builder.CreateSub(temp, one);
	} else {
	    temp = env.builder.CreateAdd(temp, one);
	}
        VDEBUG("Store8 source", loc, temp->print(cout));
        VDEBUG("Store8 destination", loc, value->print(cout));
        env.builder.CreateStore(temp, value, false);	// RICH: Volatile
    } else {
        cerr << toString(loc) << ": ";
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
CC2LLVMEnv::OperatorClass CC2LLVMEnv::makeCast(SourceLoc loc, Type* leftType,
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
	llvm::Value** value;
        Data(Type* type, llvm::Value** value) : type(type), value(value) {
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
	      size = etype->reprSize() * BITS_PER_BYTE;
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
                    size = (*value)->getType()->getPrimitiveSizeInBits();       // RICH: replace
                } else {
                    size = simpleTypeReprSize(st->type) * BITS_PER_BYTE;
                }
            } else if (isPointer) {
                size = type->reprSize() * BITS_PER_BYTE;
            }
	}
    };

    Data left(leftType, &leftValue);
    Data right(rightType, rightValue);
    Data* source = NULL;	// This will remain NULL if no cast is needed.
    Data* target = &right;

    if (Type::equalTypes(left.type, right.type)) {
        // Types identical. Do nothing.
    } else if (right.value == NULL) {
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
        VDEBUG("makeCast ptr types", loc, cout << "left " << left.type->toString() << " right " << right.type->toString());
	if (right.isPointer) {
	    // Check type, may need a bit cast.
	    if (right.type != left.type) {
		// Doesn't matter which way we go.
	        source = &left;
		target = &right;
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
        VDEBUG("makeCast int size", loc, cout << "left " << left.size << " right " << right.size);
        VDEBUG("makeCast int isUnsigned", loc, cout << "left " << left.isUnsigned << " right " << right.isUnsigned);
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
        cerr << toString(loc) << ": ";
	xunimp("cast");
    }

    if (source) {
        // We may need to generate a cast instruction.
	// We may be here with an enumeration constant. Handle no type.
	const llvm::Type* type;
	if (target->type == NULL) {
	    type = llvm::IntegerType::get(target->size);
	} else {
	    type = makeTypeSpecifier(loc, target->type);
	}

        VDEBUG("makeCast from type", loc, (*source->value)->getType()->print(cout));
        VDEBUG("makeCast to type", loc, type->print(cout));

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
                    VDEBUG("SExt1 source", loc, cout << "size " << source->size << " "; (*source->value)->print(cout));
                    VDEBUG("SExt1 destination", loc, cout << "size " << target->size << " "; type->print(cout));
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
                const llvm::Type* itype = llvm::IntegerType::get(targetData.getPointerSize());
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
                const llvm::Type* itype = llvm::IntegerType::get(targetData.getPointerSize());
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
llvm::Value* CC2LLVMEnv::initializer(const Initializer* init, Type* type, int& deref, bool top)
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
            cerr << toString(init->loc) << ": ";
            if (s) {
                // Initialize an array with a string.
                ArrayType *at = type->asArrayType();
                const llvm::Type* elttype = makeTypeSpecifier(init->loc, at->eltType);
                int size = at->getSize();
                if (size == ArrayType::NO_SIZE) {
                    size = s->data->getDataLen();
                } else if (size == ArrayType::DYN_SIZE) {
                    cerr << toString(init->loc) << ": ";
                    xunimp("dynamic array type in initializer");
                }

	        std::vector<llvm::Constant*> elements;
                for (int i = 0; i < size; ++i) {
                    elements.push_back(llvm::ConstantInt::get(elttype, i < s->data->getDataLen() ? s->data->getData()[i] : 0));
                }
	        value = llvm::ConstantArray::get((llvm::ArrayType*)makeTypeSpecifier(init->loc, type), elements);
                break;
            } else {
                xunimp("initializing an array with an expression");
            }
        } else {
            value = e->e->cc2llvm(*this, deref);
            if (!top) {
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
                cerr << toString(init->loc) << ": ";
                xunimp("dynamic array type in initializer");
	    }

	    xassert(size == c->inits.count());
	    std::vector<llvm::Constant*> elements;
            FOREACH_ASTLIST(Initializer, c->inits, iter) {
                elements.push_back((llvm::Constant*)initializer(iter.data(), at->eltType, deref));
            }
	    value = llvm::ConstantArray::get((llvm::ArrayType*)makeTypeSpecifier(init->loc, type), elements);
	    break;
	} else if (type->isCompoundType()) {
            CompoundType *ct = type->asCompoundType();
            ASTListIter<Initializer> iiter(c->inits);
            SFOREACH_OBJLIST(Variable, ct->dataMembers, iter) {
                Variable const *v = iter.data();
                if (!iiter.isDone()) {
                    // Have an initializer for this.
                    initializer(iiter.data(), v->type, deref);
                    iiter.adv();
                } else {
                    // No initializer present.
                    cerr << toString(init->loc) << ": ";
                    xunimp("missing compound initializer");
                }
            }
	}
    }

    ASTNEXTC(IN_ctor, c) {
        // RICH: cerr << toString(init->loc) << ": ";
        // RICH: xunimp("ctor initializer");
    }

    ASTNEXTC(IN_designated, d) {
        cerr << toString(init->loc) << ": ";
        xunimp("designated initializer");
    }

    ASTENDCASED
    }

    return value;
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
    return type->isIntegerType();
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
        VDEBUG("E_binary left", e1->loc, cout <<  " deref " << deref1 << " "; left->print(cout));
        right = e2->cc2llvm(env, deref2);
        VDEBUG("E_binary right", e1->loc, right->print(cout));
    }
    llvm::Value* result = env.binop(loc, op, e1, left, deref1, e2, right, deref2);
    deref = 0;
    return result;
}

llvm::Value* CC2LLVMEnv::binop(SourceLoc loc, BinaryOp op, Expression* e1, llvm::Value* left, int deref1,
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
	    xunimp("%");
	    break;
	}
        break;

    case BIN_PLUS:      // +
    case BIN_MINUS: {    // -
        VDEBUG("Plus left", loc, cout << e1->type->toString());
        VDEBUG("Plus right", loc, cout << e2->type->toString());
        Expression* te1 = e1;
        Expression* te2 = e2;
	if (op == BIN_PLUS) {
            if ((isPtr(e1->type) && isInt(e2->type)) || (isInt(e1->type) && isPtr(e2->type))) {
                // This could be an array reference *(a + i).
                VDEBUG("+/- checking",  loc, cout << e1->type->toString() << " " << e2->type->toString());
                if (e1->type->isIntegerType() && e2->type->isPtrOrRef()) {
                    // Place the integer on the right.
                    te1 = e2;
                    te2 = e1;
                   
                    llvm::Value* temp = left;
                    left = right;
                    right = temp;
                    VDEBUG("swapping i + p", loc, );
                
                    right = access(right, false, deref1);                 // RICH: Volatile.

                    VDEBUG("after swapping left", loc, left->print(cout));
                    VDEBUG("after swapping right", loc, right->print(cout));
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
            left = access(left, false, deref1);                 // RICH: Volatile.

            if (op == BIN_MINUS) {
                // Negate the integer value.
                right = access(right, false, deref2);                 // RICH: Volatile.
                VDEBUG("BIN_MINUS right", loc, right->getType()->print(cout); cout << " "; right->print(cout));
                llvm::Value* zero = llvm::Constant::getNullValue(right->getType());
                right = builder.CreateSub(zero, right);
            }

            // Make sure the pointer and index sizes match.
            uint64_t lsize = targetData.getTypeSizeInBits(left->getType());
            uint64_t rsize = targetData.getTypeSizeInBits(right->getType());
            if (lsize != rsize) {
                if (lsize > rsize) {
                    // The pointer is bigger, check for signed vs. unsigned.
                    const SimpleType* st = te2->type->asReferenceTypeC()->getAtType()->asSimpleTypeC();
                    if (   right->getType()->getPrimitiveSizeInBits() == 1
                        || ::isExplicitlyUnsigned(st->type)) {
                        right = builder.CreateZExt(right, targetData.getIntPtrType());
                    } else {
                        VDEBUG("SExt2 source", loc, right->print(cout));
                        VDEBUG("SExt2 destination ", loc, targetData.getIntPtrType()->print(cout));
                        right = builder.CreateSExt(right, targetData.getIntPtrType());
                    }
                } else {
                    // The integer is bigger.
                    right = builder.CreateTrunc(right, targetData.getIntPtrType());
                }
            }

            std::vector<llvm::Value*> index;
            if (   left->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID
                || left->getType()->getContainedType(0)->getTypeID() == llvm::Type::StructTyID) {
                VDEBUG("NullValue2", loc, right->getType()->print(cout));
                index.push_back(llvm::Constant::getNullValue(right->getType()));
            }
            index.push_back(right);
            VDEBUG("GEP2 left", loc, left->print(cout));
            VDEBUG("GEP2 right", loc, right->print(cout));
            result = builder.CreateGEP(left, index.begin(), index.end());
            VDEBUG("GEP2 result", loc, result->print(cout));
            return result;
        }

        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
        VDEBUG("makeCast left", loc, cout << te1->type->toString() << " "; left->print(cout));
        VDEBUG("makeCast right", loc, cout << te2->type->toString() << " "; right->print(cout));
	c = makeCast(loc, te1->type, left, te2->type, &right);
        VDEBUG("PlusOrMinus left", loc, left->print(cout));
        VDEBUG("PlusOrMinus right", loc, right->print(cout));
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
                llvm::ConstantInt::get(targetData.getIntPtrType(), 1));
            size = builder.CreatePtrToInt(size, targetData.getIntPtrType());
	    left = builder.CreatePtrToInt(left, targetData.getIntPtrType());
	    right = builder.CreatePtrToInt(right, targetData.getIntPtrType());
            result = builder.CreateSub(left, right);
            result = builder.CreateUDiv(result, size);
	    break;
        }
	case CC2LLVMEnv::OC_VOID:
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
            cerr << toString(loc) << ": ";
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
        llvm::Value* tValue = llvm::ConstantInt::getTrue();
	builder.CreateBr(next);
        currentBlock = NULL;

        setCurrentBlock(ifFalse);
        llvm::Value* fValue = llvm::ConstantInt::getFalse();

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
        llvm::Value* tValue = llvm::ConstantInt::getTrue();
	builder.CreateBr(next);
        currentBlock = NULL;

        setCurrentBlock(ifFalse);
        llvm::Value* fValue = llvm::ConstantInt::getFalse();

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
        cerr << toString(loc) << ": ";
        xunimp("<?");
        break;
    case BIN_MAXIMUM:   // >?
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
        cerr << toString(loc) << ": ";
        xunimp(">?");
        break;

    // this exists only between parsing and typechecking
    case BIN_BRACKETS:  // []
        break;		// Never get here.

    case BIN_ASSIGN:    // = (used to denote simple assignments in AST, as opposed to (say) "+=")
        break;		// Never get here, see E_assign.

    // C++ operators
    case BIN_DOT_STAR:    // .*
        cerr << toString(loc) << ": ";
        xunimp(".*");
        break;
    case BIN_ARROW_STAR:  // ->*
        cerr << toString(loc) << ": ";
        xunimp("->*");
        break;

    // theorem prover extension
    case BIN_IMPLIES:     // ==>
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
        cerr << toString(loc) << ": ";
        xunimp("==>");
        break;
    case BIN_EQUIVALENT:  // <==>
        left = access(left, false, deref1);                 // RICH: Volatile.
        right = access(right, false, deref2);                 // RICH: Volatile.
        cerr << toString(loc) << ": ";
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
    bool first = source->getType()->getContainedType(0)->isFirstClassType();
    if (   first
        && source->getType()->getContainedType(0)->getTypeID() != llvm::Type::ArrayTyID
        && source->getType()->getContainedType(0)->getTypeID() != llvm::Type::StructTyID) {
        ++deref;
    }
    VDEBUG("E_deref", loc, cout << "deref " << deref << " "; source->print(cout));
    return source;
}

llvm::Value *E_cast::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* result = expr->cc2llvm(env, deref);
    result = env.access(result, false, deref);                 // RICH: Volatile.
    VDEBUG("cast from", loc, result->print(cout); cout << " to " << type->toString());
    env.makeCast(loc, expr->type, result, type);
    deref = 0;
    return result;
}

llvm::Value *E_stdConv::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* result = expr->cc2llvm(env, deref);
    result = env.access(result, false, deref);                 // RICH: Volatile.
    VDEBUG("stdConv from", loc, result->print(cout); cout << " to " << type->toString());
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
    VDEBUG("E_conv true", loc, trueValue->print(cout); cout << " is " << th->type->toString());
    if (env.currentBlock) {
        // Close the current block.
        llvm::BranchInst::Create(next, env.currentBlock);
        env.currentBlock = NULL;
    }

    env.setCurrentBlock(ifFalse);
    llvm::Value* falseValue = el->cc2llvm(env, deref);
    falseValue = env.access(falseValue, false, deref);                 // RICH: Volatile.
    VDEBUG("E_conv false", loc, falseValue->print(cout); cout << " is " << el->type->toString());

    env.makeCast(loc, th->type, trueValue, el->type, &falseValue);
    env.setCurrentBlock(next);
    llvm::Value* result = NULL;
    if (trueValue->getType()->isFirstClassType() && falseValue->getType()->isFirstClassType()) {
        llvm::PHINode* phi = env.builder.CreatePHI(trueValue->getType());
	phi->addIncoming(trueValue, ifTrue);
	phi->addIncoming(falseValue, ifFalse);
        result = phi;
    } else {
        cerr << toString(loc) << ": ";
        xunimp("?:");
    }
    return result;
}

llvm::Value *E_sizeofType::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    const llvm::Type* etype = env.makeTypeSpecifier(loc, atype->getType());
    VDEBUG("GEP5", loc, etype->print(cout));
    const llvm::Type* ptype = llvm::PointerType::get(etype, 0);       // RICH: Address space.
    llvm::Value* value = env.builder.CreateGEP(
        llvm::Constant::getNullValue(ptype),
        llvm::ConstantInt::get(env.targetData.getIntPtrType(), 1));
    value = env.builder.CreatePtrToInt(value, env.targetData.getIntPtrType());
    deref = 0;
    return value;
}

llvm::Value* CC2LLVMEnv::doassign(SourceLoc loc, llvm::Value* destination, int deref1, Type* dtype,
                                                 llvm::Value* source, int deref2, Type* stype)
{
    checkCurrentBlock();
    if (   destination->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID
        || destination->getType()->getContainedType(0)->getTypeID() == llvm::Type::StructTyID) {
        // This is a compound assignment.
#if 1 // RICH: These are now first class types.
        if (llvm::ConstantArray::classof(source)) {
#if RICH
            llvm::GlobalVariable* gv = new llvm::GlobalVariable(source->getType(), true, llvm::GlobalValue::InternalLinkage, (llvm::Constant*)source, ".ar", mod);
            // Get the address of the array.
            checkCurrentBlock();
            VDEBUG("GEP5", loc, );
            source = builder.CreateGEP(gv, llvm::Constant::getNullValue(destination->getType()), "");
            VDEBUG("GEP5", loc, source->print(cout));
#endif
            --deref2;
        }
        // ++deref1;
        // ++deref2;
        source = access(source, false, deref2);                 // RICH: Volatile.
        VDEBUG("doassign get struct", loc, destination->print(cout));
        destination = access(destination, false, deref1, 1);                 // RICH: Volatile.
        VDEBUG("doassign source struct", loc, source->print(cout));
        VDEBUG("doassign destination struct", loc, destination->print(cout));
        new llvm::StoreInst(source, destination, false, currentBlock);	// RICH: isVolatile
        return source;
#else
        source = access(source, false, deref2, 1);                 // RICH: Volatile.
        destination = access(destination, false, deref1, 1);                 // RICH: Volatile.
        llvm::Function* function = llvm::Intrinsic::getDeclaration(mod, llvm::Intrinsic::memcpy_i32); // RICH: size.
        std::vector<llvm::Value*> parameters;
        const llvm::Type* type = llvm::IntegerType::get(BITS_PER_BYTE);
        type =  llvm::PointerType::get(type, 0);	// RICH: address space.
        checkCurrentBlock();
        VDEBUG("doassign dest", loc, destination->print(cout));
        llvm::Value* value = builder.CreateBitCast(destination, type);
        VDEBUG("doassign dest cast", loc, value->print(cout));
        parameters.push_back(value);
        if (llvm::ConstantArray::classof(source)) {
            llvm::GlobalVariable* gv = new llvm::GlobalVariable(source->getType(), true, llvm::GlobalValue::InternalLinkage, (llvm::Constant*)source, ".ar", mod);
            // Get the address of the array.
            checkCurrentBlock();
            VDEBUG("GEP4", loc, );
            source = builder.CreateGEP(gv, llvm::Constant::getNullValue(destination->getType()), "");
            VDEBUG("GEP4", loc, source->print(cout));
        }
        VDEBUG("doassign src", loc, source->print(cout));
        value = builder.CreateBitCast(source, type);
        VDEBUG("doassign src cast", loc, value->print(cout));
        parameters.push_back(value);
        const llvm::Type* ptype = llvm::PointerType::get(source->getType()->getContainedType(0), 0);       // RICH: Address space.
        value = builder.CreateGEP(
                llvm::Constant::getNullValue(ptype),
                llvm::ConstantInt::get(targetData.getIntPtrType(), 1));
        value = builder.CreatePtrToInt(value, targetData.getIntPtrType());
        parameters.push_back(value);
        value = llvm::ConstantInt::get(targetData.getIntPtrType(), 0);                  // RICH: Alignment.
        parameters.push_back(value);
        value = builder.CreateCall(function, parameters.begin(), parameters.end());
        VDEBUG("doassign memcpy", loc, value->print(cout));
        return source;
#endif // RICH
    } else {
        VDEBUG("doassign", loc, source->print(cout));
        source = access(source, false, deref2);                 // RICH: Volatile.
    }

    VDEBUG("doassign get", loc, destination->print(cout));
    destination = access(destination, false, deref1, 1);                 // RICH: Volatile.
    VDEBUG("doassign cast", loc, cout <<  stype->toString() << " -> " << dtype->toString());
    makeCast(loc, stype, source, dtype);
    VDEBUG("doassign source", loc, source->print(cout));
    VDEBUG("doassign destination", loc, destination->print(cout));
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

    VDEBUG("E_assign", loc, cout << "result "; temp->getType()->print(cout);
                            cout << " destination "; destination->getType()->getContainedType(0)->print(cout));
    if (temp->getType() != destination->getType()->getContainedType(0)) {
        // Cast the result to the destination type.
        env.makeCast(loc, src->type, temp, target->type);
    }
    VDEBUG("Store2 source", loc, temp->print(cout));
    VDEBUG("Store2 destination", loc, destination->print(cout));
    deref = deref1;
    new llvm::StoreInst(temp, destination, false, env.currentBlock);	// RICH: Volatile
    return destination;
}

llvm::Value *E_new::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("new");
    return NULL;
}

llvm::Value *E_delete::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("delete");
    return NULL;
}

llvm::Value *E_throw::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("throw");
    return NULL;
}

llvm::Value *E_keywordCast::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("keyword cast");
    return NULL;
}

llvm::Value *E_typeidExpr::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("typeid");
    return NULL;
}

llvm::Value *E_typeidType::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("typeid");
    return NULL;
}

llvm::Value *E_grouping::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("grouping");
    return NULL;
}

llvm::Value *E_arrow::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("->");
    return NULL;
}


llvm::Value *E_addrOfLabel::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("label address");
    return NULL;
}

llvm::Value *E_gnuCond::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("gnu conditional");
    return NULL;
}

llvm::Value *E_alignofExpr::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("alignof");
    return NULL;
}

llvm::Value *E_alignofType::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("alignof");
    return NULL;
}

llvm::Value *E___builtin_va_start::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* value = expr->cc2llvm(env, deref);
    value = env.access(value, false, deref, 1);                 // RICH: Volatile.
    deref = 0;
    const llvm::Type* type = llvm::IntegerType::get(BITS_PER_BYTE);
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
    cerr << toString(loc) << ": ";
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
    const llvm::Type* type = llvm::IntegerType::get(BITS_PER_BYTE);
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
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("__builtin_constant_p");
    return NULL;
}

llvm::Value *E___builtin_alloca::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    llvm::Value* value = expr->cc2llvm(env, deref);
    value = env.access(value, false, deref, 1);                 // RICH: Volatile.
    deref = 0;
    xassert(env.entryBlock);
    const llvm::Type* type = llvm::IntegerType::get(BITS_PER_BYTE);
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
    cerr << toString(loc) << ": ";
    xunimp("compound literal");
    return NULL;
}

llvm::Value *E_statement::cc2llvm(CC2LLVMEnv &env, int& deref) const
{
    deref = 0;
    cerr << toString(loc) << ": ";
    xunimp("statement expression");
    return NULL;
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
llvm::Module* cc_to_llvm(string name, StringTable &str, TranslationUnit const &input, string targetData, string targetTriple)
{
    llvm::IRBuilder builder;
    CC2LLVMEnv env(str, name, input, targetData, targetTriple, builder);
    return env.doit();
}

// EOF
