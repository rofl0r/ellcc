//===- Nios2RegisterInfo.td - Nios2 Register defs -----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the NIOS2 register file
//===----------------------------------------------------------------------===//

// We have registers.
class Nios2Reg<string n> : Register<n> {
  field bits<5> Num;
  let Namespace = "Nios2";
}

// Nios2 CPU Registers
class Nios2GPRReg<bits<5> num, string n> : Nios2Reg<n> {
  let Num = num;
}

//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//

let Namespace = "Nios2" in {

  // General Purpose Registers
  def ZERO : Nios2GPRReg< 0, "zero">, DwarfRegNum<[0]>;
  def AT   : Nios2GPRReg< 1, "at">,   DwarfRegNum<[1]>;
  def V0   : Nios2GPRReg< 2, "r2">,    DwarfRegNum<[2]>;
  def V1   : Nios2GPRReg< 3, "r3">,    DwarfRegNum<[3]>;
  def A0   : Nios2GPRReg< 4, "r4">,    DwarfRegNum<[5]>;
  def A1   : Nios2GPRReg< 5, "r5">,    DwarfRegNum<[5]>;
  def A2   : Nios2GPRReg< 6, "r6">,    DwarfRegNum<[6]>;
  def A3   : Nios2GPRReg< 7, "r7">,    DwarfRegNum<[7]>;
  def T0   : Nios2GPRReg< 8, "r8">,    DwarfRegNum<[8]>;
  def T1   : Nios2GPRReg< 9, "r9">,    DwarfRegNum<[9]>;
  def T2   : Nios2GPRReg< 10, "r10">,  DwarfRegNum<[10]>;
  def T3   : Nios2GPRReg< 11, "r11">,  DwarfRegNum<[11]>;
  def T4   : Nios2GPRReg< 12, "r12">,  DwarfRegNum<[12]>;
  def T5   : Nios2GPRReg< 13, "r13">,  DwarfRegNum<[13]>;
  def T6   : Nios2GPRReg< 14, "r14">,  DwarfRegNum<[14]>;
  def T7   : Nios2GPRReg< 15, "r15">,  DwarfRegNum<[15]>;
  def S0   : Nios2GPRReg< 16, "r16">,  DwarfRegNum<[16]>;
  def S1   : Nios2GPRReg< 17, "r17">,  DwarfRegNum<[17]>;
  def S2   : Nios2GPRReg< 18, "r18">,  DwarfRegNum<[18]>;
  def S3   : Nios2GPRReg< 19, "r19">,  DwarfRegNum<[19]>;
  def S4   : Nios2GPRReg< 20, "r20">,  DwarfRegNum<[20]>;
  def S5   : Nios2GPRReg< 21, "r21">,  DwarfRegNum<[21]>;
  def S6   : Nios2GPRReg< 22, "r22">,  DwarfRegNum<[22]>;
  def S7   : Nios2GPRReg< 23, "r23">,  DwarfRegNum<[23]>;
  def ET   : Nios2GPRReg< 24, "et">,  DwarfRegNum<[24]>;
  def BT   : Nios2GPRReg< 25, "bt">,  DwarfRegNum<[25]>;
  def GP   : Nios2GPRReg< 26, "gp">,  DwarfRegNum<[26]>;
  def SP   : Nios2GPRReg< 27, "sp">,  DwarfRegNum<[27]>;
  def FP   : Nios2GPRReg< 28, "fp">,  DwarfRegNum<[28]>;
  def EA   : Nios2GPRReg< 29, "ea">,  DwarfRegNum<[29]>;
  def BA   : Nios2GPRReg< 30, "ba">,  DwarfRegNum<[30]>;
  def RA   : Nios2GPRReg< 31, "ra">,  DwarfRegNum<[31]>;
}

//===----------------------------------------------------------------------===//
// Register Classes
//===----------------------------------------------------------------------===//

def CPURegs : RegisterClass<"Nios2", [i32], 32, 
  // Return Values and Arguments
  [V0, V1, A0, A1, A2, A3,
  // Not preserved across procedure calls
  T0, T1, T2, T3, T4, T5, T6, T7,
  // Callee save
  S0, S1, S2, S3, S4, S5, S6, S7,
  // Reserved
  ZERO, AT, ET, BT, GP, SP, FP, EA, BA, RA]>
{
  let MethodProtos = [{
    iterator allocation_order_begin(const MachineFunction &MF) const;
    iterator allocation_order_end(const MachineFunction &MF) const;
  }];
  let MethodBodies = [{
    // FP is reserved.
    static const unsigned FPReserved[] = {
        Nios2::V0, Nios2::V1,
        Nios2::A0, Nios2::A1, Nios2::A2, Nios2::A3,
        Nios2::T0, Nios2::T1, Nios2::T2, Nios2::T3,
        Nios2::T4, Nios2::T5, Nios2::T6, Nios2::T7,
        Nios2::S0, Nios2::S1, Nios2::S2, Nios2::S3,
        Nios2::S4, Nios2::S5, Nios2::S6, Nios2::S7,
    };
    static const unsigned FPAvailable[] = {
        Nios2::V0, Nios2::V1,
        Nios2::A0, Nios2::A1, Nios2::A2, Nios2::A3,
        Nios2::T0, Nios2::T1, Nios2::T2, Nios2::T3,
        Nios2::T4, Nios2::T5, Nios2::T6, Nios2::T7,
        Nios2::FP,
        Nios2::S0, Nios2::S1, Nios2::S2, Nios2::S3,
        Nios2::S4, Nios2::S5, Nios2::S6, Nios2::S7,
    };
    CPURegsClass::iterator
    CPURegsClass::allocation_order_begin(const MachineFunction &MF) const {
      const TargetMachine &TM = MF.getTarget();
      const TargetRegisterInfo *RI = TM.getRegisterInfo();
 
      if (RI->hasFP(MF)) {
          return FPReserved;
      }
      return FPAvailable;
    }
    CPURegsClass::iterator
    CPURegsClass::allocation_order_end(const MachineFunction &MF) const {
      const TargetMachine &TM = MF.getTarget();
      const TargetRegisterInfo *RI = TM.getRegisterInfo();
      CPURegsClass::iterator I;
      if (RI->hasFP(MF)) {
          I = FPReserved + (sizeof(FPReserved) / sizeof(unsigned));
      } else {
          I = FPAvailable + (sizeof(FPAvailable) / sizeof(unsigned));
      }
      return I;
    }
  }];
}

