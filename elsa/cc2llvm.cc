// cc2llvm.cc
// code for cc2llvm.h

#include "cc2llvm.h"         // this module

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
#include <llvm/ParameterAttributes.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Support/LLVMBuilder.h>
#include <llvm/Analysis/Verifier.h>

#define BITS_PER_BYTE	8	// RICH: Temporary.

// -------------------- CC2LLVMEnv ---------------------
CC2LLVMEnv::CC2LLVMEnv(StringTable &s, string name, const TranslationUnit& input,
                       string targetData, string targetTriple,
		       llvm::LLVMBuilder& builder)
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

/** Make sure the current block has been opened.
 */
void CC2LLVMEnv::checkCurrentBlock()
{
    if (currentBlock == NULL) {
        // No block is current, make one.
        setCurrentBlock(new llvm::BasicBlock("", function, returnBlock));
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

/** Check a condition in preparation for a branch.
 */
llvm::Value* CC2LLVMEnv::checkCondition(SourceLoc loc, llvm::Value* value)
{
    const llvm::Type* ctype = value->getType();
    if (ctype != llvm::Type::Int1Ty)
    {
        // Not a boolean, check for non-zero.
        checkCurrentBlock();
	llvm::Type::TypeID id = ctype->getTypeID();
        llvm::Value* zero = llvm::Constant::getNullValue(value->getType());
	if (id == llvm::Type::IntegerTyID || id == llvm::Type::PointerTyID) {
	    value = builder.CreateICmpNE(value, zero);
	} else if (id == llvm::Type::FloatTyID) {
	    // RICH: ordered vs. unordered.
	    value = builder.CreateFCmpONE(value, zero);
	} else {
            cerr << toString(loc) << ": ";
            xunimp("checkCondition");
	}
    }
    return value;
}

/** Check a condition in preparation for a branch.
 */
llvm::Value* CC2LLVMEnv::checkCondition(Expression* cond)
{
    return checkCondition(cond->loc, cond->cc2llvm(*this));
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
        type =  llvm::PointerType::get(type);
        break;
    }
    case Type::T_REFERENCE: {
	// The type of a reference is the underlying type. (RICH: is it?)
        ReferenceType *rt = t->asReferenceType();
        type =  makeTypeSpecifier(loc, rt->atType);
        break;
    }
    case Type::T_FUNCTION: {
        FunctionType *ft = t->asFunctionType();
        const llvm::Type* returnType = makeTypeSpecifier(loc, ft->retType);
        std::vector<const llvm::Type*>args;
	if (returnType->getTypeID() == llvm::Type::StructTyID) {
	    /* LLVM does not support a compound return type.
             * We'll call it a pointer here. In practice, a pointer to
	     * the return value holding area will be passed as the first
	     * argument to the function. The function the returns void.
	     */
	    const llvm::Type* rt = llvm::PointerType::get(returnType);
            args.push_back(rt);
            returnType = llvm::Type::VoidTy;
	}
        makeParameterTypes(ft, args);
        type = llvm::FunctionType::get(returnType, args, ft->acceptsVarargs());
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
	    type = llvm::Type::FP128Ty;	// RICH: Is this right?
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

	// Get the members.
	std::vector<const llvm::Type*>fields;
        SFOREACH_OBJLIST(Variable, ct->dataMembers, iter) {
            Variable const *v = iter.data();
	    fields.push_back(makeTypeSpecifier(v->loc, v->type));
        }

	llvm::StructType* st = llvm::StructType::get(fields, false);	// RICH: isPacked
        llvm::cast<llvm::OpaqueType>(fwd.get())->refineAbstractTypeTo(st);
	type = llvm::cast<llvm::Type>(fwd.get());
        compounds.add(ct, type);
        break;
    }

    case AtomicType::T_ENUM:
        cerr << toString(loc) << ": ";
        xunimp("enum");
        break;
      
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
           args.push_back(type);
        }
    }
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

    ASTDEFAULTC {
        cerr << toString(loc) << ": ";
        xunimp("TopForm");
    }
    
    ASTENDCASE
    }
}

