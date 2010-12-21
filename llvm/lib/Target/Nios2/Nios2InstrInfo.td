//===- Nios2InstrInfo.td - Nios2 Register defs --------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "Nios2InstrFormats.td"

//===----------------------------------------------------------------------===//
// Nios2 profiles and nodes
//===----------------------------------------------------------------------===//

def SDT_Nios2Ret          : SDTypeProfile<0, 0, []>;
def SDT_Nios2ERet         : SDTypeProfile<0, 0, []>;
def SDT_Nios2Call         : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;
def SDT_Nios2SelectCC     : SDTypeProfile<1, 3, [SDTCisSameAs<0, 2>, 
                                         SDTCisSameAs<2, 3>, SDTCisInt<1>]>;
def SDT_Nios2CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>]>;
def SDT_Nios2CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;

// Call
def Nios2Call : SDNode<"Nios2ISD::Call",SDT_Nios2Call,
                       [SDNPHasChain, SDNPOutFlag, SDNPOptInFlag,
                        SDNPVariadic]>;

// Hi and Lo nodes are used to handle global addresses. Used on 
// Nios2ISelLowering to lower stuff like GlobalAddress, ExternalSymbol 
// static model. (nothing to do with Nios2 Registers Hi and Lo)
def Nios2Hi    : SDNode<"Nios2ISD::Hi", SDTIntUnaryOp>;
def Nios2Lo    : SDNode<"Nios2ISD::Lo", SDTIntUnaryOp>;
def Nios2GPRel : SDNode<"Nios2ISD::GPRel", SDTIntUnaryOp>;

// Return
def Nios2Ret : SDNode<"Nios2ISD::Ret", SDT_Nios2Ret, [SDNPHasChain, 
                     SDNPOptInFlag]>;
// Return from exception.
def Nios2ERet : SDNode<"Nios2ISD::ERet", SDT_Nios2ERet, [SDNPHasChain, 
                     SDNPOptInFlag]>;

// These are target-independent nodes, but have target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_Nios2CallSeqStart,
                           [SDNPHasChain, SDNPOutFlag]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_Nios2CallSeqEnd,
                           [SDNPHasChain, SDNPOptInFlag, SDNPOutFlag]>;

// Select Condition Code
def Nios2SelectCC  : SDNode<"Nios2ISD::SelectCC", SDT_Nios2SelectCC>;

//===----------------------------------------------------------------------===//
// Nios2 Instruction Predicate Definitions.
//===----------------------------------------------------------------------===//
def HasMul : Predicate<"Subtarget.hasMul()">;
def HasMulx : Predicate<"Subtarget.hasMulx()">;
def HasDiv : Predicate<"Subtarget.hasDiv()">;

//===----------------------------------------------------------------------===//
// Nios2 Operand, Complex Patterns and Transformations Definitions.
//===----------------------------------------------------------------------===//

// Instruction operand types
def brtarget    : Operand<OtherVT>;
def calltarget  : Operand<i32>;
def simm16      : Operand<i32>;
def opx         : Operand<i32>;

// Unsigned Operand
def uimm16      : Operand<i32> {
  let PrintMethod = "printUnsignedImm";
}

// Address operand
def mem : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops simm16, CPURegs);
}

// Transformation Function - get the lower 16 bits.
def LO16 : SDNodeXForm<imm, [{
  return getI32Imm((unsigned)N->getZExtValue() & 0xFFFF);
}]>;

// Transformation Function - get the higher 16 bits.
def HI16 : SDNodeXForm<imm, [{
  return getI32Imm((unsigned)N->getZExtValue() >> 16);
}]>;

// Node immediate fits as 16-bit sign extended on target immediate.
// e.g. addi
def immSExt16  : PatLeaf<(imm), [{
  if (N->getValueType(0) == MVT::i32)
    return (int32_t)N->getZExtValue() == (short)N->getZExtValue();
  else
    return (int64_t)N->getZExtValue() == (short)N->getZExtValue();
}]>;

// Node immediate fits as 16-bit zero extended on target immediate.
// The LO16 param means that only the lower 16 bits of the node
// immediate are caught.
// e.g. ori, andi
def immZExt16  : PatLeaf<(imm), [{
  if (N->getValueType(0) == MVT::i32)
    return (uint32_t)N->getZExtValue() == (unsigned short)N->getZExtValue();
  else
    return (uint64_t)N->getZExtValue() == (unsigned short)N->getZExtValue();
}], LO16>;

