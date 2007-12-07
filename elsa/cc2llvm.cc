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
#include <llvm/Analysis/Verifier.h>

#define BITS_PER_BYTE	8	// RICH: Temporary.

// -------------------- CC2LLVMEnv ---------------------
CC2LLVMEnv::CC2LLVMEnv(StringTable &s, string name, const TranslationUnit& input)
  : str(s),
    input(input),
    mod(new llvm::Module(name.c_str())),
    function(NULL),
    entryBlock(NULL),
    returnBlock(NULL),
    returnValue(NULL),
    currentBlock(NULL),
    continueBlock(NULL),
    nextBlock(NULL)
{ }

CC2LLVMEnv::~CC2LLVMEnv()
{ }

/** Make sure the current block has been opened.
 */
void CC2LLVMEnv::checkCurrentBlock()
{
    if (currentBlock == NULL) {
        // No block is current, make one.
        currentBlock = new llvm::BasicBlock("", function, returnBlock);
    }
}

const llvm::Type* CC2LLVMEnv::makeTypeSpecifier(Type *t)
{
    const llvm::Type* type = NULL;

    switch (t->getTag())
    {
    case Type::T_ATOMIC: {
        // No need for a declarator.
	// RICH: Is const, volatile?
        CVAtomicType *at = t->asCVAtomicType();
        type =  makeAtomicTypeSpecifier(at->atomic);
        break;
    }
    case Type::T_POINTER: {
	// RICH: Is const, volatile?
        PointerType *pt = t->asPointerType();
        type =  llvm::PointerType::get(makeTypeSpecifier(pt->atType));
        break;
    }
    case Type::T_REFERENCE: {
        xunimp("unhandled reference type");
	// The following isn't right. Experimenting.
        ReferenceType *rt = t->asReferenceType();
        type =  makeTypeSpecifier(rt->atType);
        break;
    }
    case Type::T_FUNCTION: {
        FunctionType *ft = t->asFunctionType();
        const llvm::Type* returnType = makeTypeSpecifier(ft->retType);
        std::vector<const llvm::Type*>args;
        makeParameterTypes(ft, args);
        type = llvm::FunctionType::get(returnType, args, ft->acceptsVarargs(), NULL);	// RICH: Parameter attributes.
        break;
    }
    case Type::T_ARRAY: {
        ArrayType *at = t->asArrayType();
	int size = at->getSize();
	if (size == ArrayType::NO_SIZE) {
	    size = 0;
	} else if (size == ArrayType::DYN_SIZE) {
            xunimp("unhandled dynamic array type");
	}
        type = llvm::ArrayType::get(makeTypeSpecifier(at->eltType), size);
        break;
    }
    case Type::T_POINTERTOMEMBER:
        xunimp("unhandled pointer to member type");
        break;
    case Type::T_DEPENDENTSIZEDARRAY:
        xunimp("unhandled dependent sized array type");
        break;
    case Type::T_LAST_TYPE_TAG:	// Quell warnings.
        break;
    }

    return type;
}

const llvm::Type* CC2LLVMEnv::makeAtomicTypeSpecifier(AtomicType *at)
{
    const llvm::Type* type = NULL;
    switch (at->getTag()) {
    case AtomicType::T_SIMPLE: {
        SimpleType *st = at->asSimpleType();
        SimpleTypeId id = st->type;
        if (id == ST_BOOL) {
            type = llvm::IntegerType::get(1);
        } else if (id == ST_VOID) {
            type = llvm::Type::VoidTy;
        } else if (::isIntegerType(id)) {
	    // Define an integer type.
            type = llvm::IntegerType::get(simpleTypeReprSize(id) * BITS_PER_BYTE);
        } else {
            xunimp("makeAtomicTypeSpecifier for simple type");
        }
      
        break;
    }

    case AtomicType::T_COMPOUND: {
        xunimp("makeAtomicTypeSpecifier for compound");
        break;
#if RICH
        CompoundType *ct = at->asCompoundType();
        return new TS_elaborated(
          SL_GENERATED,
          ct->keyword == CompoundType::K_UNION? TI_UNION : TI_STRUCT,
          makePQ_name(getCompoundTypeName(ct))
        );
#endif
    }

    case AtomicType::T_ENUM:
        xunimp("makeAtomicTypeSpecifier for enum");
        break;
      
    case AtomicType::T_TYPEVAR:
    case AtomicType::T_PSEUDOINSTANTIATION:
    case AtomicType::T_DEPENDENTQTYPE:
    case AtomicType::T_TEMPLATETYPEVAR:
        xfailure("makeAtomicTypeSpecifier: template-related type");
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
    
      args.push_back(makeTypeSpecifier(param->type));
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
        xunimp("cc2llvmTopForm");
    }
    
    ASTENDCASE
    }
}

