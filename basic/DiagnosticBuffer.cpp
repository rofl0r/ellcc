//===--- DiagnosticBuffer.cpp - Buffer Diagnostics ---------------===//
//
//===----------------------------------------------------------------------===//
//
// This is a concrete diagnostic client, which buffers the diagnostic messages.
//
//===----------------------------------------------------------------------===//

#include "DiagnosticBuffer.h"
using namespace ellcc;

/** HandleDiagnostic - Store the diagnostics that are reported.
 */ 
void DiagnosticBuffer::HandleDiagnostic(Diagnostic::Level Level,
                                        const DiagnosticInfo &Info) {
  Diagnostic* diag = new Diagnostic(this);
  *diag = *Info.getDiags();
  DiagnosticInfo MyInfo(diag);
  diags.push_back(MyInfo);
}