void Function::cc2llvm(CC2LLVMEnv &env) const
{
    if (inits->isNotEmpty()) {
        cerr << toString(nameAndParams->var->loc) << ": ";
        xunimp("member initializers");
    }

    if (handlers->isNotEmpty()) {
        cerr << toString(nameAndParams->var->loc) << ": ";
        xunimp("exception handlers");
    }

    const Function* oldFunctionAST = env.functionAST;	// Handle nested functions.
    env.functionAST = this;
    const llvm::Type* returnType = env.makeTypeSpecifier(nameAndParams->var->loc, funcType->retType);
    std::vector<const llvm::Type*>args;
    env.makeParameterTypes(funcType, args);
    llvm::FunctionType* ft = llvm::FunctionType::get(returnType, args, funcType->acceptsVarargs());
    env.function = new llvm::Function(ft, llvm::GlobalValue::ExternalLinkage,	// RICH: Linkage.
        nameAndParams->var->name, env.mod);

    env.function->setCallingConv(llvm::CallingConv::C); // RICH: Calling convention.
    env.variables.add(nameAndParams->var, env.function);
    env.entryBlock = new llvm::BasicBlock("entry", env.function, NULL);

    // Set the initial current block.
    env.setCurrentBlock(env.entryBlock);

    // Add the parameter names.
    llvm::Function::arg_iterator llargs = env.function->arg_begin();
    SFOREACH_OBJLIST(Variable, funcType->params, iter) {
        const Variable *param = iter.data();
        llvm::Value* arg = llargs++;
	// Make space for the argument.
	const llvm::Type* type = arg->getType();
	// type will be NULL for "...".
	if (type) {
	    llvm::AllocaInst* addr = env.builder.CreateAlloca(type, NULL, param->name);
	    // Remember where the argument can be retrieved.
            env.variables.add(param, addr);
	    // Store the argument for later use.
	    env.builder.CreateStore(arg, addr, false);	// RICH: IsVolatile.
        }
    }

    // Set up the return block.
    env.returnBlock = new llvm::BasicBlock("return", env.function, 0);
    // Set up the return value.
    if (returnType == llvm::Type::VoidTy) {
        // A void function.
        env.returnValue = NULL;
        new llvm::ReturnInst(env.returnBlock);
    } else {
        // Create the return value holder.
	
        env.returnValue = env.builder.CreateAlloca(returnType, NULL, "retval");

        // RICH: This should happen in main() only: Default return value.
        llvm::Constant* nullInt = llvm::Constant::getNullValue(returnType);
        env.builder.CreateStore(nullInt, env.returnValue, false);	// RICH: main() only.

        // Generate the function return.
        llvm::LoadInst* rv = new llvm::LoadInst(env.returnValue, "", false, env.returnBlock);	// RICH: Volatile
        new llvm::ReturnInst(rv, env.returnBlock);
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

    // Generate a new declaration for each declarator.
    //
    // This is convenient for separating concerns, and necessary in
    // the case of declarations containing pointer-to-member,
    // because the PTM has to be handled separately, for example:
    //
    //   int i, C::*ptm;
    //
    // must become two declarations:
    //
    //  int i;
    //  struct ptm__C_int { ... } ptm;
    //
    FAKELIST_FOREACH(Declarator, decllist, declarator) {
        // At this point we exclusively consult the Type and Variable
        // system; syntax is irrelevant, only semantics matters.
        Variable *var = declarator->var;

        // But elaborated statements are relevant; for now, fail if
        // they are present.
        if (declarator->ctorStatement || declarator->dtorStatement) {
            cerr << toString(var->loc) << ": ";
            xunimp("ctorStatement or dtorStatement");
        }

        // Get any initializer.        
        llvm::Value* init = env.initializer(declarator->init, var->type);

        // Create the full generated declaration.
        const llvm::Type* type = env.makeTypeSpecifier(var->loc, var->type);
        if (var->type->getTag() == Type::T_FUNCTION) {
            llvm::Function* gf = new llvm::Function((llvm::FunctionType*)type, 
                (var->flags & DF_STATIC) ? llvm::GlobalValue::InternalLinkage : llvm::GlobalValue::ExternalLinkage,
	        env.makeName(var)->name, env.mod);
            gf->setCallingConv(llvm::CallingConv::C); // RICH: Calling convention.
            env.variables.add(var, gf);
        } else if (var->type->getTag() == Type::T_DEPENDENTSIZEDARRAY) {
            cerr << toString(var->loc) << ": ";
            xunimp("dependent sized array");
        } else if (var->type->getTag() == Type::T_LAST_TYPE_TAG) {
            cerr << toString(var->loc) << ": ";
            xunimp("last type tag");
        } else if (var->flags & (DF_STATIC|DF_GLOBAL)) {
	    // A global variable.
            if (init == NULL) {
                init = llvm::Constant::getNullValue(type);
            }
            llvm::GlobalVariable* gv = new llvm::GlobalVariable(type, false,	// RICH: isConstant
                (var->flags & DF_STATIC) ? llvm::GlobalValue::InternalLinkage : llvm::GlobalValue::ExternalLinkage,
	        (llvm::Constant*)init, env.makeName(var)->name, env.mod);
            env.variables.add(var, gv);
        } else {
            // A local variable.
            xassert(env.entryBlock);
            llvm::AllocaInst* lv;
            if (env.entryBlock == env.currentBlock) {
                lv = new llvm::AllocaInst(type, env.makeName(var)->name, env.entryBlock);
	    } else {
                lv = new llvm::AllocaInst(type, env.makeName(var)->name, env.entryBlock->getTerminator());
	    }

	    if (init) {
	        env.checkCurrentBlock();
	        env.builder.CreateStore(init, lv, false);	// RICH: isVolatile.
	    }
            env.variables.add(var, lv);
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
        block = new llvm::BasicBlock(label, env.function, env.returnBlock);
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
    llvm::BasicBlock* block = new llvm::BasicBlock("case", env.function, env.returnBlock);
    env.setCurrentBlock(block);
    llvm::Value* value = expr->cc2llvm(env);
    env.makeCast(loc, expr->type, value, env.switchType);
    // LLVM will complain if the value is not a constant integer.
    // Not clearly. The verifier complains about dominators.
    env.switchInst->addCase((llvm::ConstantInt*)value, block);
    s->cc2llvm(env);
}

void S_default::cc2llvm(CC2LLVMEnv &env) const
{
    xassert(env.switchInst);
    llvm::BasicBlock* block = new llvm::BasicBlock("default", env.function, env.returnBlock);
    env.setCurrentBlock(block);
    env.switchInst->setSuccessor(0, block);
    s->cc2llvm(env);
}

void S_expr::cc2llvm(CC2LLVMEnv &env) const
{
    expr->cc2llvm(env);
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
    llvm::BasicBlock* ifTrue = new llvm::BasicBlock("ifTrue", env.function, env.returnBlock);
    llvm::BasicBlock* ifFalse = new llvm::BasicBlock("ifFalse", env.function, env.returnBlock);
    llvm::BasicBlock* next = new llvm::BasicBlock("next", env.function, env.returnBlock);

    env.checkCurrentBlock();
    env.builder.CreateCondBr(value, ifTrue, ifFalse);
    env.currentBlock = NULL;

    env.setCurrentBlock(ifTrue);
    thenBranch->cc2llvm(env);

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
    env.nextBlock = new llvm::BasicBlock("next", env.function, env.returnBlock);
    // Generate the condition.
    const CN_expr* condition = cond->asCN_exprC();
    llvm::Value* value = condition->expr->expr->cc2llvm(env);

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

    env.continueBlock = new llvm::BasicBlock("continue", env.function, env.returnBlock);
    llvm::BasicBlock* bodyBlock = new llvm::BasicBlock("body", env.function, env.returnBlock);
    env.nextBlock = new llvm::BasicBlock("next", env.function, env.returnBlock);

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

    llvm::BasicBlock* bodyBlock = new llvm::BasicBlock("body", env.function, env.returnBlock);
    env.continueBlock = new llvm::BasicBlock("continue", env.function, env.returnBlock);
    env.nextBlock = new llvm::BasicBlock("next", env.function, env.returnBlock);
    if (env.currentBlock) {
        // Close the current block.
        env.builder.CreateBr(env.continueBlock);
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

    llvm::BasicBlock* testBlock = new llvm::BasicBlock("test", env.function, env.returnBlock);
    llvm::BasicBlock* bodyBlock = new llvm::BasicBlock("body", env.function, env.returnBlock);
    env.continueBlock = new llvm::BasicBlock("continue", env.function, env.returnBlock);
    env.nextBlock = new llvm::BasicBlock("next", env.function, env.returnBlock);

    env.checkCurrentBlock();
    // Handle the for() initialization.
    init->cc2llvm(env);

    // Set the current block.
    env.setCurrentBlock(testBlock);

    // Generate the test.
    const CN_expr* condition = cond->asCN_exprC();
    llvm::Value* value = env.checkCondition(condition->expr->expr);
    env.builder.CreateCondBr(value, bodyBlock, env.nextBlock);
    env.currentBlock = NULL;

    env.setCurrentBlock(bodyBlock);
    body->cc2llvm(env);

    // Handle the continue block.
    env.setCurrentBlock(env.continueBlock);
    after->cc2llvm(env);
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
        xassert(env.returnValue && "return a value in a function returning void");
        llvm::Value* value = expr->cc2llvm(env);
	env.makeCast(loc, expr->expr->type, value, env.functionAST->funcType->retType);
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
        block = new llvm::BasicBlock(label, env.function, env.returnBlock);
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
llvm::Value *Expression::cc2llvmNoSideEffects(CC2LLVMEnv &env, bool lvalue) const
{
    // RICH:
    // provide a place to put generated statements
    //   TRAP_SIDE_EFFECTS_STMT(temp);

    llvm::Value *ret = cc2llvm(env, lvalue);
  
    // confirm that nothing got put there
    // xassert(temp.stmts.isEmpty());
  
    return ret;
}

llvm::Value *E_boolLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    return llvm::ConstantInt::get(llvm::APInt(1, (int)b));
}

llvm::Value *E_intLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
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

    return llvm::ConstantInt::get(llvm::APInt(type->reprSize() * BITS_PER_BYTE, p, radix));
}

llvm::Value *E_floatLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    const llvm::Type* ftype = env.makeTypeSpecifier(loc, type);
    const llvm::fltSemantics* semantics;
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
    return llvm::ConstantFP::get(ftype, llvm::APFloat(*semantics, text));
}

llvm::Value *E_stringLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    // RICH: fullTextNQ is not quite right: has e.g. "\n" not replaced with \x10.
    llvm::Constant* c = llvm::ConstantArray::get(fullTextNQ, true);
    // RICH: Sizeof char.
    llvm::ArrayType* at = llvm::ArrayType::get(llvm::IntegerType::get(BITS_PER_BYTE), strlen(fullTextNQ) + 1);
    // RICH: Non-constant strings?
    llvm::GlobalVariable* gv = new llvm::GlobalVariable(at, true, llvm::GlobalValue::InternalLinkage, c, ".str", env.mod);

    // Get the address of the string as an open array.
    env.checkCurrentBlock();
    std::vector<llvm::Value*> indices;
    indices.push_back(llvm::Constant::getNullValue(llvm::IntegerType::get(32)));	// RICH: 32
    indices.push_back(llvm::Constant::getNullValue(llvm::IntegerType::get(32)));	// RICH: 32
    llvm::Instruction* address = env.builder.CreateGEP(gv, indices.begin(), indices.end(), "");
    return address;
}

llvm::Value *E_charLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    return llvm::ConstantInt::get(llvm::APInt(type->reprSize() * BITS_PER_BYTE, c));
}

