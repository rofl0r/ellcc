//===-- Nios2ISelLowering.cpp - Nios2 DAG Lowering Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Nios2 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "nios2-lower"
#include "Nios2ISelLowering.h"
#include "Nios2MachineFunction.h"
#include "Nios2TargetMachine.h"
#include "Nios2TargetObjectFile.h"
#include "Nios2Subtarget.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/GlobalVariable.h"
#include "llvm/Intrinsics.h"
#include "llvm/CallingConv.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include <queue>
#include <set>

using namespace llvm;

const char *Nios2TargetLowering::getTargetNodeName(unsigned Opcode) const 
{
    switch (Opcode) 
    {
        case Nios2ISD::Call       : return "Nios2ISD::Call";
        case Nios2ISD::Hi         : return "Nios2ISD::Hi";
        case Nios2ISD::Lo         : return "Nios2ISD::Lo";
        case Nios2ISD::GPRel      : return "Nios2ISD::GPRel";
        case Nios2ISD::Ret        : return "Nios2ISD::Ret";
        case Nios2ISD::CMov       : return "Nios2ISD::CMov";
        case Nios2ISD::SelectCC   : return "Nios2ISD::SelectCC";
        case Nios2ISD::FPSelectCC : return "Nios2ISD::FPSelectCC";
        case Nios2ISD::FPBrcond   : return "Nios2ISD::FPBrcond";
        case Nios2ISD::FPCmp      : return "Nios2ISD::FPCmp";
        default                  : return NULL;
    }
}

Nios2TargetLowering::Nios2TargetLowering(Nios2TargetMachine &TM)
    : TargetLowering(TM, new Nios2TargetObjectFile()) 
{
    Subtarget = &TM.getSubtarget<Nios2Subtarget>();

    // Nios2 does not have i1 type, so use i32 for
    // setcc operations results (slt, sgt, ...). 
    setBooleanContents(ZeroOrOneBooleanContent);

    // Set up the register classes
    addRegisterClass(MVT::i32, Nios2::CPURegsRegisterClass);

    // Load extented operations for i1 types must be promoted 
    setLoadExtAction(ISD::EXTLOAD,  MVT::i1,  Promote);
    setLoadExtAction(ISD::ZEXTLOAD, MVT::i1,  Promote);
    setLoadExtAction(ISD::SEXTLOAD, MVT::i1,  Promote);

    // Nios2 doesn't have extending float->double load/store
    setLoadExtAction(ISD::EXTLOAD, MVT::f32, Expand);
    setTruncStoreAction(MVT::f64, MVT::f32, Expand);

    // Used by legalize types to correctly generate the setcc result. 
    // Without this, every float setcc comes with a AND/OR with the result, 
    // we don't want this, since the fpcmp result goes to a flag register, 
    // which is used implicitly by brcond and select operations.
    AddPromotedToType(ISD::SETCC, MVT::i1, MVT::i32);

    // Nios2 Custom Operations
    setOperationAction(ISD::GlobalAddress,      MVT::i32,   Custom);
    setOperationAction(ISD::GlobalTLSAddress,   MVT::i32,   Custom);
    setOperationAction(ISD::JumpTable,          MVT::i32,   Custom);
    setOperationAction(ISD::ConstantPool,       MVT::i32,   Custom);
    setOperationAction(ISD::SELECT,             MVT::f32,   Custom);
    setOperationAction(ISD::SELECT,             MVT::f64,   Custom);
    setOperationAction(ISD::SELECT,             MVT::i32,   Custom);
    setOperationAction(ISD::SETCC,              MVT::f32,   Custom);
    setOperationAction(ISD::BRCOND,             MVT::Other, Custom);
    setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32,   Custom);

    setOperationAction(ISD::VASTART,            MVT::Other, Custom);

    // Use the default implementation.
    setOperationAction(ISD::VAARG,              MVT::Other, Expand);
    setOperationAction(ISD::VACOPY,             MVT::Other, Expand);
    setOperationAction(ISD::VAEND,              MVT::Other, Expand);
    setOperationAction(ISD::SMUL_LOHI,          MVT::i32,   Expand);
    setOperationAction(ISD::UMUL_LOHI,          MVT::i32,   Expand);

    // We custom lower AND/OR to handle the case where the DAG contain 'ands/ors' 
    // with operands comming from setcc fp comparions. This is necessary since 
    // the result from these setcc are in a flag registers (FCR31).
    setOperationAction(ISD::AND,                MVT::i32,   Custom);
    setOperationAction(ISD::OR,                 MVT::i32,   Custom);

    // Operations not directly supported by Nios2.
    setOperationAction(ISD::BR_JT,              MVT::Other, Expand);
    setOperationAction(ISD::BR_CC,              MVT::Other, Expand);
    setOperationAction(ISD::SELECT_CC,          MVT::Other, Expand);
    setOperationAction(ISD::UINT_TO_FP,         MVT::i32,   Expand);
    setOperationAction(ISD::FP_TO_UINT,         MVT::i32,   Expand);
    setOperationAction(ISD::CTPOP,              MVT::i32,   Expand);
    setOperationAction(ISD::CTTZ,               MVT::i32,   Expand);
    setOperationAction(ISD::ROTL,               MVT::i32,   Expand);
    setOperationAction(ISD::SHL_PARTS,          MVT::i32,   Expand);
    setOperationAction(ISD::SRA_PARTS,          MVT::i32,   Expand);
    setOperationAction(ISD::SRL_PARTS,          MVT::i32,   Expand);
    setOperationAction(ISD::FCOPYSIGN,          MVT::f32,   Expand);
    setOperationAction(ISD::FCOPYSIGN,          MVT::f64,   Expand);
    setOperationAction(ISD::BIT_CONVERT,        MVT::f32  , Expand);
    setOperationAction(ISD::BIT_CONVERT,        MVT::i32  , Expand);

    setOperationAction(ISD::EH_LABEL,           MVT::Other, Expand);

    // Use the default for now
    setOperationAction(ISD::STACKSAVE,          MVT::Other, Expand);
    setOperationAction(ISD::STACKRESTORE,       MVT::Other, Expand);
    setOperationAction(ISD::MEMBARRIER,         MVT::Other, Expand);

    setOperationAction(ISD::SELECT_CC,          MVT::f64,   Expand);

    setOperationAction(ISD::SIGN_EXTEND_INREG,  MVT::i1,    Expand);
    setOperationAction(ISD::SIGN_EXTEND_INREG,  MVT::i8,    Expand);
    setOperationAction(ISD::SIGN_EXTEND_INREG,  MVT::i16,   Expand);
    setOperationAction(ISD::SIGN_EXTEND_INREG,  MVT::i32,   Expand);

    setOperationAction(ISD::CTLZ,               MVT::i32,   Expand);

    setOperationAction(ISD::BSWAP,              MVT::i32,   Expand);

    if (!Subtarget->hasMul())
        setOperationAction(ISD::MUL, MVT::i32, Expand);

    if (!Subtarget->hasMulx()) {
        setOperationAction(ISD::MULHU, MVT::i32, Expand);
        setOperationAction(ISD::MULHS, MVT::i32, Expand);
    }

    if (!Subtarget->hasDiv()) {
        setOperationAction(ISD::UDIV, MVT::i32, Expand);
        setOperationAction(ISD::SDIV, MVT::i32, Expand);
        setOperationAction(ISD::SREM, MVT::i32, Expand);
        setOperationAction(ISD::UREM, MVT::i32, Expand);
        setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
        setOperationAction(ISD::UDIVREM, MVT::i32, Expand);
    } else {
        setOperationAction(ISD::SREM, MVT::i32, Custom);
        setOperationAction(ISD::UREM, MVT::i32, Custom);
    }

    setStackPointerRegisterToSaveRestore(Nios2::SP);
    computeRegisterProperties();
}

