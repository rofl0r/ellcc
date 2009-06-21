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
void DiagnosticBuffer::HandleDiagnostic(Diagnostic::Level level,
                                        const DiagnosticInfo &Info) {
  Diagnostic* diag = new Diagnostic(this);
  *diag = *Info.getDiags();
  diags.push_back(std::pair<Diagnostic::Level, Diagnostic*>(level, diag));
}

const DiagnosticInfo DiagnosticBuffer::take(DiagnosticClient* client, Diagnostic::Level& level)
{
    if (diags.size() == 0) return DiagnosticInfo(NULL);
    level = diags[0].first;
    Diagnostic* diag = diags[0].second;
    diags.erase(diags.begin());
    diag->setClient(client);
    return DiagnosticInfo(diag);
}

void DiagnosticBuffer::take(DiagnosticClient* client, DiagFlags flags)
{
    for (DiagnosticBuffer::const_iterator it = begin(); it != end(); ++it) {
        if (it->second->getFlags() & flags) {
            Diagnostic::Level level;
            DiagnosticInfo info(take(client, level));
            client->HandleDiagnostic(level, info);
        }
    }
}

int DiagnosticBuffer::numberOf(DiagFlags flags)
{
    int count = 0;
    for (iterator it = diags.begin(); it != diags.end(); ++it) {
        if (it->second->getFlags() & flags) {
            ++count;
        }
    }

    return count;
}

