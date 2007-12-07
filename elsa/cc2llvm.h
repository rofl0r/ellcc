// cc2llvm.h
// Given a C++ TranslationUnit, produce a C TranslationUnit.
// That is, compile C++ down into C.

// See cc2llvm.txt for some design notes.

#ifndef CC2LLVM_H
#define CC2LLVM_H

// LLVM
namespace llvm {
    class Module;
    class Type;
    class Function;
    class BasicBlock;
    class Value;
};

// Elsa
#include "cc_ast.h"          // C++ AST

// smbase
#include "ptrmap.h"          // PtrMap
#include "strtable.h"        // StringTable

// System
#include <vector>

// Main translator entry point.  Output tree is completely disjoint
// from the input tree, with the (optional) exception of the string
// table.  Also, the output tree is just (unambiguous) syntax; it is
// not decorated with types.  In contrast, the input tree must already
// be typechecked *and* elaborated (with cc_elaborate.{h,cc}, using
// all elaboration mechanisms).
llvm::Module* cc_to_llvm(string name, StringTable &str, TranslationUnit const &input);


// Translation environment.  Among other things, holds the output
// TranslationUnit while it is being built.
class CC2LLVMEnv {
public:      // data
  // StringTable for use by the created AST nodes.  Can be the
  // same as the input tree's table, but can also be different.
  StringTable &str;

public:      // funcs
  CC2LLVMEnv(StringTable &str, string name, const TranslationUnit& input);
  ~CC2LLVMEnv();

  // Create a new TypeSpecifier that names type 't'.
  const llvm::Type* makeTypeSpecifier(Type *t);
  const llvm::Type* makeAtomicTypeSpecifier(AtomicType *t);

  /** Make sure the current block has been opened.
   */
  void checkCurrentBlock();

  // Get a name for a variable.  Employs the name mangler.
  StringRef getVariableName(Variable const *v);

  // Make a PQ_name for a variable.
  PQ_name *makeName(Variable const *v);

  // Create a PQ_name.  Just encapsulates an allocation.
  PQ_name *makePQ_name(StringRef name);

  // Create a list of parameters suitable for inclusion in a D_func,
  // based on 'ft'.
  void makeParameterTypes(FunctionType *ft, std::vector<const llvm::Type*>& args);

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
  /** Map AST variables to LLVM variables.
   */
  PtrMap<Variable, llvm::Value> variables;
  /** Map labels to LLVM blocks.
   */
  PtrMap<const char, llvm::BasicBlock> labels;

};


// TODO: Add a visitor to check that a TranslationUnit only uses
// things that are legal in C.


#endif // CC2LLVM_H
