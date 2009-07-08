
add_library(basic # SHARED
    ConvertUTF.c DiagnosticBuffer.cpp Diagnostic.cpp
    FileManager.cpp IdentifierTable.cpp LangOptions.cpp SourceLocation.cpp
    SourceManager.cpp TargetInfo.cpp Targets.cpp TokenKinds.cpp)

install_targets(/lib basic)
