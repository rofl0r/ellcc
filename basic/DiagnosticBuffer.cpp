//===--- DiagnosticBuffer.cpp - Buffer Diagnostics ---------------===//
//
//===----------------------------------------------------------------------===//
//
// This is a concrete diagnostic client, which buffers the diagnostic messages.
//
//===----------------------------------------------------------------------===//

#include "DiagnosticBuffer.h"
using namespace ellcc;

DiagnosticBuffer::DiagnosticBuffer() { }
DiagnosticBuffer::~DiagnosticBuffer()
{
    Discard();
}

/** HandleDiagnostic - Store the diagnostics that are reported.
 */ 
void DiagnosticBuffer::HandleDiagnostic(Diagnostic::Level level,
                                        const DiagnosticInfo &Info)
{
  DiagnosticData* diag = new DiagnosticData();
  *diag = *Info.getDiags();
  diags.push_back(std::pair<Diagnostic::Level, DiagnosticData*>(level, diag));
}

void DiagnosticBuffer::Filter(DiagFlags flags)
{
    for (iterator it = diags.begin(); it != diags.end(); ++it) {
        if ((it->second->getFlags() & flags) == DIAG_NONE) {
            it->second->addFlags(DIAG_IGNORE);
        }
    }
}

int DiagnosticBuffer::NumberOf(DiagFlags flags)
{
    int count = 0;
    for (iterator it = diags.begin(); it != diags.end(); ++it) {
        if (it->second->getFlags() & flags) {
            ++count;
        }
    }

    return count;
}

void DiagnosticBuffer::ErrorsToWarnings()
{
    for (iterator it = diags.begin(); it != diags.end(); ++it) {
        if (it->first == Diagnostic::Error) {
            it->first = Diagnostic::Warning;
            it->second->addFlags(DIAG_WARNING);
        }
    }
}