// opx field must fit in 5 bits.
def immZExt5 : PatLeaf<(imm), [{
  return N->getZExtValue() == ((N->getZExtValue()) & 0x1f) ;
}]>;

// Nios2 Address Mode! SDNode frameindex could possibily be a match
// since load and store instructions from stack used it.
def addr : ComplexPattern<i32, 2, "SelectAddr", [frameindex], []>;

//===----------------------------------------------------------------------===//
// Instructions specific format
//===----------------------------------------------------------------------===//

// Arithmetic 3 register operands
let isCommutable = 1 in
class ArithR<bits<6> func, string instr_asm, SDNode OpNode,
             InstrItinClass itin>:
  FR< func,
      0,
      (outs CPURegs:$dst),
      (ins CPURegs:$b, CPURegs:$c),
      !strconcat(instr_asm, "\t$dst, $b, $c"),
      [(set CPURegs:$dst, (OpNode CPURegs:$b, CPURegs:$c))], itin>;

// Arithmetic 2 register operands
class ArithI<bits<6> op, string instr_asm, SDNode OpNode,
             Operand Od, PatLeaf imm_type> :
  FI< op,
      (outs CPURegs:$dst),
      (ins CPURegs:$b, Od:$c),
      !strconcat(instr_asm, "\t$dst, $b, $c"),
      [(set CPURegs:$dst, (OpNode CPURegs:$b, imm_type:$c))], IIAlu>;

//  Logical
class LogicR<bits<6> func, string instr_asm, SDNode OpNode>:
  FR< func,
      0,
      (outs CPURegs:$dst),
      (ins CPURegs:$b, CPURegs:$c),
      !strconcat(instr_asm, "\t$dst, $b, $c"),
      [(set CPURegs:$dst, (OpNode CPURegs:$b, CPURegs:$c))], IIAlu>;

class LogicI<bits<6> op, string instr_asm, SDNode OpNode>:
  FI< op,
      (outs CPURegs:$dst),
      (ins CPURegs:$b, uimm16:$c),
      !strconcat(instr_asm, "\t$dst, $b, $c"),
      [(set CPURegs:$dst, (OpNode CPURegs:$b, immZExt16:$c))], IIAlu>;

class LogicNOR<bits<6> func, string instr_asm>:
  FR< func,
      0,
      (outs CPURegs:$dst),
      (ins CPURegs:$b, CPURegs:$c),
      !strconcat(instr_asm, "\t$dst, $b, $c"),
      [(set CPURegs:$dst, (not (or CPURegs:$b, CPURegs:$c)))], IIAlu>;

// Shifts
let rb = 0 in
class LogicR_shift_imm<bits<6> func, string instr_asm, SDNode OpNode>:
  FR< func,
      0,
      (outs CPURegs:$dst),
      (ins CPURegs:$b, opx:$c),
      !strconcat(instr_asm, "\t$dst, $b, $c"),
      [(set CPURegs:$dst, (OpNode CPURegs:$b, immZExt5:$c))], IIAlu>;

class LogicR_shift_reg<bits<6> func, string instr_asm, SDNode OpNode>:
  FR< func,
      0,
      (outs CPURegs:$dst),
      (ins CPURegs:$b, CPURegs:$c),
      !strconcat(instr_asm, "\t$dst, $b, $c"),
      [(set CPURegs:$dst, (OpNode CPURegs:$b, CPURegs:$c))], IIAlu>;

// Mov
let rb = 0 in
class Mov<bits<6> op, string instr_asm>:
  FR< op,
      0,
      (outs CPURegs:$dst),
      (ins CPURegs:$b),
      !strconcat(instr_asm, "\t$dst, $b"),
      [], IIAlu>;

// Lower Imediate
class Lower<bits<6> op, string instr_asm>:
  FI< op,
      (outs CPURegs:$dst),
      (ins simm16:$imm),
      !strconcat(instr_asm, "\t$dst, $imm"),
      [], IIAlu>;

class LowerU<bits<6> op, string instr_asm>:
  FI< op,
      (outs CPURegs:$dst),
      (ins uimm16:$imm),
      !strconcat(instr_asm, "\t$dst, $imm"),
      [], IIAlu>;