MVT::SimpleValueType Nios2TargetLowering::getSetCCResultType(EVT VT) const {
    return MVT::i32;
}

/// getFunctionAlignment - Return the Log2 alignment of this function.
unsigned Nios2TargetLowering::getFunctionAlignment(const Function *) const {
  return 2;
}


SDValue Nios2TargetLowering::
LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
    switch (Op.getOpcode()) 
    {
        case ISD::AND:                return LowerANDOR(Op, DAG);
        case ISD::BRCOND:             return LowerBRCOND(Op, DAG);
        case ISD::ConstantPool:       return LowerConstantPool(Op, DAG);
        case ISD::DYNAMIC_STACKALLOC: return LowerDYNAMIC_STACKALLOC(Op, DAG);
        case ISD::GlobalAddress:      return LowerGlobalAddress(Op, DAG);
        case ISD::GlobalTLSAddress:   return LowerGlobalTLSAddress(Op, DAG);
        case ISD::JumpTable:          return LowerJumpTable(Op, DAG);
        case ISD::OR:                 return LowerANDOR(Op, DAG);
        case ISD::SELECT:             return LowerSELECT(Op, DAG);
        case ISD::SETCC:              return LowerSETCC(Op, DAG);
        case ISD::VASTART:            return LowerVASTART(Op, DAG);
        case ISD::SREM:
        case ISD::UREM:               return ExpandREM(Op, DAG);
    }
    return SDValue();
}

//===----------------------------------------------------------------------===//
//  Lower helper functions
//===----------------------------------------------------------------------===//

// AddLiveIn - This helper function adds the specified physical register to the
// MachineFunction as a live in value.  It also creates a corresponding
// virtual register for it.
    static unsigned
AddLiveIn(MachineFunction &MF, unsigned PReg, TargetRegisterClass *RC) 
{
    assert(RC->contains(PReg) && "Not the correct regclass!");
    unsigned VReg = MF.getRegInfo().createVirtualRegister(RC);
    MF.getRegInfo().addLiveIn(PReg, VReg);
    return VReg;
}

