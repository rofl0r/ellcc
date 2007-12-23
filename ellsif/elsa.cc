// main.cc            see license.txt for copyright and terms of use
// The ellsif driver.

#include <iostream.h>     // cout
#include <stdlib.h>       // exit, getenv, abort
#include <fstream.h>      // ofstream

#include "elsa.h"         // This module.
#include "trace.h"        // traceAddSys
#include "syserr.h"       // xsyserror
#include "parssppt.h"     // ParseTreeAndTokens, treeMain
#include "srcloc.h"       // SourceLocManager
#include "ckheap.h"       // malloc_stats
#include "cc_env.h"       // Env
#include "cc_ast.h"       // C++ AST (r)
#include "cc_ast_aux.h"   // class LoweredASTVisitor
#include "cc_lang.h"      // CCLang
#include "parsetables.h"  // ParseTables
#include "cc_print.h"     // PrintEnv
#include "cc.gr.gen.h"    // CCParse
#include "nonport.h"      // getMilliseconds
#include "ptreenode.h"    // PTreeNode
#include "ptreeact.h"     // ParseTreeLexer, ParseTreeActions
#include "sprint.h"       // structurePrint
#include "strtokp.h"      // StrtokParse
#include "smregexp.h"     // regexpMatch
#include "cc_elaborate.h" // ElabVisitor
#include "integrity.h"    // IntegrityVisitor
#include "xml_file_writer.h" // XmlFileWriter
#include "xml_reader.h"   // xmlDanglingPointersAllowed
#include "xml_do_read.h"  // xmlDoRead()
#include "xml_type_writer.h" // XmlTypeWriter
#include "bpprint.h"      // bppTranslationUnit
#include "cc2c.h"         // cc_to_c
#include "cc2llvm.h"      // cc_to_llvm

// LLVM
#include <llvm/Module.h>
#include <llvm/Support/Timer.h>

/** The Elsa constructor.
 */
Elsa::Elsa(llvm::TimerGroup& timerGroup) :
     timerGroup(timerGroup),
    parseTimer("Parsing", timerGroup),
    typeCheckingTimer("Type checking", timerGroup),
    elaborationTimer("Elaboration", timerGroup),
    integrityCheckingTimer("Integrity checking", timerGroup),
    llvmGenerationTimer("LLVM generation", timerGroup)
{
    wantBpprint = false;
    wantBpprintAfterElab = false;
    // I think this is more noise than signal at this point
    xBase::logExceptions = false;

    traceAddFromEnvVar();           // Add environment variable traces.
    traceAddSys("progress");
    //traceAddSys("parse-tree");

    if (tracingSys("malloc_stats")) {
        malloc_stats();
    }

    doTime = false;
}

/** The Elsa destructor.
 */
Elsa::~Elsa()
{
}

/** Add trace systems.
 */
void Elsa::addTrace(const char* systems)
{
    traceAddMultiSys(systems);
}

/** Set up after command line parsing.
 */
void Elsa::setup(bool time)
{
  doTime = time;
  if (tracingSys("printAsML")) {
    Type::printAsML = true;
  }

  // FIX: dsw: couldn't we put dashes or something in here to break up
  // the word?
  if (tracingSys("nohashline")) {
    sourceLocManager->useHashLines = false;
  }

  if (tracingSys("tolerateHashlineErrors")) {
    sourceLocManager->tolerateHashlineErrors = true;
  }

  if (tracingSys("no-orig-offset")) {
    sourceLocManager->useOriginalOffset = false;
  }

  if (tracingSys("test_xfatal")) {
    xfatal("this is a test error message");
  }

  if (tracingSys("templateDebug")) {
    // predefined set of tracing flags I've been using while debugging
    // the new templates implementation
    traceAddSys("template");
    traceAddSys("error");
    traceAddSys("scope");
    traceAddSys("templateParams");
    traceAddSys("templateXfer");
    traceAddSys("prettyPrint");
    traceAddSys("xmlPrintAST");
    traceAddSys("topform");
  }

  if (tracingSys("only_works_on_32bit") &&
      sizeof(long) != 4) {
    // we are running a regression test, and the testcase is known to
    // fail due to dependence on architecture parameters, so skip it
    cout << "skipping test b/c this is not a 32-bit architecture\n";
    exit(0);
  }

  if (tracingSys("printTracers")) {
    cout << "tracing flags:\n\t";
    printTracers(std::cout, "\n\t");
    cout << endl;
  }
}