// Upper Imediate
class Upper<bits<6> op, string instr_asm>:
  FI< op,
      (outs CPURegs:$dst),
      (ins uimm16:$imm),
      !strconcat(instr_asm, "\t$dst, $imm"),
      [], IIAlu>;

// Memory Load/Store
let canFoldAsLoad = 1 in
class LoadM<bits<6> op, string instr_asm, PatFrag OpNode>:
  FI< op,
      (outs CPURegs:$dst),
      (ins mem:$addr),
      !strconcat(instr_asm, "\t$dst, $addr"),
      [(set CPURegs:$dst, (OpNode addr:$addr))], IILoad>;

class StoreM<bits<6> op, string instr_asm, PatFrag OpNode>:
  FI< op,
      (outs),
      (ins CPURegs:$dst, mem:$addr),
      !strconcat(instr_asm, "\t$dst, $addr"),
      [(OpNode CPURegs:$dst, addr:$addr)], IIStore>;

// Conditional Branch
let isBranch = 1, isTerminator=1 in {
class CBranch<bits<6> op, string instr_asm, PatFrag cond_op>:
  FI< op,
      (outs),
      (ins CPURegs:$a, CPURegs:$b, brtarget:$offset),
      !strconcat(instr_asm, "\t$a, $b, $offset"),
      [(brcond (cond_op CPURegs:$a, CPURegs:$b), bb:$offset)],
      IIBranch>;

class CBranchSwap<bits<6> op, string instr_asm, PatFrag cond_op>:
  FI< op,
      (outs),
      (ins CPURegs:$b, CPURegs:$a, brtarget:$offset),
      !strconcat(instr_asm, "\t$b, $a, $offset"),
      [(brcond (cond_op CPURegs:$b, CPURegs:$a), bb:$offset)],
      IIBranch>;

class CBranchZero<bits<6> op, string instr_asm, PatFrag cond_op>:
  FI< op,
      (outs),
      (ins CPURegs:$src, brtarget:$offset),
      !strconcat(instr_asm, "\t$src, $offset"),
      [(brcond (cond_op CPURegs:$src, 0), bb:$offset)],
      IIBranch>;
}

let ra=0, rb=0, isBranch = 1, isTerminator=1 in {
class Branch<bits<6> op, string instr_asm>:
  FI< op,
      (outs),
      (ins brtarget:$offset),
      !strconcat(instr_asm, "\t$offset"),
      [(br bb:$offset)],
      IIBranch>;
}

// SetCC
class SetCC_R<bits<6> func, string instr_asm,
      PatFrag cond_op>:
  FR< func,
      0,
      (outs CPURegs:$dst),
      (ins CPURegs:$b, CPURegs:$c),
      !strconcat(instr_asm, "\t$dst, $b, $c"),
      [(set CPURegs:$dst, (cond_op CPURegs:$b, CPURegs:$c))],
      IIAlu>;

class SetCC_RSwap<bits<6> func, string instr_asm,
      PatFrag cond_op>:
  FR< func,
      0,
      (outs CPURegs:$dst),
      (ins CPURegs:$c, CPURegs:$b),
      !strconcat(instr_asm, "\t$dst, $c, $b"),
      [(set CPURegs:$dst, (cond_op CPURegs:$c, CPURegs:$b))],
      IIAlu>;

class SetCC_I<bits<6> op, string instr_asm, PatFrag cond_op,
      Operand Od, PatLeaf imm_type>:
  FI< op,
      (outs CPURegs:$dst),
      (ins CPURegs:$b, Od:$c),
      !strconcat(instr_asm, "\t$dst, $b, $c"),
      [(set CPURegs:$dst, (cond_op CPURegs:$b, imm_type:$c))],
      IIAlu>;

// Unconditional branch
let isBranch=1, isTerminator=1, isBarrier=1 in
class JumpFJ<bits<6> op, string instr_asm>:
  FJ< op,
      (outs),
      (ins brtarget:$target),
      !strconcat(instr_asm, "\t$target"),
      [(br bb:$target)], IIBranch>;

let isBranch=1, isTerminator=1, isBarrier=1, rc=0 in
class JumpFR<bits<6> func, string instr_asm>:
  FR< func,
      0,
      (outs),
      (ins CPURegs:$target),
      !strconcat(instr_asm, "\t$target"),
      [(brind CPURegs:$target)], IIBranch>;