MachineBasicBlock *
Nios2TargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
                                                 MachineBasicBlock *BB) const
{
    const TargetInstrInfo *TII = getTargetMachine().getInstrInfo();
    bool isFPCmp = false;
    DebugLoc dl = MI->getDebugLoc();

    switch (MI->getOpcode()) {
        default:
            llvm_unreachable("Unexpected instr type to insert");
           isFPCmp = true; // FALL THROUGH
        case Nios2::Select_CC:
        {
            // To "insert" a SELECT_CC instruction, we actually have to insert the
            // diamond control-flow pattern.  The incoming instruction knows the
            // destination vreg to set, the condition code register to branch on, the
            // true/false values to select between, and a branch opcode to use.
            const BasicBlock *LLVM_BB = BB->getBasicBlock();
            MachineFunction::iterator It = BB;
            ++It;

            //  thisMBB:
            //  ...
            //   TrueVal = ...
            //   setcc r1, r2, r3
            //   bNE   r1, r0, copy1MBB
            //   fallthrough --> copy0MBB
            MachineBasicBlock *thisMBB  = BB;
            MachineFunction *F = BB->getParent();
            MachineBasicBlock *copy0MBB = F->CreateMachineBasicBlock(LLVM_BB);
            MachineBasicBlock *sinkMBB  = F->CreateMachineBasicBlock(LLVM_BB);

            // Emit the right instruction according to the type of the operands compared
            BuildMI(BB, dl, TII->get(Nios2::BNE)).addReg(MI->getOperand(1).getReg())
                .addReg(Nios2::ZERO).addMBB(sinkMBB);

            F->insert(It, copy0MBB);
            F->insert(It, sinkMBB);
            // Update machine-CFG edges by first adding all successors of the current
            // block to the new block which will contain the Phi node for the select.
            for(MachineBasicBlock::succ_iterator i = BB->succ_begin(),
                    e = BB->succ_end(); i != e; ++i)
                sinkMBB->addSuccessor(*i);
            // Next, remove all successors of the current block, and add the true
            // and fallthrough blocks as its successors.
            while(!BB->succ_empty())
                BB->removeSuccessor(BB->succ_begin());
            BB->addSuccessor(copy0MBB);
            BB->addSuccessor(sinkMBB);

            //  copy0MBB:
            //   %FalseValue = ...
            //   # fallthrough to sinkMBB
            BB = copy0MBB;

            // Update machine-CFG edges
            BB->addSuccessor(sinkMBB);

            //  sinkMBB:
            //   %Result = phi [ %FalseValue, copy0MBB ], [ %TrueValue, thisMBB ]
            //  ...
            BB = sinkMBB;
            BuildMI(BB, dl, TII->get(Nios2::PHI), MI->getOperand(0).getReg())
                .addReg(MI->getOperand(2).getReg()).addMBB(copy0MBB)
                .addReg(MI->getOperand(3).getReg()).addMBB(thisMBB);

            F->DeleteMachineInstr(MI);   // The pseudo instruction is gone now.
            return BB;
        }
    }
}

//===----------------------------------------------------------------------===//
//  Misc Lower Operation implementation
//===----------------------------------------------------------------------===//

SDValue Nios2TargetLowering::
LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const
{
    SDValue Chain = Op.getOperand(0);
    SDValue Size = Op.getOperand(1);
    DebugLoc dl = Op.getDebugLoc();

    // Get a reference from Nios2 stack pointer
    SDValue StackPointer = DAG.getCopyFromReg(Chain, dl, Nios2::SP, MVT::i32);

    // Subtract the dynamic size from the actual stack size to
    // obtain the new stack size.
    SDValue Sub = DAG.getNode(ISD::SUB, dl, MVT::i32, StackPointer, Size);

    // The Sub result contains the new stack start address, so it 
    // must be placed in the stack pointer register.
    Chain = DAG.getCopyToReg(StackPointer.getValue(1), dl, Nios2::SP, Sub);

    // This node always has two return values: a new stack pointer 
    // value and a chain
    SDValue Ops[2] = { Sub, Chain };
    return DAG.getMergeValues(Ops, 2, dl);
}

SDValue Nios2TargetLowering::
LowerANDOR(SDValue Op, SelectionDAG &DAG) const
{
    SDValue LHS   = Op.getOperand(0);
    SDValue RHS   = Op.getOperand(1);
    DebugLoc dl   = Op.getDebugLoc();

    if (LHS.getOpcode() != Nios2ISD::FPCmp || RHS.getOpcode() != Nios2ISD::FPCmp)
        return Op;

    SDValue True  = DAG.getConstant(1, MVT::i32);
    SDValue False = DAG.getConstant(0, MVT::i32);

    SDValue LSEL = DAG.getNode(Nios2ISD::FPSelectCC, dl, True.getValueType(), 
            LHS, True, False, LHS.getOperand(2));
    SDValue RSEL = DAG.getNode(Nios2ISD::FPSelectCC, dl, True.getValueType(), 
            RHS, True, False, RHS.getOperand(2));

    return DAG.getNode(Op.getOpcode(), dl, MVT::i32, LSEL, RSEL);
}

SDValue Nios2TargetLowering::
LowerBRCOND(SDValue Op, SelectionDAG &DAG) const
{
    return Op;
}

SDValue Nios2TargetLowering::
LowerSETCC(SDValue Op, SelectionDAG &DAG) const
{
    // The operands to this are the left and right operands to compare (ops #0, 
    // and #1) and the condition code to compare them with (op #2) as a 
    // CondCodeSDNode.
    SDValue LHS = Op.getOperand(0); 
    SDValue RHS = Op.getOperand(1); 
    DebugLoc dl = Op.getDebugLoc();

    ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(2))->get();

    return DAG.getNode(Nios2ISD::FPCmp, dl, Op.getValueType(), LHS, RHS, 
            DAG.getConstant(CC, MVT::i32));
}