void Function::cc2llvm(CC2LLVMEnv &env) const
{
    if (inits->isNotEmpty()) {
        xunimp("member initializers");
    }

    if (handlers->isNotEmpty()) {
        xunimp("exception handlers");
    }

    const llvm::Type* returnType = env.makeTypeSpecifier(funcType->retType);
    std::vector<const llvm::Type*>args;
    env.makeParameterTypes(funcType, args);
    llvm::FunctionType* ft = llvm::FunctionType::get(returnType, args, funcType->acceptsVarargs(), NULL);	// RICH: Parameter attributes.
    env.function = new llvm::Function(ft, llvm::GlobalValue::ExternalLinkage,	// RICH: Linkage.
        nameAndParams->var->name, env.mod);
    env.function->setCallingConv(llvm::CallingConv::C); // RICH: Calling convention.
    env.variables.add(nameAndParams->var, env.function);
    env.entryBlock = new llvm::BasicBlock("entry", env.function, NULL);

    // Set the initial current block.
    env.currentBlock = env.entryBlock;

    // Set up the return block.
    env.returnBlock = new llvm::BasicBlock("return", env.function, 0);
    // Set up the return value.
    if (returnType == llvm::Type::VoidTy) {
        // A void function.
        env.returnValue = NULL;
        new llvm::ReturnInst(env.returnBlock);
    } else {
        // Create the return value holder.
        env.returnValue = new llvm::AllocaInst(returnType, "retval", env.entryBlock);

        // RICH: This should happen in main() only: Default return value.
        llvm::Constant* nullInt = llvm::Constant::getNullValue(returnType);
        new llvm::StoreInst(nullInt, env.returnValue, false, env.entryBlock);	// RICH: main() only.

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
      new llvm::BranchInst(env.returnBlock, env.currentBlock);
      env.currentBlock = NULL;
    }
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
            xunimp("declaration with ctorStatement or dtorStatement");
        }

        // Initializer.        
        llvm::Value* value = NULL;
        if (declarator->init) {
            if (declarator->init->isIN_expr()) {
                Expression const *e = declarator->init->asIN_exprC()->e;

                // RICH: TRAP_SIDE_EFFECTS_STMT(temp);
                value = e->cc2llvm(env);

	#if RICH
                if (temp.stmts.isNotEmpty()) {
                    xunimp("initializer expression with side effects");
                }
        #endif
            } else {
                xunimp("unhandled kind of initializer");
            }
        }

        // Create the full generated declaration.
        const llvm::Type* type = env.makeTypeSpecifier(var->type);
        if (var->type->getTag() == Type::T_FUNCTION) {
            llvm::Function* gf = new llvm::Function((llvm::FunctionType*)type, 
                (var->flags & DF_STATIC) ? llvm::GlobalValue::InternalLinkage : llvm::GlobalValue::ExternalLinkage,
	        env.makeName(var)->name, env.mod);
            gf->setCallingConv(llvm::CallingConv::C); // RICH: Calling convention.
            env.variables.add(var, gf);
        } else if (var->type->getTag() == Type::T_DEPENDENTSIZEDARRAY) {
            xunimp("unhandled dependent sized array declaration");
        } else if (var->type->getTag() == Type::T_LAST_TYPE_TAG) {
            xunimp("unhandled last type tag declaration");
        } else if (var->flags & (DF_STATIC|DF_GLOBAL)) {
	    // A global variable.
            if (value == NULL) {
                value = llvm::Constant::getNullValue(type);
            }
            llvm::GlobalVariable* gv = new llvm::GlobalVariable(type, false,	// RICH: isConstant
                (var->flags & DF_STATIC) ? llvm::GlobalValue::InternalLinkage : llvm::GlobalValue::ExternalLinkage,
	        (llvm::Constant*)value, env.makeName(var)->name, env.mod);
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

	    if (value) {
	        env.checkCurrentBlock();
	        new llvm::StoreInst(value, lv, false, env.currentBlock);	// RICH: isVolatile.
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

    if (env.currentBlock) {
        // Close the current block.
        new llvm::BranchInst(block, env.currentBlock);
    }

    env.currentBlock = block;
    s->cc2llvm(env);
}

void S_case::cc2llvm(CC2LLVMEnv &env) const
{
    xunimp("unhandled case statement");
#if RICH
    env.addStatement(
      new S_case(SL_GENERATED, expr->cc2llvmNoSideEffects(env), makeSkip()));
#endif
    s->cc2llvm(env);
}

void S_default::cc2llvm(CC2LLVMEnv &env) const
{
    xunimp("unhandled default statement");
#if RICH
    env.addStatement(
      new S_default(SL_GENERATED, makeSkip()));
#endif
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
    env.checkCurrentBlock();
    const CN_expr* condition = cond->asCN_exprC();
    llvm::Value* value = condition->expr->cc2llvm(env);
    if (value->getType() != llvm::Type::Int1Ty)
    {
        // Not a boolean, check for non-zero.
        llvm::Value* zero = llvm::Constant::getNullValue(value->getType());
        value = new llvm::ICmpInst(llvm::ICmpInst::ICMP_NE, value, zero, "", env.currentBlock);
    }
    llvm::BasicBlock* ifTrue = new llvm::BasicBlock("ifTrue", env.function, env.returnBlock);
    llvm::BasicBlock* ifFalse = new llvm::BasicBlock("ifFalse", env.function, env.returnBlock);
    llvm::BasicBlock* next = new llvm::BasicBlock("next", env.function, env.returnBlock);

    new llvm::BranchInst(ifTrue, ifFalse, value, env.currentBlock);

    env.currentBlock = ifTrue;
    thenBranch->cc2llvm(env);
    if (env.currentBlock) {
        // Close the current block.
        new llvm::BranchInst(next, env.currentBlock);
        env.currentBlock = NULL;
    }

    env.currentBlock = ifFalse;
    elseBranch->cc2llvm(env);
    if (env.currentBlock) {
        // Close the current block.
        new llvm::BranchInst(next, env.currentBlock);
        env.currentBlock = NULL;
    }

    env.currentBlock = next;
}

void S_switch::cc2llvm(CC2LLVMEnv &env) const { xunimp("switch"); }

void S_while::cc2llvm(CC2LLVMEnv &env) const
{
    // Save the old loop context.
    llvm::BasicBlock* oldContinueBlock = env.continueBlock;
    llvm::BasicBlock* oldNextBlock = env.nextBlock;

    env.continueBlock = new llvm::BasicBlock("continue", env.function, env.returnBlock);
    llvm::BasicBlock* bodyBlock = new llvm::BasicBlock("body", env.function, env.returnBlock);
    env.nextBlock = new llvm::BasicBlock("next", env.function, env.returnBlock);
    if (env.currentBlock) {
        // Close the current block.
        new llvm::BranchInst(env.continueBlock, env.currentBlock);
    }

    // Set the current block.
    env.currentBlock = env.continueBlock;

    // Generate the test.
    const CN_expr* condition = cond->asCN_exprC();
    llvm::Value* value = condition->expr->cc2llvm(env);
    if (value->getType() != llvm::Type::Int1Ty)
    {
        // Not a boolean, check for non-zero.
        llvm::Value* zero = llvm::Constant::getNullValue(value->getType());
        value = new llvm::ICmpInst(llvm::ICmpInst::ICMP_NE, value, zero, "", env.currentBlock);
    }
    new llvm::BranchInst(bodyBlock, env.nextBlock, value, env.currentBlock);

    env.currentBlock = bodyBlock;
    body->cc2llvm(env);
    new llvm::BranchInst(env.continueBlock, env.currentBlock);
    env.currentBlock = env.nextBlock;

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
        new llvm::BranchInst(env.continueBlock, env.currentBlock);
    }

    // Set the current block.
    env.currentBlock = bodyBlock;
    body->cc2llvm(env);
    new llvm::BranchInst(env.continueBlock, env.currentBlock);

    // Generate the test.
    env.currentBlock = env.continueBlock;
    llvm::Value* value = expr->cc2llvm(env);
    if (value->getType() != llvm::Type::Int1Ty)
    {
        // Not a boolean, check for non-zero.
        llvm::Value* zero = llvm::Constant::getNullValue(value->getType());
        value = new llvm::ICmpInst(llvm::ICmpInst::ICMP_NE, value, zero, "", env.currentBlock);
    }
    new llvm::BranchInst(bodyBlock, env.nextBlock, value, env.currentBlock);

    env.currentBlock = env.nextBlock;

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
    new llvm::BranchInst(testBlock, env.currentBlock);

    // Set the current block.
    env.currentBlock = testBlock;

    // Generate the test.
    const CN_expr* condition = cond->asCN_exprC();
    llvm::Value* value = condition->expr->cc2llvm(env);
    if (value->getType() != llvm::Type::Int1Ty)
    {
        // Not a boolean, check for non-zero.
        llvm::Value* zero = llvm::Constant::getNullValue(value->getType());
        value = new llvm::ICmpInst(llvm::ICmpInst::ICMP_NE, value, zero, "", env.currentBlock);
    }
    new llvm::BranchInst(bodyBlock, env.nextBlock, value, env.currentBlock);

    env.currentBlock = bodyBlock;
    body->cc2llvm(env);
    new llvm::BranchInst(env.continueBlock, env.currentBlock);

    // Handle the continue block.
    env.currentBlock = env.continueBlock;
    after->cc2llvm(env);
    new llvm::BranchInst(testBlock, env.currentBlock);

    env.currentBlock = env.nextBlock;

    // Restore the old loop context.
    env.continueBlock = oldContinueBlock;
    env.nextBlock = oldNextBlock;
}