// Call
let isCall=1,
  // All calls clobber the non-callee saved registers...
  Defs = [AT, V0, V1, A0, A1, A2, A3, T0, T1, T2, T3, T4, T5, T6, T7 ], Uses = [GP] in {
  class Call<bits<6> op, string instr_asm>:
    FJ< op,
        (outs),
        (ins calltarget:$target, variable_ops),
        !strconcat(instr_asm, "\t$target"),
        [(Nios2Call imm:$target)], IIBranch>;

  let rc=31 in
  class CallReg<bits<6> func, string instr_asm>:
    FR< func,
        0,
        (outs),
        (ins CPURegs:$ra, variable_ops),
        !strconcat(instr_asm, "\t$ra"),
        [(Nios2Call CPURegs:$ra)], IIBranch>;
}

class EffectiveAddress<string instr_asm> :
  FI<0x04,
     (outs CPURegs:$dst),
     (ins mem:$addr),
     instr_asm,
     [(set CPURegs:$dst, addr:$addr)], IIAlu>;

//===----------------------------------------------------------------------===//
// Pseudo instructions
//===----------------------------------------------------------------------===//

// As stack alignment is always done with addi, we need a 16-bit immediate
let Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN : Nios2Pseudo<(outs), (ins simm16:$amt),
                                  "!ADJCALLSTACKDOWN $amt",
                                  [(callseq_start imm:$amt)]>;
def ADJCALLSTACKUP   : Nios2Pseudo<(outs), (ins simm16:$amt1, simm16:$amt2),
                                  "!ADJCALLSTACKUP $amt1",
                                  [(callseq_end imm:$amt1, imm:$amt2)]>;
}

// When handling PIC code the assembler needs .cpload and .cprestore
// directives. If the real instructions corresponding these directives
// are used, we have the same behavior, but get also a bunch of warnings
// from the assembler.
def CPLOAD : Nios2Pseudo<(outs), (ins CPURegs:$picreg), ".cpload\t$picreg", []>;
def CPRESTORE : Nios2Pseudo<(outs), (ins uimm16:$loc), ".cprestore\t$loc\n", []>;

// The supported Nios2 ISAs dont have any instruction close to the SELECT_CC 
// operation. The solution is to create a Nios2 pseudo SELECT_CC instruction
// (Nios2SelectCC), use LowerSELECT_CC to generate this instruction and finally 
// replace it for real supported nodes into EmitInstrWithCustomInserter
let usesCustomInserter = 1 in {
  class PseudoSelCC<RegisterClass RC, string asmstr>: 
    Nios2Pseudo<(outs RC:$dst), (ins CPURegs:$CmpRes, RC:$T, RC:$F), asmstr, 
    [(set RC:$dst, (Nios2SelectCC CPURegs:$CmpRes, RC:$T, RC:$F))]>;
}

def Select_CC : PseudoSelCC<CPURegs, "# Nios2Select_CC_i32">;

//===----------------------------------------------------------------------===//
// Instruction definition
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Nios2 Instructions
//===----------------------------------------------------------------------===//

def ADD     : ArithR<0x31, "add", add, IIAlu>;
def ADDi    : ArithI<0x04, "addi", add, simm16, immSExt16>;
def AND     : LogicR<0x0e, "and", and>;
def ANDHi   : Upper<0x2c, "andhi">;
def ANDi    : LogicI<0x0c, "andi", and>;
def BEQ     : CBranch<0x26, "beq", seteq>;
def BGE     : CBranch<0x0e, "bge", setge>;
def BGEU    : CBranch<0x2e, "bgeu", setuge>;
def BGT     : CBranchSwap<0x16, "bgt", setgt>;
def BGTU    : CBranchSwap<0x36, "bgtu", setgt>;
def BLE     : CBranchSwap<0x0e, "ble", setle>;
def BLEU    : CBranchSwap<0x2e, "bleu", setule>;
def BLT     : CBranch<0x16, "blt", setlt>;
def BLTU    : CBranch<0x36, "bltu", setlt>;
def BNE     : CBranch<0x1e, "bne", setne>;
def BR      : Branch<0x06, "br">;
def CALL    : Call<0x00, "call">;
def CALLR   : CallReg<0x1d, "callr">;

