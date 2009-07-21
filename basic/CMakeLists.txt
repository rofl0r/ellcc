include(${ELLCC_SOURCE_DIR}/cmake/ellcc_components.cmake)

add_library(basic ${ELLCC_LIBRARY_TYPE}
    ConvertUTF.c DiagnosticBuffer.cpp Diagnostic.cpp
    FileManager.cpp IdentifierTable.cpp LangOptions.cpp SourceLocation.cpp
    SourceManager.cpp TargetInfo.cpp Targets.cpp TokenKinds.cpp)

install_targets(/lib basic)
