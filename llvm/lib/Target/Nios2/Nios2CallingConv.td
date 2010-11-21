//===- Nios2CallingConv.td - Calling Conventions for Nios2 --------*- C++ -*-===//
// 
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//
// This describes the calling conventions for Nios2 architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Nios2 O32 Calling Convention
//===----------------------------------------------------------------------===//
def CC_Nios2 : CallingConv<[
  // Promote i8/i16 arguments to i32.
  CCIfType<[i8, i16], CCPromoteToType<i32>>,

  // The first 4 integer arguments are passed in integer registers.
  CCIfType<[i32], CCAssignToReg<[A0, A1, A2, A3]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

def RetCC_Nios2 : CallingConv<[
  // i32 are returned in registers V0, V1
  CCIfType<[i32], CCAssignToReg<[V0, V1]>>,

  // f32 are returned in registers V0, V1
  CCIfType<[f32], CCAssignToReg<[V0, V1]>>

]>;
