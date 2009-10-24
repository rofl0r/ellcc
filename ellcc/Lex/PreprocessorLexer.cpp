//===--- PreprocessorLexer.cpp - C Language Family Lexer ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements the PreprocessorLexer and Token interfaces.
//
//===----------------------------------------------------------------------===//

#include "PreprocessorLexer.h"
#include "Preprocessor.h"
#include "LexDiagnostic.h"
#include "../basic/SourceManager.h"
using namespace ellcc;

/// LexIncludeFilename - After the preprocessor has parsed a #include, lex and
/// (potentially) macro expand the filename.
void PreprocessorLexer::LexIncludeFilename(Token &FilenameTok) {
  assert(ParsingPreprocessorDirective &&
         ParsingFilename == false &&
         "Must be in a preprocessing directive!");

  // We are now parsing a filename!
  ParsingFilename = true;
  
  // Lex the filename.
  IndirectLex(FilenameTok);

  // We should have obtained the filename now.
  ParsingFilename = false;
  
  // No filename?
  if (FilenameTok.is(tok::eom))
    PP->Diag(FilenameTok.getLocation(), diag::err_pp_expects_filename);
}

/// getFileEntry - Return the FileEntry corresponding to this FileID.  Like
/// getFileID(), this only works for lexers with attached preprocessors.
const FileEntry *PreprocessorLexer::getFileEntry() const {
  return PP->getSourceManager().getFileEntryForID(getFileID());
}
