//===- Nios2RegisterInfo.td - Nios2 Register defs -----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe NIOS2 instructions format
//
//  CPU INSTRUCTION FORMATS
//
//  opcode  - operation code.
//  rs      - src reg.
//  rt      - dst reg (on a 2 regs instr) or src reg (on a 3 reg instr).
//  rd      - dst reg, only used on 3 regs instr.
//  shamt   - only used on shift instructions, contains the shift amount.
//  funct   - combined with opcode field give us an operation code.
//
//===----------------------------------------------------------------------===//

// Generic Nios2 Format
class Nios2Inst<dag outs, dag ins, string asmstr, list<dag> pattern, 
               InstrItinClass itin>: Instruction 
{
  field bits<32> Inst;

  let Namespace = "Nios2";

  bits<6> opcode;

  // The bottom 5 bits are the 'opcode' field
  let Inst{5-0} = opcode;   
  
  dag OutOperandList = outs;
  dag InOperandList  = ins;

  let AsmString   = asmstr;
  let Pattern     = pattern;
  let Itinerary   = itin;
}

// Nios2 Pseudo Instructions Format
class Nios2Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>:
      Nios2Inst<outs, ins, asmstr, pattern, IIPseudo>;

//===----------------------------------------------------------------------===//
// Format R instruction class in Nios2 : <|opcode|rs|rt|rd|shamt|funct|>
//===----------------------------------------------------------------------===//

class FR<bits<6> _opx, bits<5> _imm5, dag outs, dag ins, string asmstr,
         list<dag> pattern, InstrItinClass itin>:
      Nios2Inst<outs, ins, asmstr, pattern, itin> 
{
  bits<5>  ra;
  bits<5>  rb;
  bits<5>  rc;
  bits<6>  opx;
  bits<5>  imm5;

  let opcode = 0x3A;
  let opx  = _opx;

  let Inst{31-27} = ra;
  let Inst{26-22} = rb; 
  let Inst{21-17} = rc;
  let Inst{16-11}  = opx;
  let Inst{10-6}  = imm5;
}

//===----------------------------------------------------------------------===//
// Format I instruction class in Nios2 : <|opcode|rs|rt|immediate|>
//===----------------------------------------------------------------------===//

class FI<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern,
         InstrItinClass itin>: Nios2Inst<outs, ins, asmstr, pattern, itin> 
{
  bits<5>  ra;
  bits<5>  rb;
  bits<16> imm16;

  let opcode = op;

  let Inst{31-27} = ra;
  let Inst{26-22} = rb; 
  let Inst{21-6}  = imm16;
}

//===----------------------------------------------------------------------===//
// Format J instruction class in Nios2 : <|opcode|address|>
//===----------------------------------------------------------------------===//

class FJ<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern,
         InstrItinClass itin>: Nios2Inst<outs, ins, asmstr, pattern, itin> 
{
  bits<26> addr;

  let opcode = op;
  
  let Inst{31-6} = addr;
}

//===----------------------------------------------------------------------===//
//
//  FLOATING POINT INSTRUCTION FORMATS
//
//  opcode  - operation code.
//  fs      - src reg.
//  ft      - dst reg (on a 2 regs instr) or src reg (on a 3 reg instr).
//  fd      - dst reg, only used on 3 regs instr.
//  fmt     - double or single precision.
//  funct   - combined with opcode field give us an operation code.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Format FR instruction class in Nios2 : <|opcode|fmt|ft|fs|fd|funct|>
//===----------------------------------------------------------------------===//

class FFR<bits<6> op, bits<6> _funct, bits<5> _fmt, dag outs, dag ins, 
          string asmstr, list<dag> pattern> : 
          Nios2Inst<outs, ins, asmstr, pattern, NoItinerary> 
{
  bits<5>  fd;
  bits<5>  fs;
  bits<5>  ft;
  bits<5>  fmt;
  bits<6>  funct;

  let opcode = op;
  let funct  = _funct;
  let fmt    = _fmt;

  let Inst{25-21} = fmt;
  let Inst{20-16} = ft; 
  let Inst{15-11} = fs;
  let Inst{10-6}  = fd;
  let Inst{5-0}   = funct;
}

//===----------------------------------------------------------------------===//
// Format FI instruction class in Nios2 : <|opcode|base|ft|immediate|>
//===----------------------------------------------------------------------===//

class FFI<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern>: 
          Nios2Inst<outs, ins, asmstr, pattern, NoItinerary> 
{
  bits<5>  ft;
  bits<5>  base;
  bits<16> imm16;

  let opcode = op;

  let Inst{25-21} = base;
  let Inst{20-16} = ft; 
  let Inst{15-0}  = imm16;
}