SDValue Nios2TargetLowering::
LowerVASTART(SDValue Op, SelectionDAG &DAG) const {
  MachineFunction &MF = DAG.getMachineFunction();
  Nios2FunctionInfo *FuncInfo = MF.getInfo<Nios2FunctionInfo>();

  DebugLoc dl = Op.getDebugLoc();
  SDValue FI = DAG.getFrameIndex(FuncInfo->getVarArgsFrameIndex(),
                                 getPointerTy());

  // vastart just stores the address of the VarArgsFrameIndex slot into the
  // memory location argument.
  const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
  return DAG.getStore(Op.getOperand(0), dl, FI, Op.getOperand(1), SV, 0,
                      false, false, 0);
}

SDValue Nios2TargetLowering::
ExpandREM(SDValue Op, SelectionDAG &DAG) const
{
    assert(Op.getValueType() == MVT::i32 &&
            (Op.getOpcode() == ISD::SREM || Op.getOpcode() == ISD::UREM) &&
            "Unknown operand to lower!");

    // A remainder becomes:
    // div t1, op1, op2
    // mul t2, t1, op2
    // sub res, op1, t2
    DebugLoc dl = Op.getDebugLoc();
    SDValue DIV = DAG.getNode(Op.getOpcode() == ISD::SREM ? ISD::SDIV : ISD::UDIV,
                              dl, MVT::i32,
                              Op.getOperand(0), Op.getOperand(1));
    SDValue MUL = DAG.getNode(ISD::MUL, dl, MVT::i32, DIV, Op.getOperand(1));
    return DAG.getNode(ISD::SUB, dl, MVT::i32, Op.getOperand(0), MUL);
}

SDValue Nios2TargetLowering::
LowerSELECT(SDValue Op, SelectionDAG &DAG) const
{
    SDValue Cond  = Op.getOperand(0); 
    SDValue True  = Op.getOperand(1);
    SDValue False = Op.getOperand(2);
    DebugLoc dl = Op.getDebugLoc();

    // if the incomming condition comes from a integer compare, the select 
    // operation must be SelectCC or a conditional move if the subtarget 
    // supports it.
    if (Cond.getOpcode() != Nios2ISD::FPCmp) {
        return DAG.getNode(Nios2ISD::SelectCC, dl, True.getValueType(), 
                Cond, True, False);
    }

    // if the incomming condition comes from fpcmp, the select
    // operation must use FPSelectCC.
    SDValue CCNode = Cond.getOperand(2);
    return DAG.getNode(Nios2ISD::FPSelectCC, dl, True.getValueType(), 
                       Cond, True, False, CCNode);
}

SDValue Nios2TargetLowering::LowerGlobalAddress(SDValue Op,
                                                SelectionDAG &DAG) const
{
    // FIXME there isn't actually debug info here
    DebugLoc dl = Op.getDebugLoc();
    const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();

    SDVTList VTs = DAG.getVTList(MVT::i32);
    // %hi/%lo relocation
    SDValue GA = DAG.getTargetGlobalAddress(GV, MVT::i32, 0, 
                                            Nios2II::MO_ABS_HILO);
    SDValue HiPart = DAG.getNode(Nios2ISD::Hi, dl, VTs, &GA, 1);
    SDValue Lo = DAG.getNode(Nios2ISD::Lo, dl, MVT::i32, GA);
    return DAG.getNode(ISD::ADD, dl, MVT::i32, HiPart, Lo);
}

SDValue Nios2TargetLowering::
LowerGlobalTLSAddress(SDValue Op, SelectionDAG &DAG) const
{
    llvm_unreachable("TLS not implemented for NIOS2.");
    return SDValue(); // Not reached
}

SDValue Nios2TargetLowering::
LowerJumpTable(SDValue Op, SelectionDAG &DAG) const
{
    SDValue ResNode;
    SDValue HiPart; 
    // FIXME there isn't actually debug info here
    DebugLoc dl = Op.getDebugLoc();
    bool IsPIC = getTargetMachine().getRelocationModel() == Reloc::PIC_;
    unsigned char OpFlag = IsPIC ? Nios2II::MO_GOT : Nios2II::MO_ABS_HILO;

    EVT PtrVT = Op.getValueType();
    JumpTableSDNode *JT  = cast<JumpTableSDNode>(Op);
    SDValue JTI = DAG.getTargetJumpTable(JT->getIndex(), PtrVT, OpFlag);

    if (IsPIC) {
        SDVTList VTs = DAG.getVTList(MVT::i32);
        SDValue Ops[] = { JTI };
        HiPart = DAG.getNode(Nios2ISD::Hi, dl, DAG.getVTList(MVT::i32), Ops, 1);
    } else // Emit Load from Global Pointer
        HiPart = DAG.getLoad(MVT::i32, dl, DAG.getEntryNode(), JTI, NULL, 0,
                             false, false, 0);

    SDValue Lo = DAG.getNode(Nios2ISD::Lo, dl, MVT::i32, JTI);
    ResNode = DAG.getNode(ISD::ADD, dl, MVT::i32, HiPart, Lo);

    return ResNode;
}