// nonempty if we want to run cc2c; value of "-" means stdout
static string cc2cOutputFname;

// little check: is it true that only global declarators
// ever have Declarator::type != Declarator::var->type?
// .. no, because it's true for class members too ..
// .. it's also true of arrays declared [] but then inited ..
// .. finally, it's true of parameters whose types get
//    normalized as per cppstd 8.3.5 para 3; I didn't include
//    that case below because there's no easy way to test for it ..
// Intended to be used with LoweredASTVisitor
class DeclTypeChecker : private ASTVisitor {
public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  int instances;

public:
  DeclTypeChecker()
    : loweredVisitor(this)
    , instances(0)
  {}
  virtual ~DeclTypeChecker() {}

  virtual bool visitDeclarator(Declarator *obj);
};

bool DeclTypeChecker::visitDeclarator(Declarator *obj)
{
  if (obj->type != obj->var->type &&
      !(obj->var->flags & (DF_GLOBAL | DF_MEMBER)) &&
      !obj->type->isArrayType()) {
    instances++;
    cout << toString(obj->var->loc) << ": " << obj->var->name
         << " has type != var->type, but is not global or member or array\n";
  }
  return true;
}

// this scans the AST for E_variables, and writes down the locations
// to which they resolved; it's helpful for writing tests of name and
// overload resolution
class NameChecker : public ASTVisitor {
public:
  // accumulates the results
  stringBuilder sb;

public:
  NameChecker() {}

  virtual bool visitExpression(Expression *obj)
  {
    Variable *v = NULL;
    if (obj->isE_variable()) {
      v = obj->asE_variable()->var;
    }
    else if (obj->isE_fieldAcc()) {
      v = obj->asE_fieldAcc()->field;
    }

    // this output format is designed to minimize the effect of
    // changes to unrelated details
    if (v
        && !streq("__testOverload", v->name)
        && !streq("dummy",          v->name)
        && !streq("__other",        v->name) // "__other": for inserted elaboration code
        && !streq("this",           v->name) // dsw: not sure why "this" is showing up
        && !streq("operator=",      v->name) // an implicitly defined member of every class
        && v->name[0]!='~'                   // don't print dtors
        ) {
      sb << " " << v->name << "=" << sourceLocManager->getLine(v->loc);
    }

    return true;
  }
};

static void handle_xBase(Env &env, xBase &x)
{
  // typically an assertion failure from the tchecker; catch it here
  // so we can print the errors, and something about the location
  env.errors.print(cout);
  cout << x << endl;
  cout << "Failure probably related to code near " << env.locStr() << endl;

  // print all the locations on the scope stack; this is sometimes
  // useful when the env.locStr refers to some template code that
  // was instantiated from somewhere else
  //
  // (unfortunately, env.instantiationLocStack isn't an option b/c
  // it will have been cleared by the automatic invocation of
  // destructors unwinding the stack...)
  cout << "current location stack:\n";
  cout << env.locationStackString();

  // I changed from using exit(4) here to using abort() because
  // that way the multitest.pl script can distinguish them; the
  // former is reserved for orderly exits, whereas signals (like
  // SIGABRT) mean that something went really wrong
  abort();
}