void S_break::cc2llvm(CC2LLVMEnv &env) const
{
    xassert(env.nextBlock && "break not in a loop or switch");
    new llvm::BranchInst(env.nextBlock, env.currentBlock);
    env.currentBlock = NULL;
}

void S_continue::cc2llvm(CC2LLVMEnv &env) const
{
    xassert(env.continueBlock && "continue not in a loop");
    new llvm::BranchInst(env.continueBlock, env.currentBlock);
    env.currentBlock = NULL;
}

void S_return::cc2llvm(CC2LLVMEnv &env) const
{
    env.checkCurrentBlock();
    if (expr) {
        // A return value is specified.
        xassert(env.returnValue && "return a value in a function returning void");
        llvm::Value* value = expr->cc2llvm(env);
        new llvm::StoreInst(value, env.returnValue, false, env.currentBlock);	// RICH: isVolatile
    } else {
        xassert(env.returnValue == NULL && "no return value in a function not returning void");
    }

    new llvm::BranchInst(env.returnBlock, env.currentBlock);
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
    new llvm::BranchInst(block, env.currentBlock);
    env.currentBlock = NULL;
}

void S_decl::cc2llvm(CC2LLVMEnv &env) const
{
    decl->cc2llvm(env);
}

void S_try::cc2llvm(CC2LLVMEnv &env) const { xunimp("try"); }