def CMPEQ   : SetCC_R<0x20, "cmpeq", seteq>;
def CMPEQi  : SetCC_I<0x20, "cmpeqi", seteq, simm16, immSExt16>;

def CMPGE   : SetCC_R<0x08, "cmpge", setge>;
def CMPGEi  : SetCC_I<0x08, "cmpgei", setge, simm16, immSExt16>;
def CMPGEu  : SetCC_R<0x28, "cmpgeu", setuge>;
def CMPGEiu : SetCC_I<0x28, "cmpgeui", setuge, uimm16, immZExt16>;

def CMPGT   : SetCC_RSwap<0x10, "cmpgt", setgt>;
def CMPGTu  : SetCC_RSwap<0x30, "cmpgtu", setugt>;

def CMPLE   : SetCC_RSwap<0x08, "cmple", setle>;
def CMPLEu  : SetCC_RSwap<0x28, "cmpleu", setule>;

def CMPLT   : SetCC_R<0x10, "cmplt", setlt>;
def CMPLTi  : SetCC_I<0x10, "cmplti", setlt, simm16, immSExt16>;
def CMPLTu  : SetCC_R<0x30, "cmpltu", setult>;
def CMPLTiu : SetCC_I<0x30, "cmpltui", setult, uimm16, immZExt16>;

def CMPNE   : SetCC_R<0x18, "cmpne", setne>;
def CMPNEi  : SetCC_I<0x18, "cmpnei", setne, simm16, immSExt16>;

// RICH: custom

let Predicates = [HasDiv] in {
  def DIV     : ArithR<0x25, "div", sdiv, IIAlu>;
  def DIVu    : ArithR<0x24, "divu", udiv, IIAlu>;
}

let isReturn=1, isTerminator=1,
    isBarrier=1, hasCtrlDep=1, ra=0x1d, rb=0, rc=0 in
  def ERET  : FR <0x01, 0, (outs), (ins),
                  "eret", [(Nios2ERet)], IIBranch>;

// RICH: flushd
// RICH: flushda
// RICH: flushi
// RICH: flushp
// RICH: initd
// RICH: initda
// RICH: initi

def JMPi    : JumpFJ<0x01, "jmpi">;
def JMP     : JumpFR<0x0d, "jmp">;

def LDB     : LoadM<0x07, "ldb", sextloadi8>;
def LDBio   : LoadM<0x27, "ldbio", sextloadi8>;
def LDBu    : LoadM<0x03, "ldbu", zextloadi8>;
def LDBuio  : LoadM<0x23, "ldbuio", zextloadi8>;
def LDH     : LoadM<0x0f, "ldh",  sextloadi16>;
def LDHio   : LoadM<0x2f, "ldhio",  sextloadi16>;
def LDHu    : LoadM<0x0b, "ldhu", zextloadi16>;
def LDHuio  : LoadM<0x2b, "ldhuio", zextloadi16>;
def LDW     : LoadM<0x17, "ldw",  load>;
def LDWio   : LoadM<0x37, "ldwio",  load>;

def MOV     : Mov<0x31, "mov">;
def MOVi    : Lower<0x04, "movi">;
def MOVui   : LowerU<0x14, "movui">;

def MOVHi   : Upper<0x34, "movhi">;

let Predicates = [HasMul] in {
  def MUL     : ArithR<0x27, "mul", mul, IIAlu>;
  def MULi    : ArithI<0x24, "muli", mul, simm16, immSExt16>;
}

let Predicates = [HasMulx] in {
  def MULx    : ArithR<0x1f, "mulxss", mulhs, IIAlu>;
  def MULxu   : ArithR<0x07, "mulxuu", mulhu, IIAlu>;
}

let ra=0, rb=0, rc=0 in
  def NOP   : FR<0x31, 0, (outs), (ins), "nop", [], IIAlu>;

def NOR     : LogicNOR<0x06, "nor">;
def OR      : LogicR<0x16, "or",  or>;
def ORHi    : Upper<0x34, "orhi">;
def ORi     : LogicI<0x14, "ori",  or>;

let isReturn=1, isTerminator=1,
    isBarrier=1, hasCtrlDep=1, ra=0x1f, rb=0, rc=0 in
  def RET   : FR <0x05, 0, (outs), (ins),
                  "ret", [(Nios2Ret)], IIBranch>;