SDValue Nios2TargetLowering::
LowerConstantPool(SDValue Op, SelectionDAG &DAG) const
{
    SDValue ResNode;
    ConstantPoolSDNode *N = cast<ConstantPoolSDNode>(Op);
    const Constant *C = N->getConstVal();
    // FIXME there isn't actually debug info here
    DebugLoc dl = Op.getDebugLoc();

    // gp_rel relocation
    // FIXME: we should reference the constant pool using small data sections, 
    // but the asm printer currently doens't support this feature without
    // hacking it. This feature should come soon so we can uncomment the 
    // stuff below.
    //if (IsInSmallSection(C->getType())) {
    //  SDValue GPRelNode = DAG.getNode(Nios2ISD::GPRel, MVT::i32, CP);
    //  SDValue GOT = DAG.getNode(ISD::GLOBAL_OFFSET_TABLE, MVT::i32);
    //  ResNode = DAG.getNode(ISD::ADD, MVT::i32, GOT, GPRelNode); 
    SDValue CP = DAG.getTargetConstantPool(C, MVT::i32, N->getAlignment(),
                                           N->getOffset(), Nios2II::MO_ABS_HILO);
    SDValue HiPart = DAG.getNode(Nios2ISD::Hi, dl, MVT::i32, CP);
    SDValue Lo = DAG.getNode(Nios2ISD::Lo, dl, MVT::i32, CP);
    ResNode = DAG.getNode(ISD::ADD, dl, MVT::i32, HiPart, Lo);

    return ResNode;
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "Nios2GenCallingConv.inc"

//===----------------------------------------------------------------------===//
//                  CALL Calling Convention Implementation
//===----------------------------------------------------------------------===//

/// LowerCall - functions arguments are copied from virtual
/// regs to (physical regs)/(stack frame), CALLSEQ_START and
/// CALLSEQ_END are emitted.
/// TODO: isVarArg, isTailCall.
SDValue Nios2TargetLowering::
LowerCall(SDValue Chain, SDValue Callee,
          CallingConv::ID CallConv, bool isVarArg,
          bool &isTailCall,
          const SmallVectorImpl<ISD::OutputArg> &Outs,
          const SmallVectorImpl<ISD::InputArg> &Ins,
          DebugLoc dl, SelectionDAG &DAG,
          SmallVectorImpl<SDValue> &InVals) const
{
  // Nios2 target does not yet support tail call optimization.
  isTailCall = false;

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  bool IsPIC = getTargetMachine().getRelocationModel() == Reloc::PIC_;

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, getTargetMachine(), ArgLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeCallOperands(Outs, CC_Nios2);
  
  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getNextStackOffset();
  Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(NumBytes, true));

  // With EABI is it possible to have 16 args on registers.
  SmallVector<std::pair<unsigned, SDValue>, 16> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;

  // First/LastArgStackLoc contains the first/last 
  // "at stack" argument location.
  int LastArgStackLoc = 0;
  unsigned FirstStackArgLoc = 0;

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    SDValue Arg = Outs[i].Val;
    CCValAssign &VA = ArgLocs[i];

    // Promote the value if needed.
    switch (VA.getLocInfo()) {
    default: llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full: 
      break;
    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, dl, VA.getLocVT(), Arg);
      break;
    }
    
    // Arguments that can be passed on register must be kept at 
    // RegsToPass vector
    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
      continue;
    }
    
    // Register can't get to this point...
    assert(VA.isMemLoc());
    
    // Create the frame index object for this incoming parameter
    // This guarantees that when allocating Local Area the firsts
    // 16 bytes which are alwayes reserved won't be overwritten
    // if O32 ABI is used. For EABI the first address is zero.
    LastArgStackLoc = (FirstStackArgLoc + VA.getLocMemOffset());
    int FI = MFI->CreateFixedObject(VA.getValVT().getSizeInBits()/8,
                                    LastArgStackLoc, true, false);

    SDValue PtrOff = DAG.getFrameIndex(FI,getPointerTy());

    // emit ISD::STORE whichs stores the 
    // parameter value to a stack Location
    MemOpChains.push_back(DAG.getStore(Chain, dl, Arg, PtrOff, NULL, 0,
                                       false, false, 0));
  }

  // Transform all store nodes into one single node because all store
  // nodes are independent of each other.
  if (!MemOpChains.empty())     
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, 
                        &MemOpChains[0], MemOpChains.size());

  // Build a sequence of copy-to-reg nodes chained together with token 
  // chain and flag operands which copy the outgoing args into registers.
  // The InFlag in necessary since all emited instructions must be
  // stuck together.
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Chain = DAG.getCopyToReg(Chain, dl, RegsToPass[i].first, 
                             RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // If the callee is a GlobalAddress/ExternalSymbol node (quite common, every
  // direct call is) turn it into a TargetGlobalAddress/TargetExternalSymbol 
  // node so that legalize doesn't hack it. 
  unsigned char OpFlag = IsPIC ? Nios2II::MO_GOT_CALL : Nios2II::MO_NO_FLAG;
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) 
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(),
                                        getPointerTy(), 0, OpFlag);
  else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(S->getSymbol(),
                                         getPointerTy(), OpFlag);

  // Nios2Call = #chain, #target_address, #opt_in_flags...
  //           = Chain, Callee, Reg#1, Reg#2, ...  
  //
  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Flag);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are 
  // known live into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));

  if (InFlag.getNode())
    Ops.push_back(InFlag);

  Chain  = DAG.getNode(Nios2ISD::Call, dl, NodeTys, &Ops[0], Ops.size());
  InFlag = Chain.getValue(1);

  // Create a stack location to hold GP when PIC is used. This stack 
  // location is used on function prologue to save GP and also after all 
  // emited CALL's to restore GP. 
  if (IsPIC) {
      // Function can have an arbitrary number of calls, so 
      // hold the LastArgStackLoc with the biggest offset.
      int FI;
      Nios2FunctionInfo *Nios2FI = MF.getInfo<Nios2FunctionInfo>();
      if (LastArgStackLoc >= Nios2FI->getGPStackOffset()) {
        LastArgStackLoc = (!LastArgStackLoc) ? (16) : (LastArgStackLoc+4);
        // Create the frame index only once. SPOffset here can be anything 
        // (this will be fixed on processFunctionBeforeFrameFinalized)
        if (Nios2FI->getGPStackOffset() == -1) {
          FI = MFI->CreateFixedObject(4, 0, true, false);
          Nios2FI->setGPFI(FI);
        }
        Nios2FI->setGPStackOffset(LastArgStackLoc);
      }

      // Reload GP value.
      FI = Nios2FI->getGPFI();
      SDValue FIN = DAG.getFrameIndex(FI,getPointerTy());
      SDValue GPLoad = DAG.getLoad(MVT::i32, dl, Chain, FIN, NULL, 0,
                                   false, false, 0);
      Chain = GPLoad.getValue(1);
      Chain = DAG.getCopyToReg(Chain, dl, DAG.getRegister(Nios2::GP, MVT::i32), 
                               GPLoad, SDValue(0,0));
      InFlag = Chain.getValue(1);
  }      

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(NumBytes, true),
                             DAG.getIntPtrConstant(0, true), InFlag);
  InFlag = Chain.getValue(1);

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, isVarArg,
                         Ins, dl, DAG, InVals);
}