llvm::Value *E_this::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("this");
    return NULL;
}

/** Get a variable used in an expression.
 */
llvm::Value *E_variable::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    if (var->isEnumerator()) {
        // This is an enumerator constant. Return it's value.
        return llvm::ConstantInt::get(llvm::APInt(type->reprSize() * BITS_PER_BYTE, var->getEnumeratorValue()));
    }

    // The variable will have been previously seen in a declaration.
    llvm::Value* value = env.variables.get(var);
    xassert(value && "An undeclared variable has been referenced");
    xassert(value->getType()->getTypeID() == llvm::Type::PointerTyID && "expected pointer type");
    bool first = value->getType()->getContainedType(0)->isFirstClassType();
    if (!lvalue && first) {
        // Return the value this represents.
        value = env.builder.CreateLoad(value, false);		// RICH: isVolatile
    }
    return value;
}

llvm::Value *E_funCall::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    env.checkCurrentBlock();
    std::vector<llvm::Value*> parameters;
    FAKELIST_FOREACH(ArgExpression, args, arg) {
        parameters.push_back(arg->expr->cc2llvm(env));
    }

    llvm::Value* function = func->cc2llvm(env, true);
    return env.builder.CreateCall(function, parameters.begin(), parameters.end());
}

llvm::Value *E_constructor::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("constructor");
    return NULL;
}