def SLL     : LogicR_shift_reg<0x13, "sll", shl>;
def SLLi    : LogicR_shift_imm<0x12, "slli", shl>;
def SRA     : LogicR_shift_reg<0x3b, "sra", sra>;
def SRAi    : LogicR_shift_imm<0x3a, "srai", sra>;
def SRL     : LogicR_shift_reg<0x1b, "srl", srl>;
def SRLi    : LogicR_shift_imm<0x1a, "srli", srl>;

def STB     : StoreM<0x05, "stb", truncstorei8>;
def STH     : StoreM<0x0d, "sth", truncstorei16>;
def STW     : StoreM<0x15, "stw", store>;

def SUB     : ArithR<0x39, "sub", sub, IIAlu>;
// RICH: SUBi

def XOR     : LogicR<0x1e, "xor", xor>;
def XORHi   : Upper<0x3c, "xorhi">;
def XORi    : LogicI<0x1c, "xori",  xor>;

// FrameIndexes are legalized when they are operands from load/store
// instructions. The same does not happen for stack address copies, so an
// add op with mem ComplexPattern is used so the stack address copy
// can be matched.
def LEA_ADDi : EffectiveAddress<"addi\t$dst, ${addr:stackloc}">;

//===----------------------------------------------------------------------===//
//  Arbitrary patterns that map to one or more instructions
//===----------------------------------------------------------------------===//

// Small immediates
def : Pat<(i32 immSExt16:$in),
          (MOVi imm:$in)>;
def : Pat<(i32 immZExt16:$in),
          (MOVui imm:$in)>;

// Arbitrary immediates
def : Pat<(i32 imm:$imm),
          (ORi (MOVHi (HI16 imm:$imm)), (LO16 imm:$imm))>;

// Carry patterns
def : Pat<(subc CPURegs:$lhs, CPURegs:$rhs),
          (SUB CPURegs:$lhs, CPURegs:$rhs)>;
def : Pat<(addc CPURegs:$lhs, CPURegs:$rhs),
          (ADD CPURegs:$lhs, CPURegs:$rhs)>;
def : Pat<(addc  CPURegs:$src, imm:$imm),
          (ADDi CPURegs:$src, imm:$imm)>;

// Call
def : Pat<(Nios2Call (i32 tglobaladdr:$dst)),
          (CALL tglobaladdr:$dst)>;
def : Pat<(Nios2Call (i32 texternalsym:$dst)),
          (CALL texternalsym:$dst)>;
def : Pat<(Nios2Call CPURegs:$dst),
          (CALLR CPURegs:$dst)>;

// hi/lo relocs
def : Pat<(Nios2Hi tglobaladdr:$in), (MOVHi tglobaladdr:$in)>;
def : Pat<(add CPURegs:$hi, (Nios2Lo tglobaladdr:$lo)),
          (ORi CPURegs:$hi, tglobaladdr:$lo)>;

def : Pat<(Nios2Hi tjumptable:$in), (MOVHi tjumptable:$in)>;
def : Pat<(add CPURegs:$hi, (Nios2Lo tjumptable:$lo)),
          (ORi CPURegs:$hi, tjumptable:$lo)>;

def : Pat<(Nios2Hi tconstpool:$in), (MOVHi tconstpool:$in)>;
def : Pat<(add CPURegs:$hi, (Nios2Lo tconstpool:$lo)),
          (ORi CPURegs:$hi, tconstpool:$lo)>;

// gp relocs
def : Pat<(add CPURegs:$gp, (Nios2GPRel tglobaladdr:$in)), 
          (ADDi CPURegs:$gp, tglobaladdr:$in)>;
def : Pat<(add CPURegs:$gp, (Nios2GPRel tconstpool:$in)), 
          (ADDi CPURegs:$gp, tconstpool:$in)>;

// Nios2 does not have "not", so we expand our way
def : Pat<(not CPURegs:$in),
          (NOR CPURegs:$in, ZERO)>;

// extended load and stores
def : Pat<(extloadi1  addr:$src), (LDBu addr:$src)>;
def : Pat<(extloadi8  addr:$src), (LDBu addr:$src)>;
def : Pat<(extloadi16 addr:$src), (LDHu addr:$src)>;