/// LowerCallResult - Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
SDValue
Nios2TargetLowering::LowerCallResult(SDValue Chain, SDValue InFlag,
                                     CallingConv::ID CallConv, bool isVarArg,
                                     const SmallVectorImpl<ISD::InputArg> &Ins,
                                     DebugLoc dl, SelectionDAG &DAG,
                                     SmallVectorImpl<SDValue> &InVals) const
{

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, getTargetMachine(),
                 RVLocs, *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_Nios2);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    Chain = DAG.getCopyFromReg(Chain, dl, RVLocs[i].getLocReg(),
                               RVLocs[i].getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

static bool CC_Nios2_VarArgs(unsigned ValNo, EVT ValVT,
                       EVT LocVT, CCValAssign::LocInfo LocInfo,
                       ISD::ArgFlagsTy ArgFlags, CCState &State) {

  static const unsigned IntRegsSize=4;

  static const unsigned IntRegs[] = {
      Nios2::A0, Nios2::A1, Nios2::A2, Nios2::A3
  };

  // Promote i8 and i16
  if (LocVT == MVT::i8 || LocVT == MVT::i16) {
    LocVT = MVT::i32;
    if (ArgFlags.isSExt())
      LocInfo = CCValAssign::SExt;
    else if (ArgFlags.isZExt())
      LocInfo = CCValAssign::ZExt;
    else
      LocInfo = CCValAssign::AExt;
  }

  if (ValVT == MVT::i32 || ValVT == MVT::f32) {
    if (unsigned Reg = State.AllocateReg(IntRegs, IntRegsSize)) {
      State.addLoc(CCValAssign::getReg(ValNo, ValVT, Reg, MVT::i32, LocInfo));
      return false;
    }
    unsigned Off = State.AllocateStack(4, 4);
    State.addLoc(CCValAssign::getMem(ValNo, ValVT, Off, LocVT, LocInfo));
    return false;
  }

  unsigned UnallocIntReg = State.getFirstUnallocated(IntRegs, IntRegsSize);
  if (ValVT == MVT::f64) {
    if (IntRegs[UnallocIntReg] == (unsigned (Nios2::A1))) {
      // A1 can't be used anymore, because 64 bit arguments
      // must be aligned when copied back to the caller stack
      State.AllocateReg(IntRegs, IntRegsSize);
      UnallocIntReg++;
    }

    if (IntRegs[UnallocIntReg] == (unsigned (Nios2::A0)) ||
        IntRegs[UnallocIntReg] == (unsigned (Nios2::A2))) {
      unsigned Reg = State.AllocateReg(IntRegs, IntRegsSize);
      State.addLoc(CCValAssign::getReg(ValNo, ValVT, Reg, MVT::i32, LocInfo));
      // Shadow the next register so it can be used 
      // later to get the other 32bit part.
      State.AllocateReg(IntRegs, IntRegsSize);
      return false;
    }

    // Register is shadowed to preserve alignment, and the
    // argument goes to a stack location.
    if (UnallocIntReg != IntRegsSize)
      State.AllocateReg(IntRegs, IntRegsSize);

    unsigned Off = State.AllocateStack(8, 8);
    State.addLoc(CCValAssign::getMem(ValNo, ValVT, Off, LocVT, LocInfo));
    return false;
  }

  return true; // CC didn't match
}

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

/// LowerFormalArguments - transform physical registers into
/// virtual registers and generate load operations for
/// arguments places on the stack.
/// TODO: isVarArg
SDValue
Nios2TargetLowering::LowerFormalArguments(SDValue Chain,
                                         CallingConv::ID CallConv, bool isVarArg,
                                         const SmallVectorImpl<ISD::InputArg>
                                           &Ins,
                                         DebugLoc dl, SelectionDAG &DAG,
                                         SmallVectorImpl<SDValue> &InVals) const
{
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  Nios2FunctionInfo *Nios2FI = MF.getInfo<Nios2FunctionInfo>();

  unsigned StackReg = MF.getTarget().getRegisterInfo()->getFrameRegister(MF);
  Nios2FI->setVarArgsFrameIndex(0);

  // Used with vargs to acumulate store chains.
  std::vector<SDValue> OutChains;

  // Keep track of the last register used for arguments
  unsigned ArgRegEnd = 0;

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, getTargetMachine(),
                 ArgLocs, *DAG.getContext());

  CCInfo.AnalyzeFormalArguments(Ins, isVarArg ? CC_Nios2_VarArgs : CC_Nios2);

  SDValue StackPtr;

  unsigned FirstStackArgLoc = 0;

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];

    // Arguments stored on registers
    if (VA.isRegLoc()) {
      EVT RegVT = VA.getLocVT();
      ArgRegEnd = VA.getLocReg();
      TargetRegisterClass *RC = 0;

      if (RegVT == MVT::i32)
        RC = Nios2::CPURegsRegisterClass; 
      else  
        llvm_unreachable("RegVT not supported by FormalArguments Lowering");

      // Transform the arguments stored on 
      // physical registers into virtual ones
      unsigned Reg = AddLiveIn(DAG.getMachineFunction(), ArgRegEnd, RC);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, RegVT);
      
      // If this is an 8 or 16-bit value, it has been passed promoted 
      // to 32 bits.  Insert an assert[sz]ext to capture this, then 
      // truncate to the right size.
      if (VA.getLocInfo() != CCValAssign::Full) {
        unsigned Opcode = 0;
        if (VA.getLocInfo() == CCValAssign::SExt)
          Opcode = ISD::AssertSext;
        else if (VA.getLocInfo() == CCValAssign::ZExt)
          Opcode = ISD::AssertZext;
        if (Opcode)
          ArgValue = DAG.getNode(Opcode, dl, RegVT, ArgValue, 
                                 DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgValue);
      }

      InVals.push_back(ArgValue);
    } else { // VA.isRegLoc()

      // sanity check
      assert(VA.isMemLoc());

      // The last argument is not a register anymore
      ArgRegEnd = 0;
      
      // The stack pointer offset is relative to the caller stack frame. 
      // Since the real stack size is unknown here, a negative SPOffset 
      // is used so there's a way to adjust these offsets when the stack
      // size get known (on EliminateFrameIndex). A dummy SPOffset is 
      // used instead of a direct negative address (which is recorded to
      // be used on emitPrologue) to avoid mis-calc of the first stack 
      // offset on PEI::calculateFrameObjectOffsets.
      // Arguments are always 32-bit.
      unsigned ArgSize = VA.getLocVT().getSizeInBits()/8;
      int FI = MFI->CreateFixedObject(ArgSize, 0, true, false);
      Nios2FI->recordLoadArgsFI(FI, -(ArgSize+
        (FirstStackArgLoc + VA.getLocMemOffset())));

      // Create load nodes to retrieve arguments from the stack
      SDValue FIN = DAG.getFrameIndex(FI, getPointerTy());
      InVals.push_back(DAG.getLoad(VA.getValVT(), dl, Chain, FIN, NULL, 0,
                                   false, false, 0));
    }
  }

  // The mips ABIs for returning structs by value requires that we copy
  // the sret argument into $v0 for the return. Save the argument into
  // a virtual register so that we can access it from the return points.
  if (DAG.getMachineFunction().getFunction()->hasStructRetAttr()) {
    unsigned Reg = Nios2FI->getSRetReturnReg();
    if (!Reg) {
      Reg = MF.getRegInfo().createVirtualRegister(getRegClassFor(MVT::i32));
      Nios2FI->setSRetReturnReg(Reg);
    }
    SDValue Copy = DAG.getCopyToReg(DAG.getEntryNode(), dl, Reg, InVals[0]);
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, Copy, Chain);
  }

  // To meet ABI, when VARARGS are passed on registers, the registers
  // must have their values written to the caller stack frame. If the last
  // argument was placed in the stack, there's no need to save any register. 
  if ((isVarArg) && ArgRegEnd) {
    if (StackPtr.getNode() == 0)
      StackPtr = DAG.getRegister(StackReg, getPointerTy());
  
    // The last register argument that must be saved is Nios2::A3
    TargetRegisterClass *RC = Nios2::CPURegsRegisterClass;
    unsigned StackLoc = ArgLocs.size()-1;

    for (++ArgRegEnd; ArgRegEnd <= Nios2::A3; ++ArgRegEnd, ++StackLoc) {
      unsigned Reg = AddLiveIn(DAG.getMachineFunction(), ArgRegEnd, RC);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, MVT::i32);

      int FI = MFI->CreateFixedObject(4, 0, true, false);
      Nios2FI->recordStoreVarArgsFI(FI, -(4+(StackLoc*4)));
      SDValue PtrOff = DAG.getFrameIndex(FI, getPointerTy());
      OutChains.push_back(DAG.getStore(Chain, dl, ArgValue, PtrOff, NULL, 0,
                                       false, false, 0));

      // Record the frame index of the first variable argument
      // which is a value necessary to VASTART.
      if (!Nios2FI->getVarArgsFrameIndex())
        Nios2FI->setVarArgsFrameIndex(FI);
    }
  }

  // All stores are grouped in one node to allow the matching between 
  // the size of Ins and InVals. This only happens when on varg functions
  if (!OutChains.empty()) {
    OutChains.push_back(Chain);
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other,
                        &OutChains[0], OutChains.size());
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue
Nios2TargetLowering::LowerReturn(SDValue Chain,
                                CallingConv::ID CallConv, bool isVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                DebugLoc dl, SelectionDAG &DAG) const
{

  // CCValAssign - represent the assignment of
  // the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, getTargetMachine(),
                 RVLocs, *DAG.getContext());

  // Analize return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_Nios2);

  // If this is the first return lowered for this function, add 
  // the regs to the liveout set for the function.
  if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
    for (unsigned i = 0; i != RVLocs.size(); ++i)
      if (RVLocs[i].isRegLoc())
        DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
  }

  SDValue Flag;

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), 
                             Outs[i].Val, Flag);

    // guarantee that all emitted copies are
    // stuck together, avoiding something bad
    Flag = Chain.getValue(1);
  }

  // The mips ABIs for returning structs by value requires that we copy
  // the sret argument into $v0 for the return. We saved the argument into
  // a virtual register in the entry block, so now we copy the value out
  // and into $v0.
  if (DAG.getMachineFunction().getFunction()->hasStructRetAttr()) {
    MachineFunction &MF      = DAG.getMachineFunction();
    Nios2FunctionInfo *Nios2FI = MF.getInfo<Nios2FunctionInfo>();
    unsigned Reg = Nios2FI->getSRetReturnReg();

    if (!Reg) 
      llvm_unreachable("sret virtual register not created in the entry block");
    SDValue Val = DAG.getCopyFromReg(Chain, dl, Reg, getPointerTy());

    Chain = DAG.getCopyToReg(Chain, dl, Nios2::V0, Val, Flag);
    Flag = Chain.getValue(1);
  }

  // Return on Nios2 is always a "jr $ra"
  if (Flag.getNode())
    return DAG.getNode(Nios2ISD::Ret, dl, MVT::Other, 
                       Chain, DAG.getRegister(Nios2::RA, MVT::i32), Flag);
  else // Return Void
    return DAG.getNode(Nios2ISD::Ret, dl, MVT::Other, 
                       Chain, DAG.getRegister(Nios2::RA, MVT::i32));
}