int Elsa::doit(Language language, const char* inputFname, const char* outputFname, llvm::Module*& mod)
{
    mod = NULL;
    SourceLocManager mgr;
    // String table for storing parse tree identifiers.
    StringTable strTable;
    

    // Parsing language options.
    CCLang lang;
    lang.GNU_Cplusplus();
    switch (language) {
    case GNUCXX:         // GNU C++
        break;
    case ANSICXX:        // ANSI C++
        lang.ANSI_Cplusplus();
        break;
    case ANSIC89:        // ANSI C89
        lang.ANSI_C89();
        break;
    case ANSIC99:        // ANSI C99
        lang.ANSI_C99();
        break;
    case GNUC:           // GNU C
        lang.GNU_C();
        break;
    case GNUC89:         // GNU C89
        lang.ANSI_C89();
        lang.GNU_C_extensions();
        break;
    case KANDRC:         // K&R C
         lang.GNU_KandR_C();
#ifndef KANDR_EXTENSION
        xfatal("gnu_kandr_c_lang option requires the K&R module (./configure -kandr=yes)");
#endif
        break;
    case GNUKANDRC:      // GNU K&R C
         lang.GNU2_KandR_C();
#ifndef KANDR_EXTENSION
        xfatal("gnu2_kandr_c_lang option requires the K&R module (./configure -kandr=yes)");
#endif
        break;
  }

  if (tracingSys("msvcBugs")) {
    lang.MSVC_bug_compatibility();
  }
  if (!tracingSys("nowarnings")) {
    lang.enableAllWarnings();
  }

  // dump out the lang settings if the user wants them
  if (tracingSys("printLang")) {
    cout << "language settings:\n";
    cout << lang.toString();
    cout << endl;
  }

  // --------------- parse --------------
  TranslationUnit *unit;

  // dsw: I needed this to persist past typechecking, so I moved it
  // out here.  Feel free to refactor.
  ArrayStack<Variable*> madeUpVariables;
  ArrayStack<Variable*> builtinVars;

  int parseWarnings = 0;
  if (tracingSys("parseXml")) {
    if (tracingSys("parseXml-no-danglingPointers")) {
      xmlDanglingPointersAllowed = false;
    }
    unit = xmlDoRead(strTable, inputFname);
    if (!unit) return 0;
  } else {
    if (doTime) {
        parseTimer.startTimer();
    }

    SemanticValue treeTop;
    ParseTreeAndTokens tree(lang, treeTop, strTable, inputFname);

    // grab the lexer so we can check it for errors (damn this
    // 'tree' thing is stupid..)
    Lexer *lexer = dynamic_cast<Lexer*>(tree.lexer);
    xassert(lexer);

    CCParse *parseContext = new CCParse(strTable, lang);
    tree.userAct = parseContext;

    traceProgress(2) << "building parse tables from internal data\n";
    ParseTables *tables = parseContext->makeTables();
    tree.tables = tables;

    maybeUseTrivialActions(tree);

    if (tracingSys("parseTree")) {
      // make some helpful aliases
      LexerInterface *underLexer = tree.lexer;
      UserActions *underAct = parseContext;

      // replace the lexer and parser with parse-tree-building versions
      tree.lexer = new ParseTreeLexer(underLexer, underAct);
      tree.userAct = new ParseTreeActions(underAct, tables);

      // 'underLexer' and 'tree.userAct' will be leaked.. oh well
    }

    if (doTime) {
        parseTimer.stopTimer();
    }

    if (!toplevelParse(tree, inputFname)) {
      return 2; // parse error
    }

    // check for parse errors detected by the context class
    if (parseContext->errors || lexer->errors) {
      return 2;
    }
    parseWarnings = lexer->warnings + parseContext->warnings;

    if (tracingSys("parseTree")) {
      // the 'treeTop' is actually a PTreeNode pointer; print the
      // tree and bail
      PTreeNode *ptn = (PTreeNode*)treeTop;
      ptn->printTree(cout, PTreeNode::PF_EXPAND);
      return 0;
    }

    // treeTop is a TranslationUnit pointer
    unit = (TranslationUnit*)treeTop;

    //unit->debugPrint(cout, 0);

    delete parseContext;
    delete tables;
  }

  checkHeap();

  // print abstract syntax tree
  if (tracingSys("printAST")) {
    unit->debugPrint(cout, 0);
  }

  //if (unit) {     // when "-tr trivialActions" it's NULL...
  //  cout << "ambiguous nodes: " << numAmbiguousNodes(unit) << endl;
  //}

  if (tracingSys("stopAfterParse")) {
    return 0;
  }


  // ---------------- typecheck -----------------
  BasicTypeFactory tfac;
  if (tracingSys("no-typecheck")) {
    cout << "no-typecheck" << endl;
  } else {
    if (doTime) {
        typeCheckingTimer.startTimer();
    }

    Env env(strTable, lang, tfac, madeUpVariables, builtinVars, unit);
    try {
      env.tcheckTranslationUnit(unit);
    }
    catch (XUnimp &x) {
      HANDLER();

      // relay to handler in main()
      cout << "in code near " << env.locStr() << ":\n";
      throw;
    }
    catch (x_assert &x) {
      HANDLER();

      if (env.errors.hasFromNonDisambErrors()) {
        if (tracingSys("expect_confused_bail")) {
          cout << "got the expected confused/bail\n";
          exit(0);
        }

        // The assertion failed only after we encountered and diagnosed
        // at least one real error in the input file.  Therefore, the
        // assertion might simply have been caused by a failure of the
        // error recovery code to fully restore all invariants (which is
        // often difficult).  So, we'll admit to being "confused", but
        // not to the presence of a bug in Elsa (which is what a failed
        // assertion otherwise nominally means).
        //
        // The error message is borrowed from gcc; I wouldn't be
        // surprised to discover they use a similar technique to decide
        // when to emit the same message.
        //
        // The reason I do not put the assertion failure message into
        // this one is I don't want it showing up in the output where
        // Delta might see it.  If I am intending to minimize an assertion
        // failure, it's no good if Delta introduces an error.
        env.error("confused by earlier errors, bailing out");
        env.errors.print(cout);
        return 4;
      }

      if (tracingSys("expect_xfailure")) {
        cout << "got the expected xfailure\n";
        exit(0);
      }

      // if we don't have a basis for reducing severity, pass this on
      // to the normal handler
      handle_xBase(env, x);
    }
    catch (xBase &x) {
      HANDLER();
      handle_xBase(env, x);
    }

    if (doTime) {
        typeCheckingTimer.stopTimer();
    }

    int numErrors = env.errors.numErrors();
    int numWarnings = env.errors.numWarnings() + parseWarnings;

    // do this now so that 'printTypedAST' will include CFG info
    #ifdef CFG_EXTENSION
    // A possible TODO is to do this right after each function is type
    // checked.  However, in the current design, that means physically
    // inserting code into Function::tcheck (ifdef'd of course).  A way
    // to do it better would be to have a general post-function-tcheck
    // interface that analyses could hook in to.  That could be augmented
    // by a parsing mode that parsed each function, analyzed it, and then
    // immediately discarded its AST.
    if (numErrors == 0) {
      numErrors += computeUnitCFG(unit);
    }
    #endif // CFG_EXTENSION

    // print abstract syntax tree annotated with types
    if (tracingSys("printTypedAST")) {
      unit->debugPrint(cout, 0);
    }

    // structural delta thing
    if (tracingSys("structure")) {
      structurePrint(unit);
    }

    if (numErrors==0 && tracingSys("secondTcheck")) {
      // this is useful to measure the cost of disambiguation, since
      // now the tree is entirely free of ambiguities
      traceProgress() << "beginning second tcheck...\n";
      Env env2(strTable, lang, tfac, madeUpVariables, builtinVars, unit);
      unit->tcheck(env2);
      traceProgress() << "end of second tcheck\n";
    }

    // print errors and warnings
    env.errors.print(cout);

    if (numErrors || numWarnings) {
        cout << "typechecking results:\n"
            << "  errors:   " << numErrors << "\n"
            << "  warnings: " << numWarnings << "\n";
    }

    if (numErrors != 0) {
      return 4;
    }

    // lookup diagnostic
    if (env.collectLookupResults.length()) {
      // scan AST
      NameChecker nc;
      nc.sb << "collectLookupResults";
      unit->traverse(nc);

      // compare to given text
      if (streq(env.collectLookupResults, nc.sb)) {
        // ok
      }
      else {
        cout << "collectLookupResults do not match:\n"
             << "  source: " << env.collectLookupResults << "\n"
             << "  tcheck: " << nc.sb << "\n"
             ;
        return 4;
      }
    }
  }

  // do this before elaboration; I just want to see the result of type
  // checking and disambiguation
  if (wantBpprint) {
    cout << "// bpprint\n";
    bppTranslationUnit(cout, *unit);
  }

  // ---------------- integrity checking ----------------
  {
    if (doTime) {
        integrityCheckingTimer.startTimer();
    }

    // check AST integrity
    IntegrityVisitor ivis;
    unit->traverse(ivis);

    // check that the AST is a tree *and* that the lowered AST is a
    // tree; only do this *after* confirming that tcheck finished
    // without errors
    if (tracingSys("treeCheck")) {
      long start = getMilliseconds();
      LoweredIsTreeVisitor treeCheckVisitor;
      unit->traverse(treeCheckVisitor.loweredVisitor);
      traceProgress() << "done with tree check 1 ("
                      << (getMilliseconds() - start)
                      << " ms)\n";
    }

    // check an expected property of the annotated AST
    if (tracingSys("declTypeCheck") || getenv("declTypeCheck")) {
      DeclTypeChecker vis;
      unit->traverse(vis.loweredVisitor);
      cout << "instances of type != var->type: " << vis.instances << endl;
    }

    if (doTime) {
        integrityCheckingTimer.stopTimer();
    }

    if (tracingSys("stopAfterTCheck")) {
      return 0;
    }
  }

  // ----------------- elaboration ------------------
  if (tracingSys("no-elaborate")) {
    cout << "no-elaborate" << endl;
  }
  else {
    if (doTime) {
        elaborationTimer.startTimer();
    }

    ElabVisitor vis(strTable, tfac, unit);

    if (!lang.isCplusplus) {
      // do only the C elaboration activities
      vis.activities = EA_C_ACTIVITIES;
    }

    // if we are going to pretty print, then we need to retain defunct children
    if (tracingSys("prettyPrint")
        // dsw: I don't know if this is right, but printing the xml
        // AST kind of resembles pretty-printing the AST; fix this if
        // it is wrong
        || tracingSys("xmlPrintAST")
        || wantBpprintAfterElab
        ) {
      vis.cloneDefunctChildren = true;
    }

    // do elaboration
    unit->traverse(vis.loweredVisitor);

    // print abstract syntax tree annotated with types
    if (tracingSys("printElabAST")) {
      unit->debugPrint(cout, 0);
    }

    if (doTime) {
        elaborationTimer.stopTimer();
    }
    if (tracingSys("stopAfterElab")) {
      return 0;
    }
  }

  // mark "real" (non-template) variables as such
  if (!tracingSys("parseXml")) {
    // mark "real" (non-template) variables as such
    MarkRealVars markReal;
    visitVarsF(builtinVars, markReal);
    visitRealVarsF(unit, markReal);
  }

  // more integrity checking
  {
    if (doTime) {
        integrityCheckingTimer.startTimer();
    }

    // check that the AST is a tree *and* that the lowered AST is a
    // tree (do this *after* elaboration!)
    if (tracingSys("treeCheck")) {
      long start = getMilliseconds();
      LoweredIsTreeVisitor treeCheckVisitor;
      unit->traverse(treeCheckVisitor.loweredVisitor);
      traceProgress() << "done with tree check 2 ("
                      << (getMilliseconds() - start)
                      << " ms)\n";
    }

    if (doTime) {
        integrityCheckingTimer.stopTimer();
    }
  }

  // dsw: pretty printing
  if (tracingSys("prettyPrint")) {
    traceProgress() << "dsw pretty print...\n";
    cout << "---- START ----" << endl;
    cout << "// -*-c++-*-" << endl;
    prettyPrintTranslationUnit(cout, *unit);
    cout << "---- STOP ----" << endl;
    traceProgress() << "dsw pretty print... done\n";
  }

  if (wantBpprintAfterElab) {
    cout << "// bpprintAfterElab\n";
    bppTranslationUnit(cout, *unit);
  }

  // dsw: xml printing of the raw ast
  if (tracingSys("xmlPrintAST")) {
    traceProgress() << "dsw xml print...\n";
    bool indent = tracingSys("xmlPrintAST-indent");
    int depth = 0;              // shared depth counter between printers
    cout << "---- START ----" << endl;

    // serialize Files
    IdentityManager idmgr;
    XmlFileWriter fileXmlWriter(idmgr, &cout, depth, indent, NULL);
    fileXmlWriter.toXml(sourceLocManager->serializationOnly_get_files());

    // serialize AST and maybe Types
    if (tracingSys("xmlPrintAST-types")) {
      IdentityManager idmgr;
      XmlTypeWriter xmlTypeVis( idmgr, (ASTVisitor*)NULL, &cout, depth, indent, NULL );
      XmlTypeWriter_AstVisitor xmlVis_Types(xmlTypeVis, cout, depth, indent);
      xmlTypeVis.astVisitor = &xmlVis_Types;
      ASTVisitor *vis = &xmlVis_Types;
      LoweredASTVisitor loweredXmlVis(&xmlVis_Types); // might not be used
      if (tracingSys("xmlPrintAST-lowered")) {
        vis = &loweredXmlVis;
      }
      unit->traverse(*vis);
    } else {
      IdentityManager idmgr;
      XmlAstWriter_AstVisitor xmlVis(cout, idmgr, depth, indent);
      ASTVisitor *vis = &xmlVis;
      LoweredASTVisitor loweredXmlVis(&xmlVis); // might not be used
      if (tracingSys("xmlPrintAST-lowered")) {
        vis = &loweredXmlVis;
      }
      unit->traverse(*vis);
    }

    cout << endl;
    cout << "---- STOP ----" << endl;
    traceProgress() << "dsw xml print... done\n";
  }

  // test AST cloning
  if (tracingSys("testClone")) {
    TranslationUnit *u2 = unit->clone();

    if (tracingSys("cloneAST")) {
      cout << "------- cloned AST --------\n";
      u2->debugPrint(cout, 0);
    }

    if (tracingSys("cloneCheck")) {
      ArrayStack<Variable*> madeUpVariables2;
      ArrayStack<Variable*> builtinVars2;
      // dsw: I hope you intend that I should use the cloned TranslationUnit
      Env env3(strTable, lang, tfac, madeUpVariables2, builtinVars2, u2);
      u2->tcheck(env3);

      if (tracingSys("cloneTypedAST")) {
        cout << "------- cloned typed AST --------\n";
        u2->debugPrint(cout, 0);
      }

      if (tracingSys("clonePrint")) {
        OStreamOutStream out0(cout);
        CodeOutStream codeOut(out0);
        CTypePrinter typePrinter;
        PrintEnv penv(typePrinter, &codeOut);
        cout << "---- cloned pretty print ----" << endl;
        u2->print(penv);
        codeOut.finish();
      }
    }
  }

  // test debugPrint but send the output to /dev/null (basically just
  // make sure it doesn't segfault or abort)
  if (tracingSys("testDebugPrint")) {
    ofstream devnull("/dev/null");
    unit->debugPrint(devnull, 0);
  }

  if (!cc2cOutputFname.empty()) {
    TranslationUnit *lowered = cc_to_c(strTable, *unit);
    if (cc2cOutputFname == string("-")) {
      cout << "// cc2c\n";
      bppTranslationUnit(cout, *lowered);
    }
    else {
      ofstream out(cc2cOutputFname.c_str());
      if (!out) {
        xsyserror("open", stringb("write \"" << cc2cOutputFname << "\""));
      }
      out << "// cc2c\n";
      bppTranslationUnit(out, *lowered);
    }
  }

    if (doTime) {
        llvmGenerationTimer.startTimer();
    }
    // RICH: Target data and target triple.
    mod = cc_to_llvm(outputFname, strTable, *unit,
        "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-s0:0:64-f80:32:32",
        "i686-pc-linux-gnu");
    if (doTime) {
        llvmGenerationTimer.stopTimer();
    }

  //traceProgress() << "cleaning up...\n";

  //malloc_stats();

  // delete the tree
  // (currently this doesn't do very much because FakeLists are
  // non-owning, so I won't pretend it does)
  //delete unit;

  strTable.clear();

  //checkHeap();
  //malloc_stats();
  return 0;
}

int Elsa::parse(Language language, const char* inputFname, const char* outputFname, llvm::Module*& mod)
{
  try {
    return doit(language, inputFname, outputFname, mod);
  }
  catch (XUnimp &x) {
    HANDLER();
    cout << x << endl;

    // don't consider this the same as dying on an assertion failure;
    // I want to have tests in regrtest that are "expected" to fail
    // for the reason that they use unimplemented language features
    return 10;
  }
  catch (XFatal &x) {
    HANDLER();

    // similar to XUnimp
    cout << x << endl;
    return 10;
  }
  catch (xBase &x) {
    HANDLER();
    cout << x << endl;
    abort();
  }

  //malloc_stats();

  return 0;
}