llvm::Value *E_fieldAcc::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("field access");
    return NULL;
}

llvm::Value *E_sizeof::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    const llvm::Type* etype = env.makeTypeSpecifier(expr->loc, expr->type);
    xassert(size != -1);
    uint64_t lsize = env.targetData.getTypeSizeInBits(etype);
    xassert(lsize / BITS_PER_BYTE == (unsigned)size);
    const llvm::Type* rtype = env.makeTypeSpecifier(loc, type);
    llvm::Value* value = llvm::ConstantInt::get(rtype, size);
    return value;
}


llvm::Value *E_unary::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    llvm::Value* value = expr->cc2llvm(env, lvalue);

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
	value = env.checkCondition(loc, value);
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
llvm::Value *E_effect::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    // Evaluate the expression as an lvalue.
    llvm::Value* value = expr->cc2llvm(env, true);
    llvm::Value* result = NULL;
    llvm::Value* temp = NULL;

    if (isPostfix(op)) {
        // A postfix operator: return the result from before the operation.
        result = env.builder.CreateLoad(value, false);	// RICH: Volatile
        temp = env.builder.CreateLoad(value, false);	// RICH: Volatile
    } else {
        temp = env.builder.CreateLoad(value, false);	// RICH: Volatile
    }

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
	result = env.builder.CreateGEP(temp, index.begin(), index.end());
    } else if (temp->getType()->isInteger()) {
        llvm::ConstantInt* one = llvm::ConstantInt::get(llvm::APInt(env.targetData.getTypeSizeInBits(temp->getType()), 1));
	if (op == EFF_POSTDEC || op == EFF_PREDEC) {
	    temp = env.builder.CreateSub(temp, one);
	} else {
	    temp = env.builder.CreateAdd(temp, one);
	}
        env.builder.CreateStore(temp, value, false);	// RICH: Volatile
    } else {
        cerr << toString(loc) << ": ";
        xunimp("++,--");
    }

    if (!isPostfix(op)) {
        result = temp;
    }
    return result;
}

/** Make two operand the same type or cast the first operand to the second type.
 * We use both the AST and LLVM information to do this.
 */