//===----------------------------------------------------------------------===//
//                           Nios2 Inline Assembly Support
//===----------------------------------------------------------------------===//

/// getConstraintType - Given a constraint letter, return the type of
/// constraint it is for this target.
Nios2TargetLowering::ConstraintType Nios2TargetLowering::
getConstraintType(const std::string &Constraint) const 
{
    // Nios2 specific constrainy 
    // GCC config/nios2/constraints.md
    //
    // 'd' : An address register. Equivalent to r 
    //       unless generating NIOS216 code. 
    // 'y' : Equivalent to r; retained for 
    //       backwards compatibility. 
    // 'f' : Floating Point registers.      
    if (Constraint.size() == 1) {
        switch (Constraint[0]) {
            default : break;
            case 'd':     
            case 'y': 
            case 'f':
                      return C_RegisterClass;
                      break;
        }
    }
    return TargetLowering::getConstraintType(Constraint);
}

/// getRegClassForInlineAsmConstraint - Given a constraint letter (e.g. "r"),
/// return a list of registers that can be used to satisfy the constraint.
/// This should only be used for C_RegisterClass constraints.
std::pair<unsigned, const TargetRegisterClass*> Nios2TargetLowering::
getRegForInlineAsmConstraint(const std::string &Constraint, EVT VT) const
{
    if (Constraint.size() == 1) {
        switch (Constraint[0]) {
            case 'r':
                return std::make_pair(0U, Nios2::CPURegsRegisterClass);
        }
    }
    return TargetLowering::getRegForInlineAsmConstraint(Constraint, VT);
}