void S_asm::cc2llvm(CC2LLVMEnv &env) const
{
    xunimp("unhandled asm statement");
#if RICH
    env.addStatement(
      new S_asm(SL_GENERATED, text->cc2llvm(env)->asE_stringLit()));
#endif
}

void S_namespaceDecl::cc2llvm(CC2LLVMEnv &env) const 
{
    // should be able to just drop these
}


void S_computedGoto::cc2llvm(CC2LLVMEnv &env) const { xunimp("computed goto"); }
void S_rangeCase::cc2llvm(CC2LLVMEnv &env) const { xunimp("range case"); }
void S_function::cc2llvm(CC2LLVMEnv &env) const { xunimp("S_function"); }

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

llvm::Value *E_floatLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

llvm::Value *E_stringLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    // RICH: fullTextNQ is not quite right: has e.g. "\n" not replaced with \x10.
    llvm::Constant* c = llvm::ConstantArray::get(fullTextNQ, true);
    // RICH: Sizeof char.
    llvm::ArrayType* at = llvm::ArrayType::get(llvm::IntegerType::get(8), strlen(fullTextNQ) + 1);
    // RICH: Non-constant strings?
    llvm::GlobalVariable* gv = new llvm::GlobalVariable(at, true, llvm::GlobalValue::InternalLinkage, c, ".str", env.mod);

    // Get the address of the string as an open array.
    env.checkCurrentBlock();
    std::vector<llvm::Value*> indices;
    indices.push_back(llvm::Constant::getNullValue(llvm::IntegerType::get(32)));
    indices.push_back(llvm::Constant::getNullValue(llvm::IntegerType::get(32)));
    llvm::Instruction* address = new llvm::GetElementPtrInst(gv, indices.begin(), indices.end(), "", env.currentBlock);
    return address;
}