// peepholes
def : Pat<(truncstorei8 (i32 0), addr:$dst), (STB ZERO, addr:$dst)>;
def : Pat<(truncstorei16 (i32 0), addr:$dst), (STH ZERO, addr:$dst)>;
def : Pat<(store (i32 0), addr:$dst), (STW ZERO, addr:$dst)>;

// brcond patterns
def : Pat<(brcond (setne CPURegs:$lhs, 0), bb:$dst),
          (BNE CPURegs:$lhs, ZERO, bb:$dst)>;
def : Pat<(brcond (seteq CPURegs:$lhs, 0), bb:$dst),
          (BEQ CPURegs:$lhs, ZERO, bb:$dst)>;
def : Pat<(brcond (setge CPURegs:$lhs, CPURegs:$rhs), bb:$dst),
          (BGE CPURegs:$lhs, CPURegs:$rhs, bb:$dst)>;
def : Pat<(brcond (setgt CPURegs:$lhs, CPURegs:$rhs), bb:$dst),
          (BGT CPURegs:$lhs, CPURegs:$rhs, bb:$dst)>;
def : Pat<(brcond (setle CPURegs:$lhs, CPURegs:$rhs), bb:$dst),
          (BLE CPURegs:$lhs, CPURegs:$rhs, bb:$dst)>;
def : Pat<(brcond (setlt CPURegs:$lhs, CPURegs:$rhs), bb:$dst),
          (BLT CPURegs:$lhs, CPURegs:$rhs, bb:$dst)>;

def : Pat<(brcond CPURegs:$cond, bb:$dst),
          (BNE CPURegs:$cond, ZERO, bb:$dst)>;

// setcc patterns
def : Pat<(seteq CPURegs:$lhs, CPURegs:$rhs),
          (CMPEQ CPURegs:$lhs, CPURegs:$rhs)>;
def : Pat<(seteq CPURegs:$lhs, immSExt16:$rhs),
          (CMPEQi CPURegs:$lhs, immSExt16:$rhs)>;

def : Pat<(setge CPURegs:$lhs, CPURegs:$rhs),
          (CMPEQ CPURegs:$lhs, CPURegs:$rhs)>;
def : Pat<(setge CPURegs:$lhs, immSExt16:$rhs),
          (CMPGEi CPURegs:$lhs, immSExt16:$rhs)>;
def : Pat<(setuge CPURegs:$lhs, CPURegs:$rhs),
          (CMPGEu CPURegs:$lhs, CPURegs:$rhs)>;
def : Pat<(setuge CPURegs:$lhs, immZExt16:$rhs),
          (CMPGEiu CPURegs:$lhs, immZExt16:$rhs)>;

def : Pat<(setgt CPURegs:$lhs, CPURegs:$rhs),
          (CMPGT CPURegs:$lhs, CPURegs:$rhs)>;
def : Pat<(setugt CPURegs:$lhs, CPURegs:$rhs),
          (CMPGTu CPURegs:$lhs, CPURegs:$rhs)>;

def : Pat<(setle CPURegs:$lhs, CPURegs:$rhs),
          (CMPLE CPURegs:$lhs, CPURegs:$rhs)>;
def : Pat<(setule CPURegs:$lhs, immZExt16:$rhs),
          (CMPLEu CPURegs:$lhs, immZExt16:$rhs)>;

def : Pat<(setlt CPURegs:$lhs, CPURegs:$rhs),
          (CMPLT CPURegs:$lhs, CPURegs:$rhs)>;
def : Pat<(setlt CPURegs:$lhs, immSExt16:$rhs),
          (CMPLTi CPURegs:$lhs, immSExt16:$rhs)>;
def : Pat<(setult CPURegs:$lhs, CPURegs:$rhs),
          (CMPLTu CPURegs:$lhs, CPURegs:$rhs)>;
def : Pat<(setult CPURegs:$lhs, immZExt16:$rhs),
          (CMPLTiu CPURegs:$lhs, immZExt16:$rhs)>;

def : Pat<(setne CPURegs:$lhs, CPURegs:$rhs),
          (CMPNE CPURegs:$lhs, CPURegs:$rhs)>;
def : Pat<(setne CPURegs:$lhs, immSExt16:$rhs),
          (CMPNEi CPURegs:$lhs, immSExt16:$rhs)>;
