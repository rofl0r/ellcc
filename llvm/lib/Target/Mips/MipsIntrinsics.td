//===- IntrinsicsMips.td - Defines Mips intrinsics -----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the Mips-specific intrinsics.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Definitions for all Mips intrinsics.
//
// TODO: Real intrinsics. Have a dummy intrinsic as a place holder.
// Mips intrinsic classes.

let TargetPrefix = "mips", isTarget = 1 in {
  class MIPSDUMMY_Get_Intrinsic : Intrinsic<[llvm_i32_ty], [llvm_i32_ty], []>;

}

//===----------------------------------------------------------------------===//
// Mips Get Intrinsic Definitions.
//

def int_mips_dummy_get      : GCCBuiltin<"__builtin_mips_dummy_get">,
                              MIPSDUMMY_Get_Intrinsic;