CC2LLVMEnv::OperatorClass CC2LLVMEnv::makeCast(SourceLoc loc, Type* leftType,
    llvm::Value*& leftValue, Type* rightType, llvm::Value** rightValue)
{
    // For operators, we need simple types or pointers.
    // At this point we can treat a refrence as its inderlying type.
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
	int size;
	Type * type;
	llvm::Value** value;
        Data(Type* type, llvm::Value** value) : type(type), value(value) {
            // Information needed convert and classify.
            isPointer = type->isPtrOrRef();
            isSimple = type->isSimpleType();
	    isInteger = false; isUnsigned = false; isFloat = false; size = 0;
	    if (type->isEnumType()) {
	      // We have to handle enumerated constants specially.
	      isInteger = true;
	      EnumType* etype = type->asCVAtomicType()->atomic->asEnumType();
	      isUnsigned = !etype->hasNegativeValues;
	      size = etype->reprSize();
	      type = NULL;	// See special handling below.
	    } else {
                xassert(isSimple || isPointer);
	    }
            if (isSimple) {
	        const SimpleType* st = type->asSimpleTypeC();
                isInteger = ::isIntegerType(st->type);
                isUnsigned = ::isExplicitlyUnsigned(st->type);
                isFloat = ::isFloatType(st->type);
                size = simpleTypeReprSize(st->type);
            }
	}
    };

    Data left(leftType, &leftValue);
    Data right(rightType, rightValue);
    Data* source = NULL;	// This will remain NULL if no cast is needed.
    Data* target = &right;

    if (right.value == NULL) {
        // This is a cast of the left value to the right type.
        target = &right;
        source = &left;
    } else if ((*right.value)->getType() == (*left.value)->getType()) {
        // No conversion is needed.
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
    } else {
        // Both sides are integers.
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
    } else {
        c = OC_OTHER;
        cerr << toString(loc) << ": ";
	xunimp("cast");
    }

    if (source) {
        // We may need to generate a cast instruction.
	// We may be here with an enumeration constant on the target side. Handle no type.
	const llvm::Type* type;
	if (target->type == NULL) {
	    type = llvm::IntegerType::get(target->size * BITS_PER_BYTE);
	} else {
	    type = makeTypeSpecifier(loc, target->type);
	}

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
		} else if (source->isUnsigned) {
		    // Zero extend the source value.
	            checkCurrentBlock();
	            *source->value = builder.CreateZExt(*source->value, type);
		} else {
		    // Sign extend the source value.
	            checkCurrentBlock();
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

	case OC_OTHER:
	    break;
	}
    }

    return c;
}

/** Create a value from an initializer.
 */
llvm::Value* CC2LLVMEnv::initializer(const Initializer* init, Type* type)
{
    if (init == NULL) {
        // No initializer.
	return NULL;
    }

    llvm::Value* value = NULL;

    // Handle an initializer.
    ASTSWITCHC(Initializer, init) {
    ASTCASEC(IN_expr, e) {
        value = e->e->cc2llvm(*this);
        makeCast(e->e->loc, e->e->type, value, type);
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
                elements.push_back((llvm::Constant*)initializer(iter.data(), at->eltType));
            }
	    value = llvm::ConstantArray::get((llvm::ArrayType*)makeTypeSpecifier(init->loc, type), elements);
	    break;
	} else if (type->isCompoundType()) {
            FOREACH_ASTLIST(Initializer, c->inits, iter) {
                initializer(iter.data(), NULL);
            }
            cerr << toString(init->loc) << ": ";
            xunimp("compound initializer");
	}
    }

    ASTNEXTC(IN_ctor, c) {
        cerr << toString(init->loc) << ": ";
        xunimp("ctor initializer");
    }

    ASTNEXTC(IN_designated, d) {
        cerr << toString(init->loc) << ": ";
        xunimp("designated initializer");
    }

    ASTENDCASED
    }

    return value;
}

