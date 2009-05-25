//===- Version.h - Clang Version Number -------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This header defines version macros for Clang.
//
//===----------------------------------------------------------------------===//

#ifndef VERSION_H
#define VERSION_H

/// \brief Clang major version
#define ELLCC_VERSION_MAJOR 1

/// \brief Clang minor version
#define ELLCC_VERSION_MINOR 0

/// \brief Helper macro for ELLCC_VERSION_STRING.
#define ELLCC_MAKE_VERSION_STRING2(X) #X

/// \brief Helper macro for ELLCC_VERSION_STRING.
#define ELLCC_MAKE_VERSION_STRING(X,Y) ELLCC_MAKE_VERSION_STRING2(X.Y)

/// \brief A string that describes the Clang version number, e.g.,
/// "1.0".
#define ELLCC_VERSION_STRING \
  ELLCC_MAKE_VERSION_STRING(ELLCC_VERSION_MAJOR,ELLCC_VERSION_MINOR)


#endif
