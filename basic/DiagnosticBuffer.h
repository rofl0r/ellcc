//===--- DiagnosticBuffer.h - Buffer Diagnostics -------*- C++ -*-===//
//
// This is a concrete diagnostic client, which buffers the diagnostic messages.
//
//===----------------------------------------------------------------------===//

#ifndef DIAGNOSTIC_BUFFER_H
#define DIAGNOSTIC_BUFFER_H

#include "Diagnostic.h"
#include <vector>

namespace ellcc {

class Preprocessor;
class SourceManager;

class DiagnosticBuffer : public DiagnosticClient {
public:
  typedef std::vector<std::pair<Diagnostic::Level, DiagnosticData*> > DiagList;
  typedef DiagList::const_iterator const_iterator;
private:
  typedef DiagList::iterator iterator;
  DiagList diags;
public:
  const_iterator begin() const  { return diags.begin(); }
  const_iterator end() const    { return diags.end(); }

  DiagnosticBuffer();
  ~DiagnosticBuffer();

  void Discard()
  {
      for (iterator it = diags.begin(); it != diags.end(); ++it) {
        delete it->second;
      }
  }

  void Filter(DiagFlags flags);

  int NumberOf(DiagFlags flags);

  void ErrorsToWarnings();
  
  void HandleDiagnostic(Diagnostic::Level DiagLevel, const DiagnosticInfo &Info,
                        std::vector<SourceLocation>* InstantiationLocStack = NULL);
};

} // end namspace ellcc

#endif