llvm::Value *E_binary::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    env.checkCurrentBlock();
    llvm::Value* left = e1->cc2llvm(env);
    llvm::Value* right = e2->cc2llvm(env);
    llvm::Value* result = NULL;
    CC2LLVMEnv::OperatorClass c = CC2LLVMEnv::OC_OTHER;

    switch (op)
    {
    case BIN_EQUAL:	// ==
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = env.builder.CreateICmpEQ(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = env.builder.CreateFCmpOEQ(left, right);
            break;
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("==");
	    break;
	}
        break;

    case BIN_NOTEQUAL:	// !=
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = env.builder.CreateICmpNE(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = env.builder.CreateFCmpONE(left, right);
            break;
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("!=");
	    break;
	}
        break;

    case BIN_LESS:	// < 
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = env.builder.CreateICmpSLT(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = env.builder.CreateICmpULT(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = env.builder.CreateFCmpOLT(left, right);
            break;
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("<");
	    break;
	}
        break;

    case BIN_LESSEQ:	// <= 
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = env.builder.CreateICmpSLE(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = env.builder.CreateICmpULE(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = env.builder.CreateFCmpOLE(left, right);
            break;
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("<=");
	    break;
	}
        break;

    case BIN_GREATER:   // >
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = env.builder.CreateICmpSGT(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = env.builder.CreateICmpUGT(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = env.builder.CreateFCmpOGT(left, right);
            break;
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp(">");
	    break;
	}
        break;

    case BIN_GREATEREQ: // >=
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = env.builder.CreateICmpSGE(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = env.builder.CreateICmpUGE(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
	    // RICH: ordered vs. unordered.
            result = env.builder.CreateFCmpOGE(left, right);
            break;
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp(">=");
	    break;
	}
        break;

    case BIN_MULT:      // *
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
            result = env.builder.CreateMul(left, right);
            break;
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("*");
	    break;
	}
        break;

    case BIN_DIV:       // /
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = env.builder.CreateSDiv(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = env.builder.CreateUDiv(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
            result = env.builder.CreateFDiv(left, right);
            break;
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("/");
	    break;
	}
        break;

    case BIN_MOD:       // %
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = env.builder.CreateSRem(left, right);
	    break;
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
            result = env.builder.CreateURem(left, right);
            break;
	case CC2LLVMEnv::OC_FLOAT:
            result = env.builder.CreateFRem(left, right);
            break;
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("%");
	    break;
	}
        break;

    case BIN_PLUS:      // +
    case BIN_MINUS:     // -
        if (left->getType() != right->getType()) {
            // The types differ. This could be an array reference *(a + i).
	    if (right->getType()->getTypeID() == llvm::Type::PointerTyID) {
		// Place the pointer on the left.
	        llvm::Value* temp = right;
		left = right;
		right = temp;
	    }

	    if (   left->getType()->getTypeID() == llvm::Type::PointerTyID
	        && right->getType()->getTypeID() == llvm::Type::IntegerTyID) {
	        // If the left size is a pointer and the right side is an integer, calculate the address.
		if (op == BIN_MINUS) {
		    // Negate the integer value.
                    llvm::Value* zero = llvm::Constant::getNullValue(right->getType());
		    right = env.builder.CreateSub(zero, right);
		}
		std::vector<llvm::Value*> index;
		if (left->getType()->getContainedType(0)->getTypeID() == llvm::Type::ArrayTyID) {
	          index.push_back(llvm::Constant::getNullValue(right->getType()));
		}
	        index.push_back(right);
		result = env.builder.CreateGEP(left, index.begin(), index.end());
	        break;
	    }
        }

	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	    if (op == BIN_PLUS) {
                result = env.builder.CreateAdd(left, right);
	    } else {
                result = env.builder.CreateSub(left, right);
	    }
            break;
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("+/-");
	    break;
	}
        break;

    case BIN_LSHIFT:    // <<
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
            result = env.builder.CreateShl(left, right);
            break;
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("<<");
	    break;
	}
        break;

    case BIN_RSHIFT:    // >>
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
            result = env.builder.CreateAShr(left, right);
            break;
	case CC2LLVMEnv::OC_UINT:
            result = env.builder.CreateLShr(left, right);
            break;
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp(">>");
	    break;
	}
        break;

    case BIN_BITAND:    // &
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
            result = env.builder.CreateAnd(left, right);
            break;
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("&");
	    break;
	}
        break;

    case BIN_BITXOR:    // ^
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
            result = env.builder.CreateXor(left, right);
            break;
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("^");
	    break;
	}
        break;

    case BIN_BITOR:     // |
	c = env.makeCast(loc, e1->type, left, e2->type, &right);
	switch (c) {
	case CC2LLVMEnv::OC_SINT:
	case CC2LLVMEnv::OC_UINT:
            result = env.builder.CreateOr(left, right);
            break;
	case CC2LLVMEnv::OC_POINTER:
	case CC2LLVMEnv::OC_FLOAT:
	case CC2LLVMEnv::OC_OTHER:
            cerr << toString(loc) << ": ";
	    xunimp("|");
	    break;
	}
        break;

    case BIN_AND: {     // &&
        llvm::Value* value = env.checkCondition(e1);
        llvm::BasicBlock* doRight = new llvm::BasicBlock("doRight", env.function, env.returnBlock);
        llvm::BasicBlock* ifFalse = new llvm::BasicBlock("condFalse", env.function, env.returnBlock);
        llvm::BasicBlock* ifTrue = new llvm::BasicBlock("condTrue", env.function, env.returnBlock);
        llvm::BasicBlock* next = new llvm::BasicBlock("next", env.function, env.returnBlock);
        env.checkCurrentBlock();
        new llvm::BranchInst(ifTrue, ifFalse, value, env.currentBlock);
        env.currentBlock = NULL;

        env.setCurrentBlock(doRight);
        value = env.checkCondition(e2);
        env.checkCurrentBlock();
        new llvm::BranchInst(ifTrue, ifFalse, value, env.currentBlock);
        env.currentBlock = NULL;

        env.setCurrentBlock(ifTrue);
        llvm::Value* tValue = new llvm::AllocaInst(llvm::IntegerType::get(1), "", env.entryBlock->getTerminator());
	new llvm::StoreInst(llvm::ConstantInt::getTrue(), tValue, false, env.currentBlock);
        new llvm::BranchInst(next, env.currentBlock);
        env.currentBlock = NULL;

        env.setCurrentBlock(ifFalse);
        llvm::Value* fValue = new llvm::AllocaInst(llvm::IntegerType::get(1), "", env.entryBlock->getTerminator());
	new llvm::StoreInst(llvm::ConstantInt::getTrue(), fValue, false, env.currentBlock);

        env.setCurrentBlock(next);
        llvm::PHINode* phi = new llvm::PHINode(tValue->getType(), "", env.currentBlock);
	phi->addIncoming(tValue, ifTrue);
	phi->addIncoming(fValue, ifFalse);
        result = phi;
        break;
    }

    case BIN_OR: {      // ||
        llvm::Value* value = env.checkCondition(e1);
        llvm::BasicBlock* doRight = new llvm::BasicBlock("doRight", env.function, env.returnBlock);
        llvm::BasicBlock* ifFalse = new llvm::BasicBlock("condFalse", env.function, env.returnBlock);
        llvm::BasicBlock* ifTrue = new llvm::BasicBlock("condTrue", env.function, env.returnBlock);
        llvm::BasicBlock* next = new llvm::BasicBlock("next", env.function, env.returnBlock);
        env.checkCurrentBlock();
        new llvm::BranchInst(ifTrue, doRight, value, env.currentBlock);
        env.currentBlock = NULL;

        env.setCurrentBlock(doRight);
        value = env.checkCondition(e2);
        env.checkCurrentBlock();
        new llvm::BranchInst(ifTrue, ifFalse, value, env.currentBlock);
        env.currentBlock = NULL;

        env.setCurrentBlock(ifTrue);
        llvm::Value* tValue = new llvm::AllocaInst(llvm::IntegerType::get(1), "", env.entryBlock->getTerminator());
	new llvm::StoreInst(llvm::ConstantInt::getTrue(), tValue, false, env.currentBlock);
        new llvm::BranchInst(next, env.currentBlock);
        env.currentBlock = NULL;

        env.setCurrentBlock(ifFalse);
        llvm::Value* fValue = new llvm::AllocaInst(llvm::IntegerType::get(1), "", env.entryBlock->getTerminator());
	new llvm::StoreInst(llvm::ConstantInt::getTrue(), fValue, false, env.currentBlock);

        env.setCurrentBlock(next);
        llvm::PHINode* phi = new llvm::PHINode(tValue->getType(), "", env.currentBlock);
	phi->addIncoming(tValue, ifTrue);
	phi->addIncoming(fValue, ifFalse);
        result = phi;
        break;
    }

    case BIN_COMMA:     // ,
	result = right;
        break;

    // gcc extensions
    case BIN_MINIMUM:   // <?
        cerr << toString(loc) << ": ";
        xunimp("<?");
        break;
    case BIN_MAXIMUM:   // >?
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
        cerr << toString(loc) << ": ";
        xunimp("==>");
        break;
    case BIN_EQUIVALENT:  // <==>
        cerr << toString(loc) << ": ";
        xunimp("<==>");
        break;
    case NUM_BINARYOPS:
        break;
    }

    return result;
}

