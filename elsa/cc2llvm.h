// cc2llvm.h
// Given a C++ TranslationUnit, produce LLVM code.

#ifndef CC2LLVM_H
#define CC2LLVM_H

// LLVM
namespace llvm {
    class Module;
    class Type;
    class Function;
    class BasicBlock;
    class Value;
    class SwitchInst;
    class LLVMFoldingBuilder;
};
#include <llvm/Target/TargetData.h>

// Elsa
#include "cc_ast.h"          // C++ AST

// smbase
#include "ptrmap.h"          // PtrMap
#include "strtable.h"        // StringTable

// System
#include <vector>

/** The main translator entry point.
 */
llvm::Module* cc_to_llvm(string name, StringTable &str, TranslationUnit const &input, string targetData, string targetTriple);


/** The translation environment.
 * Among other things, the environment holds the output LLVM module.
 * while it is being built.
 */
class CC2LLVMEnv {
public:      // data
    /** StringTable for storing generated strings.
     */ 
    StringTable &str;

public:      // funcs
    /** Construct an LLVM convertor.
     */
    CC2LLVMEnv(StringTable &str, string name, const TranslationUnit& input,
               string targetData, string targetTriple, llvm::LLVMFoldingBuilder& builder);
    /** Destruct an LLVM convertor.
     */
    ~CC2LLVMEnv();

    /** Information aboue the target.
     */
    llvm::TargetData targetData;

    /** Convert an AST type specifier into an LLVM type specifier.
     */
    const llvm::Type* makeTypeSpecifier(SourceLoc loc, Type *t);

    /** Convert an AST atomoc type specifier into an LLVM atomoc type specifier.
     */
    const llvm::Type* makeAtomicTypeSpecifier(SourceLoc loc, AtomicType *t);

    /** Make sure the current basic block has been opened.
     */
    void checkCurrentBlock();
    /** Set the current block.
     */
    void setCurrentBlock(llvm::BasicBlock* block);
    /** Check a condition in preparation for a branch.
     */
    llvm::Value* checkCondition(SourceLoc loc, llvm::Value* value);
    /** Check a condition in preparation for a branch.
     */
    llvm::Value* checkCondition(Expression* cond);

     /** Get a name for a variable.
      * Employs the name mangler.
      */
    StringRef getVariableName(Variable const *v);

    /** Make a PQ_name for a variable.
     */
    PQ_name *makeName(Variable const *v);

    /** Create a PQ_name.  Just encapsulates an allocation.
     */
    PQ_name *makePQ_name(StringRef name);

    /** Create a list of parameters suitable for inclusion in a llvm::FunctionType.
     */
    void makeParameterTypes(FunctionType *ft, std::vector<const llvm::Type*>& args);

    /** Operator classification.
     */
    enum OperatorClass {
      /** Signed integer.
       */
      OC_SINT,
      /** Unsigned integer.
       */
      OC_UINT,
      /** A pointer.
       */
      OC_POINTER,
      /** Floating point.
       */
      OC_FLOAT,
      /** Everything else.
       */
      OC_OTHER
    };

    /** Make two operand the same type or cast the first operand to the second type.
     * @return The TypeID of the result.
     */
    OperatorClass makeCast(SourceLoc loc, Type* leftType,
        llvm::Value*& leftValue, Type* rightType, llvm::Value** rightValue = NULL);
    /** Create a value from an initializer.
     * @return The LLVM value representing the initializer.
     */
    llvm::Value* initializer(const Initializer* init, Type* type);

    /** Perform the AST to LLVM lowering.
     * @return The LLVM module.
     */
    llvm::Module* doit();

    /** The input AST.
     */
    const TranslationUnit& input;
    /** The LLVM module created.
     */
    llvm::Module* mod;
    /** The current function.
     */
    llvm::Function* function;
    /** The AST of the currrent function.
     */
    const Function* functionAST;
    /** The current function entry block.
     */
    llvm::BasicBlock* entryBlock;
    /** The current function return block.
     */
    llvm::BasicBlock* returnBlock;
    /** The return value.
     */
    llvm::Value* returnValue;
    /** The current block being processed.
     */
    llvm::BasicBlock* currentBlock;
    /** The continuation point of the current loop.
     */
    llvm::BasicBlock* continueBlock;
    /** The block following current loop or switch.
     */
    llvm::BasicBlock* nextBlock;
    /** The currently active switch instruction.
     */
    llvm::SwitchInst* switchInst;
    /** The type of the currently active switch expression.
     */
    Type* switchType;
    /** Map AST variables to LLVM variables.
     */
    PtrMap<const Variable, llvm::Value> variables;
    /** Map AST compound types to LLVM compound types.
     */
    PtrMap<CompoundType, const llvm::Type> compounds;
    /** Map AST types to LLVM types.
     */
    PtrMap<Type, const llvm::Type> types;
    /** Map labels to LLVM blocks.
     */
    PtrMap<const char, llvm::BasicBlock> labels;
    /** The LLVM Builder.
     */
    llvm::LLVMFoldingBuilder& builder;
};

#endif // CC2LLVM_H
