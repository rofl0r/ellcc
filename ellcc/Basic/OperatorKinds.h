//===--- OperatorKinds.h - C++ Overloaded Operators -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines C++ overloaded operators.
//
//===----------------------------------------------------------------------===//

#ifndef OPERATOR_KINDS_H
#define OPERATOR_KINDS_H

namespace ellcc {

/// OverloadedOperatorKind - Enumeration specifying the different kinds of
/// C++ overloaded operators.
enum OverloadedOperatorKind {
  OO_None,                //< Not an overloaded operator
#define OVERLOADED_OPERATOR(Name,Spelling,Token,Unary,Binary,MemberOnly) \
  OO_##Name,
#include "OperatorKinds.def"
  NUM_OVERLOADED_OPERATORS
};


} // end namespace ellcc

#endif
