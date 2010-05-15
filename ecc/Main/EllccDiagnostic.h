//===--- EllccDiagnostic.h - Diagnostics for ELLCC ---------------*- C++ -*-===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef ELLCC_DIAGNOSTIC_H
#define ELLCC_DIAGNOSTIC_H

#include "Diagnostic.h"

namespace ellcc {
  namespace diag { 
    enum {
#define DIAG(ENUM,FLAGS,DEFAULT_MAPPING,DESC,GROUP,SFINAE,CATEGORY) ENUM,
#define ELLCCSTART
#include "DiagnosticEllccKinds.inc"
#undef DIAG
      NUM_BUILTIN_ELLCC_DIAGNOSTICS
    };
  }  // end namespace diag
}  // end namespace ellcc

#endif