llvm::Value *E_addrOf::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    llvm::Value * result = expr->cc2llvm(env, true);	// Get the lvalue.
    return result;
}

llvm::Value *E_deref::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    if (lvalue) {
        // The dereference will happen later.
	return ptr->cc2llvm(env);
    }

    env.checkCurrentBlock();
    llvm::Value* source = ptr->cc2llvm(env);
    return new llvm::LoadInst(source, "", false, env.currentBlock);	// RICH: Volatile
}

llvm::Value *E_cast::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    llvm::Value* result = expr->cc2llvm(env);
    env.makeCast(loc, expr->type, result, type);
    return result;
}

llvm::Value *E_stdConv::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    llvm::Value* result = expr->cc2llvm(env);
    env.makeCast(loc, expr->type, result, type);
    return result;
}

llvm::Value *E_cond::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
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
    llvm::BasicBlock* ifTrue = new llvm::BasicBlock("condTrue", env.function, env.returnBlock);
    llvm::BasicBlock* ifFalse = new llvm::BasicBlock("condFalse", env.function, env.returnBlock);
    llvm::BasicBlock* next = new llvm::BasicBlock("next", env.function, env.returnBlock);
    env.checkCurrentBlock();
    new llvm::BranchInst(ifTrue, ifFalse, value, env.currentBlock);
    env.currentBlock = NULL;

    env.setCurrentBlock(ifTrue);
    llvm::Value* trueValue = th->cc2llvm(env);
    if (env.currentBlock) {
        // Close the current block.
        new llvm::BranchInst(next, env.currentBlock);
        env.currentBlock = NULL;
    }

    env.setCurrentBlock(ifFalse);
    llvm::Value* falseValue = el->cc2llvm(env);

    env.setCurrentBlock(next);
    llvm::Value* result = NULL;
    if (trueValue->getType()->isFirstClassType() && falseValue->getType()->isFirstClassType()) {
        llvm::PHINode* phi = new llvm::PHINode(trueValue->getType(), "", env.currentBlock);
	phi->addIncoming(trueValue, ifTrue);
	phi->addIncoming(falseValue, ifFalse);
        result = phi;
    } else {
        cerr << toString(loc) << ": ";
        xunimp("?:");
    }
    return result;
}

llvm::Value *E_sizeofType::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("sizeof");
    return NULL;
}

