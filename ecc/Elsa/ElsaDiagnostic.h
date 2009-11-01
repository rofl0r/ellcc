//===--- ElsaDiagnostic.h - Diagnostics for Elsa ---------------*- C++ -*-===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef ELSA_DIAGNOSTIC_H
#define ELSA_DIAGNOSTIC_H

#include "Diagnostic.h"

namespace ellcc {
  namespace diag { 
    enum {
#define DIAG(ENUM,FLAGS,DEFAULT_MAPPING,DESC,GROUP,SFINAE) ENUM,
#define ELSASTART
#include "DiagnosticElsaKinds.inc"
#undef DIAG
      NUM_BUILTIN_ELSA_DIAGNOSTICS
    };
  }  // end namespace diag
}  // end namespace ellcc

#endif