/// Given a register class constraint, like 'r', if this corresponds directly
/// to an LLVM register class, return a register of 0 and the register class
/// pointer.
std::vector<unsigned> Nios2TargetLowering::
getRegClassForInlineAsmConstraint(const std::string &Constraint,
        EVT VT) const
{
    if (Constraint.size() != 1)
        return std::vector<unsigned>();

    switch (Constraint[0]) {         
        default : break;
        case 'r':
                  // GCC Nios2 Constraint Letters
        case 'd':     
        case 'y': 
                  return make_vector<unsigned>(Nios2::T0, Nios2::T1, Nios2::T2, Nios2::T3, 
                          Nios2::T4, Nios2::T5, Nios2::T6, Nios2::T7, Nios2::S0, Nios2::S1, 
                          Nios2::S2, Nios2::S3, Nios2::S4, Nios2::S5, Nios2::S6, Nios2::S7, 
                          0);
    }
    return std::vector<unsigned>();
}

bool
Nios2TargetLowering::isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const {
  // The Nios2 target isn't yet aware of offsets.
  return false;
}

bool Nios2TargetLowering::isFPImmLegal(const APFloat &Imm, EVT VT) const {
  if (VT != MVT::f32 && VT != MVT::f64)
    return false;
  return Imm.isZero();
}

