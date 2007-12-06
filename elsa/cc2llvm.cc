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
    loopBlock(NULL),
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
    if (t->isCVAtomicType()) {
        // no need for a declarator
        CVAtomicType *at = t->asCVAtomicType();
        return makeAtomicTypeSpecifier(at->atomic);
    } else if (t->isPointerType()) {
        xunimp("unhandled pointer type");
    } else {
        // Need a declarator.
        xunimp("unhandled makeTypeSpecifier");
        return NULL;
    }
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
            type = llvm::IntegerType::get(simpleTypeReprSize(id) * 8);	// RICH: Bits per byte.
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
        llvm::LoadInst* rv = new llvm::LoadInst(env.returnValue, "", false, env.returnBlock);
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
            xunimp("unhandled function declaration");
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
    llvm::Value* zero = llvm::Constant::getNullValue(value->getType());
    value = new llvm::ICmpInst(llvm::ICmpInst::ICMP_NE, value, zero, "", env.currentBlock);
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
    llvm::BasicBlock* oldLoopBlock = env.loopBlock;
    llvm::BasicBlock* oldNextBlock = env.nextBlock;

    env.loopBlock = new llvm::BasicBlock("loop", env.function, env.returnBlock);
    llvm::BasicBlock* bodyBlock = new llvm::BasicBlock("body", env.function, env.returnBlock);
    env.nextBlock = new llvm::BasicBlock("next", env.function, env.returnBlock);
    if (env.currentBlock) {
        // Close the current block.
        new llvm::BranchInst(env.loopBlock, env.currentBlock);
    }

    // Set the current block.
    env.currentBlock = env.loopBlock;

    // Generate the test.
    const CN_expr* condition = cond->asCN_exprC();
    llvm::Value* value = condition->expr->cc2llvm(env);
    llvm::Value* zero = llvm::Constant::getNullValue(value->getType());
    value = new llvm::ICmpInst(llvm::ICmpInst::ICMP_NE, value, zero, "", env.currentBlock);
    new llvm::BranchInst(bodyBlock, env.nextBlock, value, env.currentBlock);

    env.currentBlock = bodyBlock;
    body->cc2llvm(env);
    new llvm::BranchInst(env.loopBlock, env.currentBlock);
    env.currentBlock = env.nextBlock;

    // Restore the old loop context.
    env.loopBlock = oldLoopBlock;
    env.nextBlock = oldNextBlock;
}

void S_doWhile::cc2llvm(CC2LLVMEnv &env) const
{
    // Save the old loop context.
    llvm::BasicBlock* oldLoopBlock = env.loopBlock;
    llvm::BasicBlock* oldNextBlock = env.nextBlock;

    llvm::BasicBlock* bodyBlock = new llvm::BasicBlock("body", env.function, env.returnBlock);
    env.loopBlock = new llvm::BasicBlock("loop", env.function, env.returnBlock);
    env.nextBlock = new llvm::BasicBlock("next", env.function, env.returnBlock);
    if (env.currentBlock) {
        // Close the current block.
        new llvm::BranchInst(env.loopBlock, env.currentBlock);
    }

    // Set the current block.
    env.currentBlock = bodyBlock;
    body->cc2llvm(env);
    new llvm::BranchInst(env.loopBlock, env.currentBlock);

    // Generate the test.
    env.currentBlock = env.loopBlock;
    llvm::Value* value = expr->cc2llvm(env);
    llvm::Value* zero = llvm::Constant::getNullValue(value->getType());
    value = new llvm::ICmpInst(llvm::ICmpInst::ICMP_NE, value, zero, "", env.currentBlock);
    new llvm::BranchInst(bodyBlock, env.nextBlock, value, env.currentBlock);

    env.currentBlock = env.nextBlock;

    // Restore the old loop context.
    env.loopBlock = oldLoopBlock;
    env.nextBlock = oldNextBlock;
}

void S_for::cc2llvm(CC2LLVMEnv &env) const { xunimp("for"); }

void S_break::cc2llvm(CC2LLVMEnv &env) const
{
    xassert(env.nextBlock && "break not in a loop or switch");
    new llvm::BranchInst(env.nextBlock, env.currentBlock);
    env.currentBlock = NULL;
}

void S_continue::cc2llvm(CC2LLVMEnv &env) const
{
    xassert(env.loopBlock && "continue not in a loop");
    new llvm::BranchInst(env.loopBlock, env.currentBlock);
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

llvm::Value *E_boolLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

llvm::Value *E_intLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    return llvm::ConstantInt::get(llvm::APInt(32, text, 10));	// RICH: Size, base.
}

llvm::Value *E_floatLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_stringLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_charLit::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_this::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

/** Get a variable used in an expression.
 */
llvm::Value *E_variable::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    // The variable will have been previously seen in a declaration.
    llvm::Value* value = env.variables.get(var);
    xassert(value && "An undeclared variable has been referenced");
    if (!lvalue) {
        // Return the value this represents.
        value = new llvm::LoadInst(value, "", false, env.currentBlock);		// RICH: isVolatile
    }
    return value;
}

llvm::Value *E_funCall::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    env.checkCurrentBlock();
    llvm::Value* function = func->cc2llvm(env, true);
    return new llvm::CallInst(function, "", env.currentBlock);
}

llvm::Value *E_constructor::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_fieldAcc::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_sizeof::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_unary::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_effect::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }

llvm::Value *E_binary::cc2llvm(CC2LLVMEnv &env, bool lvalue) const
{
    env.checkCurrentBlock();
    llvm::Value* left = e1->cc2llvm(env);
    llvm::Value* right = e2->cc2llvm(env);
    llvm::Value* result = NULL;
    switch (op)
    {
    case BIN_PLUS:
	result = llvm::BinaryOperator::create(llvm::Instruction::Add, left, right, "", env.currentBlock);
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
    default:
        xunimp("");
        break;
    }

    return result;
}

llvm::Value *E_addrOf::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
llvm::Value *E_deref::cc2llvm(CC2LLVMEnv &env, bool lvalue) const { xunimp(""); return NULL; }
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