llvm::Value *E_assign::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    llvm::Value* destination = target->cc2llvm(env, true);	// Evaluate the destination expression as an lvalue.
    llvm::Value* source = src->cc2llvm(env);			// Evaluate the source expression as an rvalue.

    env.checkCurrentBlock();
    if (op == BIN_ASSIGN) {
	// Assign is simple. Get it out of the way.
	env.makeCast(loc, src->type, source, target->type);
	new llvm::StoreInst(source, destination, false, env.currentBlock);	// RICH: isVolatile
        return source;
    }

    llvm::Value* temp = NULL;					// A place to store the temporary.
    llvm::Instruction::BinaryOps opcode;			// The opcode to use.

    // Get the value.
    temp = new llvm::LoadInst(destination, "", false, env.currentBlock);	// RICH: Volatile
    if (temp->getType()->getTypeID() == llvm::Type::PointerTyID) {
        // Handle pointer arithmetic.
        cerr << toString(loc) << ": ";
        xunimp("pointer assignment operator");
        return NULL;
    }

    // Make sure the values match in type.
    CC2LLVMEnv::OperatorClass c = env.makeCast(loc, target->type, temp, src->type, &source);

    switch (op)
    {
    case BIN_MULT:      // *=
	opcode = llvm::Instruction::Mul;
        break;

    case BIN_DIV:       // /=
	if (c == env.OC_SINT) {
	    opcode = llvm::Instruction::SDiv;
	} else if (c == env.OC_UINT) {
	    opcode = llvm::Instruction::UDiv;
	} else if (c == env.OC_FLOAT) {
	    opcode = llvm::Instruction::FDiv;
	} else {
            cerr << toString(loc) << ": ";
            xunimp("/=");
	}
        break;

    case BIN_MOD:       // %=
	if (c == env.OC_SINT) {
	    opcode = llvm::Instruction::SRem;
	} else if (c == env.OC_UINT) {
	    opcode = llvm::Instruction::URem;
	} else if (c == env.OC_FLOAT) {
	    opcode = llvm::Instruction::FRem;
	} else {
            cerr << toString(loc) << ": ";
            xunimp("%=");
	}
        break;

    case BIN_PLUS:      // +=
	opcode = llvm::Instruction::Add;
        break;

    case BIN_MINUS:     // -=
	opcode = llvm::Instruction::Sub;
        break;

    case BIN_LSHIFT:    // <<=
	if (c == env.OC_SINT || c == env.OC_UINT) {
	    opcode = llvm::Instruction::Shl;
	} else {
            cerr << toString(loc) << ": ";
            xunimp("<<=");
	}
        break;

    case BIN_RSHIFT:    // >>=
	if (c == env.OC_SINT) {
	    opcode = llvm::Instruction::AShr;
	} else if (c == env.OC_UINT) {
	    opcode = llvm::Instruction::LShr;
	} else {
            cerr << toString(loc) << ": ";
            xunimp(">>=");
	}
        break;

    case BIN_BITAND:    // &=
	if (c == env.OC_SINT || c == env.OC_UINT) {
	    opcode = llvm::Instruction::And;
	} else {
            cerr << toString(loc) << ": ";
            xunimp("&=");
	}
        break;

    case BIN_BITXOR:    // ^=
	if (c == env.OC_SINT || c == env.OC_UINT) {
	    opcode = llvm::Instruction::Xor;
	} else {
            cerr << toString(loc) << ": ";
            xunimp("^=");
	}
        break;

    case BIN_BITOR:     // |=
	if (c == env.OC_SINT || c == env.OC_UINT) {
	    opcode = llvm::Instruction::Or;
	} else {
            cerr << toString(loc) << ": ";
            xunimp("<<=");
	}
        break;
		     
    default:
        cerr << toString(loc) << ": ";
        xunimp("assignment operator");
        break;
    }

    temp = llvm::BinaryOperator::create(opcode, temp, source, "", env.currentBlock);
    new llvm::StoreInst(temp, destination, false, env.currentBlock);	// RICH: Volatile
    return source;
}

llvm::Value *E_new::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("new");
    return NULL;
}

llvm::Value *E_delete::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("delete");
    return NULL;
}

llvm::Value *E_throw::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("throw");
    return NULL;
}

llvm::Value *E_keywordCast::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("keyword cast");
    return NULL;
}

llvm::Value *E_typeidExpr::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("typeid");
    return NULL;
}

llvm::Value *E_typeidType::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("typeid");
    return NULL;
}

llvm::Value *E_grouping::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("grouping");
    return NULL;
}

llvm::Value *E_arrow::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("->");
    return NULL;
}


llvm::Value *E_addrOfLabel::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("label address");
    return NULL;
}

llvm::Value *E_gnuCond::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("gnu conditional");
    return NULL;
}

llvm::Value *E_alignofExpr::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("alignof");
    return NULL;
}

llvm::Value *E_alignofType::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("alignof");
    return NULL;
}

llvm::Value *E___builtin_va_arg::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    llvm::Value* value = expr->cc2llvm(env);
    const llvm::Type* type = env.makeTypeSpecifier(loc, atype->getType());
    env.checkCurrentBlock();
    return new llvm::VAArgInst(value, type, "", env.currentBlock);
}

llvm::Value *E___builtin_constant_p::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("__builtin_constant_p");
    return NULL;
}

llvm::Value *E_compoundLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("compound literal");
    return NULL;
}

llvm::Value *E_statement::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    cerr << toString(loc) << ": ";
    xunimp("statement expression");
    return NULL;
}



// ------------------ FullExpression -----------------
llvm::Value *FullExpression::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
  return expr->cc2llvm(env, lvalue);
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
    llvm::LLVMFoldingBuilder builder;
    CC2LLVMEnv env(str, name, input, targetData, targetTriple, builder);
    return env.doit();
}

// EOF