llvm::Value *E_charLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    return llvm::ConstantInt::get(llvm::APInt(type->reprSize() * BITS_PER_BYTE, c));
}

llvm::Value *E_this::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

/** Get a variable used in an expression.
 */
llvm::Value *E_variable::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    // The variable will have been previously seen in a declaration.
    llvm::Value* value = env.variables.get(var);
    xassert(value && "An undeclared variable has been referenced");
    xassert(value->getType()->getTypeID() == llvm::Type::PointerTyID && "expected pointer type");
    bool first = value->getType()->getContainedType(0)->isFirstClassType();
    if (!lvalue && first) {
        // Return the value this represents.
        value = new llvm::LoadInst(value, "", false, env.currentBlock);		// RICH: isVolatile
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
    return new llvm::CallInst(function, parameters.begin(), parameters.end(), "", env.currentBlock);
}

llvm::Value *E_constructor::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_fieldAcc::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_sizeof::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_unary::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

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
        result = new llvm::LoadInst(value, "", false, env.currentBlock);	// RICH: Volatile
        temp = new llvm::LoadInst(value, "", false, env.currentBlock);	// RICH: Volatile
    } else {
        temp = new llvm::LoadInst(value, "", false, env.currentBlock);	// RICH: Volatile
    }

    if (temp->getType()->getTypeID() == llvm::Type::PointerTyID) {
        // This is a pointer increment/decrement.
        xunimp("pointer ++,--");
    } else if (temp->getType()->isInteger()) {
	llvm::Instruction::BinaryOps opcode = op == EFF_POSTDEC || op == EFF_PREDEC ?
	    llvm::Instruction::Sub :
	    llvm::Instruction::Add;

        llvm::ConstantInt* one = llvm::ConstantInt::get(llvm::APInt(temp->getType()->getPrimitiveSizeInBits(), 1));
	temp = llvm::BinaryOperator::create(opcode, temp, one, "", env.currentBlock);
        new llvm::StoreInst(temp, value, false, env.currentBlock);	// RICH: Volatile
    } else {
        xunimp("++,-- for this operand type");
    }

    if (!isPostfix(op)) {
        result = temp;
    }
    return result;
}

