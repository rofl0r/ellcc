// tlexer.cc            see license.txt for copyright and terms of use
// test the lexer alone

#include "lexer.h"         // Lexer
#include "strtable.h"      // StringTable
#include "LangOptions.h"   // LangOptions
#include "test.h"          // ARGS_MAIN
#include "nonport.h"       // getMilliseconds
#include "trace.h"         // tracingSys
#include <iostream>        // cout

using namespace ellcc;

void entry(int argc, char **argv)
{
  char const *progName = argv[0];
  TRACE_ARGS()

  if (argc != 2) {
    std::cout << "usage: " << progName << " [-tr tokens] input.i\n";
    return;
  }
  traceAddSys("progress");

  StringTable table;
  LangOptions LO;
  LO.ANSI_Cplusplus98();     // want 'true' and 'false' keywords
  SourceLocManager mgr;
  traceProgress() << "making Lexer\n";
  Lexer lexer(table, LO, argv[1]);
  Lexer::NextTokenFunc nextToken = lexer.getTokenFunc();

  bool print = tracingSys("tokens");

  traceProgress() << "lexing " << argv[1] << "...\n";
  long start = getMilliseconds();

  while (lexer.type != 0 /*eof*/) {
    if (print) {
      std::cout << toString(lexer.loc) << ": " << lexer.tokenDesc() << std::endl;
    }

    nextToken(&lexer);
  }

  traceProgress() << "done lexing (" << (getMilliseconds() - start)
                  << " ms)\n";
}

ARGS_MAIN
