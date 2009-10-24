//===--- DiagnosticLex.h - Diagnostics for liblex ---------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef DIAGNOSTICLEX_H
#define DIAGNOSTICLEX_H

#include "../basic/Diagnostic.h"

namespace ellcc {
  namespace diag { 
    enum {
#define DIAG(ENUM,FLAGS,DEFAULT_MAPPING,DESC,GROUP,SFINAE) ENUM,
#define LEXSTART
#include "../basic/DiagnosticLexKinds.inc"
#undef DIAG
      NUM_BUILTIN_LEX_DIAGNOSTICS
    };
  }  // end namespace diag
}  // end namespace ellcc

#endif