llvm::Value *E_binary::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    env.checkCurrentBlock();
    llvm::Value* left = e1->cc2llvm(env);
    llvm::Value* right = e2->cc2llvm(env);
    llvm::Value* result = NULL;

    switch (op)
    {
    case BIN_PLUS:
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
	        // If the left size is a pointer and the left side is an integer, calculate the address.
		std::vector<llvm::Value*> index;
	        index.push_back(llvm::Constant::getNullValue(right->getType()));
	        index.push_back(right);
		result = new llvm::GetElementPtrInst(left, index.begin(), index.end(), "", env.currentBlock);
	    } else {
	        xunimp("addition of unlike types");
	    }
        } else {
	    result = llvm::BinaryOperator::create(llvm::Instruction::Add, left, right, "", env.currentBlock);
	}
        break;
    case BIN_EQUAL:
        result = new llvm::ICmpInst(llvm::ICmpInst::ICMP_EQ, left, right, "", env.currentBlock);
        break;
    case BIN_NOTEQUAL:
        result = new llvm::ICmpInst(llvm::ICmpInst::ICMP_NE, left, right, "", env.currentBlock);
        break;
    case BIN_LESS:	// RICH: Signed vs. unsigned.
        result = new llvm::ICmpInst(llvm::ICmpInst::ICMP_SLT, left, right, "", env.currentBlock);
        break;
    case BIN_LESSEQ:	// RICH: Signed vs. unsigned.
        result = new llvm::ICmpInst(llvm::ICmpInst::ICMP_SLE, left, right, "", env.currentBlock);
        break;
    default:
        xunimp("");
        break;
    }

    return result;
}

llvm::Value *E_addrOf::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

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

llvm::Value *E_cast::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

llvm::Value *E_cond::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

llvm::Value *E_sizeofType::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

llvm::Value *E_assign::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    env.checkCurrentBlock();
    llvm::Value* destination = target->cc2llvm(env, true);
    llvm::Value* source = src->cc2llvm(env);
    switch (op)
    {
    case BIN_ASSIGN:
	destination = new llvm::StoreInst(source, destination, false, env.currentBlock);	// RICH: isVolatile
        break;
    default:
        xunimp("");
        break;
    }

    return destination;
}

llvm::Value *E_new::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_delete::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_throw::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_keywordCast::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_typeidExpr::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_typeidType::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_grouping::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_arrow::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

llvm::Value *E_addrOfLabel::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_gnuCond::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_alignofExpr::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_alignofType::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E___builtin_va_arg::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E___builtin_constant_p::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_compoundLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_statement::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }


// ------------------ FullExpression -----------------
llvm::Value *FullExpression::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
  return expr->cc2llvm(env, lvalue);
}

// ------------------ Translate ----------------------
llvm::Module* CC2LLVMEnv::doit()
{
    // RICH: Data layout and target triple.
    mod->setDataLayout("e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-s0:0:64-f80:32:32");
    mod->setTargetTriple("i686-pc-linux-gnu");

    FOREACH_ASTLIST(TopForm, input.topForms, iter) {
        iter.data()->cc2llvm(*this);
    }

    // Verify the generated module.
    llvm::verifyModule(*mod, llvm::PrintMessageAction);
    // llvm::verifyModule(*mod, llvm::AbortProcessAction);

    return mod;
}

// ------------------- entry point -------------------
llvm::Module* cc_to_llvm(string name, StringTable &str, TranslationUnit const &input)
{
    CC2LLVMEnv env(str, name, input);
    return env.doit();
}

// EOF
