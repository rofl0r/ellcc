//===--- DiagnosticBuffer.cpp - Buffer Diagnostics ---------------===//
//
//===----------------------------------------------------------------------===//
//
// This is a concrete diagnostic client, which buffers the diagnostic messages.
//
//===----------------------------------------------------------------------===//

#include "DiagnosticBuffer.h"
#include "llvm/ADT/SmallString.h"
using namespace ellcc;

/// HandleDiagnostic - Store the errors, warnings, and notes that are
/// reported.
/// 
void DiagnosticBuffer::HandleDiagnostic(Diagnostic::Level Level,
                                        const DiagnosticInfo &Info) {
  llvm::SmallString<100> StrC;
  Info.FormatDiagnostic(StrC);
  std::string Str(StrC.begin(), StrC.end());
  switch (Level) {
  default: assert(0 && "Diagnostic not handled during diagnostic buffering!");
  case Diagnostic::Note:
    Notes.push_back(std::make_pair(Info.getLocation(), Str));
    break;
  case Diagnostic::Warning:
    Warnings.push_back(std::make_pair(Info.getLocation(), Str));
    break;
  case Diagnostic::Error:
  case Diagnostic::Fatal:
    Errors.push_back(std::make_pair(Info.getLocation(), Str));
    break;
  }
}
