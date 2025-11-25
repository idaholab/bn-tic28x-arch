// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "instructions.h"

#include "util.h"

namespace binaryninja = BN;

namespace TIC28X {
/**
 * DecodeInstruction - parses an opcode and returns a corresponding Instruction.
 *
 * This method iterates over a vector containing all Instructions.
 * Starting with 16-bit instructions, the given opcode is masked with each
 * instruction's opcode mask and the result is compared with that instruction's
 * opcode. If we have a match, the instruction is returned.
 *
 * If we do not find a match, we do the same thing with 32-bits of data and
 * check against 32-bit instructions. If we have no match, nullopt is returned.
 *
 * @param data - pointer to opcode data from binary ninja
 * @param instruction_list - vector of pointers to Instructions
 * @param objmode - current operating object mode for the processor
 * @return either nullopt or a valid instruction
 */
std::optional<std::shared_ptr<Instruction>> DecodeInstruction(
    const uint8_t* data,
    const std::vector<std::optional<std::shared_ptr<Instruction>>>&
        instruction_list,
    const ObjectMode objmode) {
  // Construct 16-bit opcode
  const uint16_t op16 = DataToOpcode(data, Sizes::_2_BYTES);
  for (auto inst : instruction_list) {
    if (inst->get()->GetLength() == Sizes::_2_BYTES) {
      if ((op16 & inst->get()->GetOpcodeMask()) == inst->get()->GetOpcode()) {
        const auto inst_objmode = inst->get()->GetObjmode();
        if (inst_objmode == OBJMODE_ANY || inst_objmode == objmode) {
          return inst;
        }
      }
    }
  }

  // Construct 32-bit opcode
  const uint32_t op32 = DataToOpcode(data, Sizes::_4_BYTES);
  for (auto inst : instruction_list) {
    if (inst->get()->GetLength() == Sizes::_4_BYTES) {
      if ((op32 & inst->get()->GetOpcodeMask()) == inst->get()->GetOpcode()) {
        const auto inst_objmode = inst->get()->GetObjmode();
        if (inst_objmode == OBJMODE_ANY || inst_objmode == objmode) {
          return inst;
        }
      }
    }
  }
  BN::LogDebug("invalid opcode: %x", op32);
  return std::nullopt;
}

/**
 * GenerateInstructionVector - returns a list of shared pointers to
 * Instructions. This method should be called once when constructing the
 * architecture, and the vector can be saved and reused throughout
 * info/text/lift generation.
 * @return vector of all possible Instructions
 */
std::vector<std::optional<std::shared_ptr<Instruction>>>
GenerateInstructionVector() {
  std::vector<std::optional<std::shared_ptr<Instruction>>> vec = {
      std::make_shared<Aborti>(),
      std::make_shared<AbsAcc>(),
      std::make_shared<AbstcAcc>(),
      std::make_shared<AddAccConst16Shift>(),
      std::make_shared<AddAccLoc16ShiftT>(),
      std::make_shared<AddAccLoc16>(),
      std::make_shared<AddAccLoc16Shift1_15>(),
      std::make_shared<AddAccLoc16Shift16>(),
      std::make_shared<AddAccLoc16Shift0_15>(),
      std::make_shared<AddAxLoc16>(),
      std::make_shared<AddLoc16Ax>(),
      std::make_shared<AddLoc16Const16>(),
      std::make_shared<AddbAccConst8>(),
      std::make_shared<AddbAxConst8>(),
      std::make_shared<AddbSpConst7>(),
      std::make_shared<AddbXarnConst7>(),
      std::make_shared<AddclAccLoc32>(),
      std::make_shared<AddcuAccLoc16>(),
      std::make_shared<AddlAccLoc32>(),
      std::make_shared<AddlAccPShiftPm>(),
      std::make_shared<AddlLoc32Acc>(),
      std::make_shared<AdduAccLoc16>(),
      std::make_shared<AddulPLoc32>(),
      std::make_shared<AddulAccLoc32>(),
      std::make_shared<AdrkImm8>(),
      std::make_shared<AndAccConst16Shift0_15>(),
      std::make_shared<AndAccConst16Shift16>(),
      std::make_shared<AndAccLoc16>(),
      std::make_shared<AndAxLoc16Const16>(),
      std::make_shared<AndIerConst16>(),
      std::make_shared<AndIfrConst16>(),
      std::make_shared<AndLoc16Ax>(),
      std::make_shared<AndAxLoc16>(),
      std::make_shared<AndLoc16Const16>(),
      std::make_shared<AndbAxConst8>(),
      std::make_shared<Asp>(),
      std::make_shared<AsrAxShift>(),
      std::make_shared<AsrAxT>(),
      std::make_shared<Asr64AccPShift>(),
      std::make_shared<Asr64AccPT>(),
      std::make_shared<AsrlAccT>(),
      std::make_shared<BOff16Cond>(),
      std::make_shared<BanzOff16Arn>(),
      std::make_shared<BarOff16ArnArmEq>(),
      std::make_shared<BarOff16ArnArmNeq>(),
      std::make_shared<BfOff16Cond>(),
      std::make_shared<ClrcAmode>(),
      std::make_shared<ClrcM0M1Map>(),
      std::make_shared<ClrcObjmode>(),
      std::make_shared<ClrcOvc>(),
      std::make_shared<ClrcXf>(),
      std::make_shared<ClrcMode>(),
      std::make_shared<CmpAxLoc16>(),
      std::make_shared<CmpLoc16Const16>(),
      std::make_shared<Cmp64AccP>(),
      std::make_shared<CmpbAxConst8>(),
      std::make_shared<CmplAccLoc32>(),
      std::make_shared<CmplAccPShiftPm>(),
      std::make_shared<Cmpr0>(),
      std::make_shared<Cmpr1>(),
      std::make_shared<Cmpr2>(),
      std::make_shared<Cmpr3>(),
      std::make_shared<CsbAcc>(),
      std::make_shared<DecLoc16>(),
      std::make_shared<DmacAccPLoc32Xar7>(),
      std::make_shared<DmacAccPLoc32Xar7Postinc>(),
      std::make_shared<DmovLoc16>(),
      std::make_shared<Eallow>(),
      std::make_shared<Edis>(),
      std::make_shared<Estop0>(),
      std::make_shared<Estop1>(),
      std::make_shared<FfcXar7Const22>(),
      std::make_shared<FlipAx>(),
      std::make_shared<IackConst16>(),
      std::make_shared<Idle>(),
      std::make_shared<ImaclPLoc32Xar7>(),
      std::make_shared<ImaclPLoc32Xar7Postinc>(),
      std::make_shared<ImpyalPXtLoc32>(),
      std::make_shared<ImpylAccXtLoc32>(),
      std::make_shared<ImpylPXtLoc32>(),
      std::make_shared<ImpyslPXtLoc32>(),
      std::make_shared<ImpyxulPXtLoc32>(),
      std::make_shared<InLoc16Pa>(),
      std::make_shared<IncLoc16>(),
      std::make_shared<IntrIntx>(),
      std::make_shared<IntrNmi>(),
      std::make_shared<IntrEmuint>(),
      std::make_shared<Iret>(),
      std::make_shared<LbXar7>(),
      std::make_shared<LbConst22>(),
      std::make_shared<LcXar7>(),
      std::make_shared<LcConst22>(),
      std::make_shared<LcrConst22>(),
      std::make_shared<LcrXarn>(),
      std::make_shared<LoopnzLoc16Const16>(),
      std::make_shared<LoopzLoc16Const16>(),
      std::make_shared<Lpaddr>(),
      std::make_shared<Lret>(),
      std::make_shared<Lrete>(),
      std::make_shared<Lretr>(),
      std::make_shared<LslAccShift>(),
      std::make_shared<LslAccT>(),
      std::make_shared<LslAxShift>(),
      std::make_shared<LslAxT>(),
      std::make_shared<Lsl64AccPShift>(),
      std::make_shared<Lsl64AccPT>(),
      std::make_shared<LsllAccT>(),
      std::make_shared<LsrAxShift>(),
      std::make_shared<LsrAxT>(),
      std::make_shared<Lsr64AccPShift>(),
      std::make_shared<Lsr64AccPT>(),
      std::make_shared<LsrlAccT>(),
      std::make_shared<MacPLoc16Pma>(),
      std::make_shared<MacPLoc16Xar7>(),
      std::make_shared<MacPLoc16Xar7Postinc>(),
      std::make_shared<MaxAxLoc16>(),
      std::make_shared<MaxculPLoc32>(),
      std::make_shared<MaxlAccLoc32>(),
      std::make_shared<MinAxLoc16>(),
      std::make_shared<MinculPLoc32>(),
      std::make_shared<MinlAccLoc32>(),
      std::make_shared<MovMem16Loc16>(),
      std::make_shared<MovAccConst16Shift>(),
      std::make_shared<MovAccLoc16ShiftT>(),
      std::make_shared<MovAccLoc16Objmode1>(),
      std::make_shared<MovAccLoc16Objmode0>(),
      std::make_shared<MovAccLoc16Shift1_15Objmode1>(),
      std::make_shared<MovAccLoc16Shift1_15Objmode0>(),
      std::make_shared<MovAccLoc16Shift16>(),
      std::make_shared<MovAr6Loc16>(),
      std::make_shared<MovAr7Loc16>(),
      std::make_shared<MovAxLoc16>(),
      std::make_shared<MovDpConst10>(),
      std::make_shared<MovIerLoc16>(),
      std::make_shared<MovLoc16Const16>(),
      std::make_shared<MovLoc16Mem16>(),
      std::make_shared<MovLoc16_0>(),
      std::make_shared<MovLoc16AccShift1>(),
      std::make_shared<MovLoc16AccShift2_8Objmode1>(),
      std::make_shared<MovLoc16AccShift2_8Objmode0>(),
      std::make_shared<MovLoc16Arn>(),
      std::make_shared<MovLoc16Ax>(),
      std::make_shared<MovLoc16AxCond>(),
      std::make_shared<MovLoc16Ier>(),
      std::make_shared<MovLoc16Ovc>(),
      std::make_shared<MovLoc16P>(),
      std::make_shared<MovLoc16T>(),
      std::make_shared<MovOvcLoc16>(),
      std::make_shared<MovPhLoc16>(),
      std::make_shared<MovPlLoc16>(),
      std::make_shared<MovPmAx>(),
      std::make_shared<MovTLoc16>(),
      std::make_shared<MovTl0>(),
      std::make_shared<MovXarnPc>(),
      std::make_shared<MovaTLoc16>(),
      std::make_shared<MovadTLoc16>(),
      std::make_shared<MovbAccConst8>(),
      std::make_shared<MovbAr6Const8>(),
      std::make_shared<MovbAr7Const8>(),
      std::make_shared<MovbAxConst8>(),
      std::make_shared<MovbAxlsbLoc16>(),
      std::make_shared<MovbAxmsbLoc16>(),
      std::make_shared<MovbLoc16Const8Cond>(),
      std::make_shared<MovbLoc16Axlsb>(),
      std::make_shared<MovbLoc16Axmsb>(),
      std::make_shared<MovbXar0Const8>(),
      std::make_shared<MovbXar1Const8>(),
      std::make_shared<MovbXar2Const8>(),
      std::make_shared<MovbXar3Const8>(),
      std::make_shared<MovbXar4Const8>(),
      std::make_shared<MovbXar5Const8>(),
      std::make_shared<MovbXar6Const8>(),
      std::make_shared<MovbXar7Const8>(),
      std::make_shared<MovdlXtLoc32>(),
      std::make_shared<MovhLoc16AccShift1>(),
      std::make_shared<MovhLoc16AccShift2_8Objmode1>(),
      std::make_shared<MovhLoc16AccShift2_8Objmode0>(),
      std::make_shared<MovhLoc16P>(),
      std::make_shared<MovlAccLoc32>(),
      std::make_shared<MovlAccPShiftPm>(),
      std::make_shared<MovlLoc32Acc>(),
      std::make_shared<MovlLoc32AccCond>(),
      std::make_shared<MovlLoc32P>(),
      std::make_shared<MovlLoc32Xar0>(),
      std::make_shared<MovlLoc32Xar1>(),
      std::make_shared<MovlLoc32Xar2>(),
      std::make_shared<MovlLoc32Xar3>(),
      std::make_shared<MovlLoc32Xar4>(),
      std::make_shared<MovlLoc32Xar5>(),
      std::make_shared<MovlLoc32Xar6>(),
      std::make_shared<MovlLoc32Xar7>(),
      std::make_shared<MovlLoc32Xt>(),
      std::make_shared<MovlPAcc>(),
      std::make_shared<MovlPLoc32>(),
      std::make_shared<MovlXar0Loc32>(),
      std::make_shared<MovlXar1Loc32>(),
      std::make_shared<MovlXar2Loc32>(),
      std::make_shared<MovlXar3Loc32>(),
      std::make_shared<MovlXar4Loc32>(),
      std::make_shared<MovlXar5Loc32>(),
      std::make_shared<MovlXar6Loc32>(),
      std::make_shared<MovlXar7Loc32>(),
      std::make_shared<MovlXar0Const22>(),
      std::make_shared<MovlXar1Const22>(),
      std::make_shared<MovlXar2Const22>(),
      std::make_shared<MovlXar3Const22>(),
      std::make_shared<MovlXar4Const22>(),
      std::make_shared<MovlXar5Const22>(),
      std::make_shared<MovlXar6Const22>(),
      std::make_shared<MovlXar7Const22>(),
      std::make_shared<MovlXtLoc32>(),
      std::make_shared<MovpTLoc16>(),
      std::make_shared<MovsTLoc16>(),
      std::make_shared<MovuAccLoc16>(),
      std::make_shared<MovuLoc16Ovc>(),
      std::make_shared<MovuOvcLoc16>(),
      std::make_shared<MovwDpConst16>(),
      std::make_shared<MovxTlLoc16>(),
      std::make_shared<MovzAr0Loc16>(),
      std::make_shared<MovzAr1Loc16>(),
      std::make_shared<MovzAr2Loc16>(),
      std::make_shared<MovzAr3Loc16>(),
      std::make_shared<MovzAr4Loc16>(),
      std::make_shared<MovzAr5Loc16>(),
      std::make_shared<MovzAr6Loc16>(),
      std::make_shared<MovzAr7Loc16>(),
      std::make_shared<MovzDpConst10>(),
      std::make_shared<MpyAccLoc16Const16>(),
      std::make_shared<MpyAccTLoc16>(),
      std::make_shared<MpyPLoc16Const16>(),
      std::make_shared<MpyPTLoc16>(),
      std::make_shared<MpyaPLoc16Const16>(),
      std::make_shared<MpyaPTLoc16>(),
      std::make_shared<MpybAccTConst8>(),
      std::make_shared<MpybPTConst8>(),
      std::make_shared<MpysPTLoc16>(),
      std::make_shared<MpyuPTLoc16>(),
      std::make_shared<MpyuAccTLoc16>(),
      std::make_shared<MpyxuAccTLoc16>(),
      std::make_shared<MpyxuPTLoc16>(),
      std::make_shared<Nasp>(),
      std::make_shared<NegAcc>(),
      std::make_shared<NegAx>(),
      std::make_shared<Neg64AccP>(),
      std::make_shared<NegtcAcc>(),
      std::make_shared<NopIndArpn>(),
      std::make_shared<NormAccInd>(),
      std::make_shared<NormAccIndPostinc>(),
      std::make_shared<NormAccIndPostdec>(),
      std::make_shared<NormAccInd0Postinc>(),
      std::make_shared<NormAccInd0Postdec>(),
      std::make_shared<NormAccXarnPostinc>(),
      std::make_shared<NormAccXarnPostdec>(),
      std::make_shared<NotAcc>(),
      std::make_shared<NotAx>(),
      std::make_shared<OrAccLoc16>(),
      std::make_shared<OrAccConst16Shift0_15>(),
      std::make_shared<OrAccConst16Shift16>(),
      std::make_shared<OrAxLoc16>(),
      std::make_shared<OrIerConst16>(),
      std::make_shared<OrIfrConst16>(),
      std::make_shared<OrLoc16Const16>(),
      std::make_shared<OrLoc16Ax>(),
      std::make_shared<OrbAxConst8>(),
      std::make_shared<OutPaLoc16>(),
      std::make_shared<PopAr1Ar0>(),
      std::make_shared<PopAr3Ar2>(),
      std::make_shared<PopAr5Ar4>(),
      std::make_shared<PopAr1hAr0h>(),
      std::make_shared<PopDbgier>(),
      std::make_shared<PopDp>(),
      std::make_shared<PopDpSt1>(),
      std::make_shared<PopIfr>(),
      std::make_shared<PopLoc16>(),
      std::make_shared<PopP>(),
      std::make_shared<PopRpc>(),
      std::make_shared<PopSt0>(),
      std::make_shared<PopSt1>(),
      std::make_shared<PopTSt0>(),
      std::make_shared<PopXt>(),
      std::make_shared<PreadLoc16Xar7>(),
      std::make_shared<PushAr1Ar0>(),
      std::make_shared<PushAr3Ar2>(),
      std::make_shared<PushAr5Ar4>(),
      std::make_shared<PushAr1hAr0h>(),
      std::make_shared<PushDbgier>(),
      std::make_shared<PushDp>(),
      std::make_shared<PushDpSt1>(),
      std::make_shared<PushIfr>(),
      std::make_shared<PushLoc16>(),
      std::make_shared<PushP>(),
      std::make_shared<PushRpc>(),
      std::make_shared<PushSt0>(),
      std::make_shared<PushSt1>(),
      std::make_shared<PushTSt0>(),
      std::make_shared<PushXt>(),
      std::make_shared<PwriteXar7Loc16>(),
      std::make_shared<QmaclPLoc32Xar7>(),
      std::make_shared<QmaclPLoc32Xar7Postinc>(),
      std::make_shared<QmpyalPXtLoc32>(),
      std::make_shared<QmpylPXtLoc32>(),
      std::make_shared<QmpylAccXtLoc32>(),
      std::make_shared<QmpyslPXtLoc32>(),
      std::make_shared<QmpyulPXtLoc32>(),
      std::make_shared<QmpyxulPXtLoc32>(),
      std::make_shared<RolAcc>(),
      std::make_shared<RorAcc>(),
      std::make_shared<RptConst8>(),
      std::make_shared<RptLoc16>(),
      std::make_shared<SatAcc>(),
      std::make_shared<Sat64AccP>(),
      std::make_shared<SbOff8Cond>(),
      std::make_shared<SbbuAccLoc16>(),
      std::make_shared<SbfOff8Eq>(),
      std::make_shared<SbfOff8Neq>(),
      std::make_shared<SbfOff8Tc>(),
      std::make_shared<SbfOff8Ntc>(),
      std::make_shared<SbrkConst8>(),
      std::make_shared<SetcMode>(),
      std::make_shared<SetcM0M1Map>(),
      std::make_shared<SetcObjmode>(),
      std::make_shared<SetcXf>(),
      std::make_shared<SfrAccShift>(),
      std::make_shared<SfrAccT>(),
      std::make_shared<SpmShift>(),
      std::make_shared<SqraLoc16>(),
      std::make_shared<SqrsLoc16>(),
      std::make_shared<SubAccLoc16Objmode1>(),
      std::make_shared<SubAccLoc16Objmode0>(),
      std::make_shared<SubAccLoc16Shift1_15Objmode1>(),
      std::make_shared<SubAccLoc16Shift1_15Objmode0>(),
      std::make_shared<SubAccLoc16Shift16>(),
      std::make_shared<SubAccLoc16ShiftT>(),
      std::make_shared<SubAccConst16Shift>(),
      std::make_shared<SubAxLoc16>(),
      std::make_shared<SubLoc16Ax>(),
      std::make_shared<SubbAccConst8>(),
      std::make_shared<SubbSpConst7>(),
      std::make_shared<SubbXarnConst7>(),
      std::make_shared<SubblAccLoc32>(),
      std::make_shared<SubcuAccLoc16>(),
      std::make_shared<SubculAccLoc32>(),
      std::make_shared<SublAccLoc32>(),
      std::make_shared<SublLoc32Acc>(),
      std::make_shared<SubrLoc16Ax>(),
      std::make_shared<SubrlLoc32Acc>(),
      std::make_shared<SubuAccLoc16>(),
      std::make_shared<SubulAccLoc32>(),
      std::make_shared<SubulPLoc32>(),
      std::make_shared<TbitLoc16Bit>(),
      std::make_shared<TbitLoc16T>(),
      std::make_shared<TclrLoc16Bit>(),
      std::make_shared<TestAcc>(),
      std::make_shared<TrapVec>(),
      std::make_shared<TsetLoc16Bit>(),
      std::make_shared<UoutPaLoc16>(),
      std::make_shared<XbAl>(),
      std::make_shared<XbPmaArpn>(),
      std::make_shared<XbPmaCond>(),
      std::make_shared<XbanzPmaInd>(),
      std::make_shared<XbanzPmaIndPostinc>(),
      std::make_shared<XbanzPmaIndPostdec>(),
      std::make_shared<XbanzPmaInd0Postinc>(),
      std::make_shared<XbanzPmaInd0Postdec>(),
      std::make_shared<XbanzPmaIndArpn>(),
      std::make_shared<XbanzPmaIndPostincArpn>(),
      std::make_shared<XbanzPmaIndPostdecArpn>(),
      std::make_shared<XbanzPmaInd0PostincArpn>(),
      std::make_shared<XbanzPmaInd0PostdecArpn>(),
      std::make_shared<XcallAl>(),
      std::make_shared<XcallPmaArpn>(),
      std::make_shared<XcallPmaCond>(),
      std::make_shared<XmacPLoc16Pma>(),
      std::make_shared<XmacdPLoc16Pma>(),
      std::make_shared<XorAccLoc16>(),
      std::make_shared<XorAccConst16Shift0_15>(),
      std::make_shared<XorAccConst16Shift16>(),
      std::make_shared<XorAxLoc16>(),
      std::make_shared<XorLoc16Ax>(),
      std::make_shared<XorLoc16Const16>(),
      std::make_shared<XorbAxConst8>(),
      std::make_shared<XpreadLoc16Pma>(),
      std::make_shared<XpreadLoc16Al>(),
      std::make_shared<XpwriteAlLoc16>(),
      std::make_shared<XretcCond>(),
      std::make_shared<ZalrAccLoc16>(),
      std::make_shared<Zapa>(),

      /* Extended Instruction Set */

      // FPU Instructions
      std::make_shared<Absf32RahRbh>(),
      std::make_shared<Addf32Rah16fhiRbh>(),
      std::make_shared<Addf32RahRbhRch>(),
      std::make_shared<Addf32RdhRehRfhMov32Mem32Rah>(),
      std::make_shared<Addf32RdhRehRfhMov32RahMem32>(),
      std::make_shared<Cmpf32RahRbh>(),
      std::make_shared<Cmpf32Rah16fhi>(),
      std::make_shared<Cmpf32Rah0>(),
      std::make_shared<Einvf32RahRbh>(),
      std::make_shared<Eisqrtf32RahRbh>(),
      std::make_shared<F32toi16RahRbh>(),
      std::make_shared<F32toi16rRahRbh>(),
      std::make_shared<F32toi32RahRbh>(),
      std::make_shared<F32toui16RahRbh>(),
      std::make_shared<F32toui16rRahRbh>(),
      std::make_shared<F32toui32RahRbh>(),
      std::make_shared<Fracf32RahRbh>(),
      std::make_shared<I16tof32RahRbh>(),
      std::make_shared<I16tof32RahMem16>(),
      std::make_shared<I32tof32RahMem32>(),
      std::make_shared<I32tof32RahRbh>(),
      std::make_shared<Macf32R3hR2hRdhRehRfh>(),
      std::make_shared<Macf32R3hR2hRdhRehRfhMov32RahMem32>(),
      std::make_shared<Macf32R7hR3hMem32Xar7Postinc>(),
      std::make_shared<Macf32R7hR6hRdhRehRfh>(),
      std::make_shared<Macf32R7hR6hRdhRehRfhMov32RahMem32>(),
      std::make_shared<Maxf32RahRbh>(),
      std::make_shared<Maxf32Rah16fhi>(),
      std::make_shared<Maxf32RahRbhMov32RchRdh>(),
      std::make_shared<Minf32RahRbh>(),
      std::make_shared<Minf32Rah16fhi>(),

  };
  return vec;
}

/* Instruction Operand Extractors */

// Floating Point reg A-C, 16FHi helpers, format I:
// LSW: xxxx xxxx xxxx xxxx
// MSW: xxxx xxxc ccbb baaa
// OR
// LSW: xxxx xxxx xxII IIII
// MSW: IIII IIII IIbb baaa

uint8_t FpuGetRegA_I(const uint32_t data) { return data & 0x7u; }

uint32_t FpuSetRegA_I(const uint32_t opcode, const uint8_t a) {
  return opcode | a & 0x7u;
}

uint8_t FpuGetRegB_I(const uint32_t data) { return (data & 0x38u) >> 3; }

uint32_t FpuSetRegB_I(const uint32_t opcode, const uint8_t b) {
  return opcode | (b & 0x7u) << 3;
}

uint8_t FpuGetRegC_I(const uint32_t data) { return (data & 0x1C0u) >> 6; }

uint32_t FpuSetRegC_I(const uint32_t opcode, const uint8_t c) {
  return opcode | (c & 0x7u) << 6;
}

uint16_t FpuGet16FHi_I(const uint32_t data) { return (data & 0x3FFFC0u) >> 6; }

uint32_t FpuSet16FHi_I(const uint32_t opcode, const uint16_t i) {
  return opcode | i << 6;
}

// Floating Point reg A, D-F helpers, format II:
// LSW: xxxx xxxx xxxx fffe
// MSW: eedd daaa xxxx xxxx

uint8_t FpuGetRegA_II(const uint32_t data) { return (data & 0x700u) >> 8; }

uint32_t FpuSetRegA_II(const uint32_t opcode, const uint8_t a) {
  return opcode | (a & 0x7u) << 8;
}

uint8_t FpuGetRegD_II(const uint32_t data) { return (data & 0x3800u) >> 11; }

uint32_t FpuSetRegD_II(const uint32_t opcode, const uint8_t d) {
  return opcode | (d & 0x7u) << 11;
}

uint8_t FpuGetRegE_II(const uint32_t data) { return (data & 0x1C000u) >> 14; }

uint32_t FpuSetRegE_II(const uint32_t opcode, const uint8_t e) {
  return opcode | (e & 0x7u) << 14;
}

uint8_t FpuGetRegF_II(const uint32_t data) { return (data & 0xE0000u) >> 17; }

uint32_t FpuSetRegF_II(const uint32_t opcode, const uint8_t f) {
  return opcode | (f & 0x7u) << 17;
}

// Floating point reg A, 16FHi helpers, format III
// LSW: xxxx xxxx xxxx xIII
// MSW: IIII IIII IIII Iaaa

uint8_t FpuGetRegA_III(const uint32_t data) { return data & 0x7u; }

uint32_t FpuSetRegA_III(const uint32_t opcode, const uint8_t a) {
  return opcode | a & 0x7u;
}

uint16_t FpuGet16FHi_III(const uint32_t data) { return (data & 0x7FFF8u) >> 3; }

uint32_t FpuSet16FHi_III(const uint32_t opcode, const uint16_t i) {
  return opcode | i << 3;
}

// Floating point reg D-F, format IV
// LSW: xxxx xxxx xxxx xxff
// MSW: feee dddc ccbb baaa
// A-C regs are covered in format I

uint8_t FpuGetRegD_IV(const uint32_t data) { return (data & 0xE00u) >> 9; }

uint32_t FpuSetRegD_IV(const uint32_t opcode, const uint8_t d) {
  return opcode | (d & 0x7u) << 9;
}

uint8_t FpuGetRegE_IV(const uint32_t data) { return (data & 0x7000u) >> 12; }

uint32_t FpuSetRegE_IV(const uint32_t opcode, const uint8_t e) {
  return opcode | (e & 0x7u) << 12;
}

uint8_t FpuGetRegF_IV(const uint32_t data) { return (data & 0x38000u) >> 15; }

uint32_t FpuSetRegF_IV(const uint32_t opcode, const uint8_t f) {
  return opcode | (f & 0x7u) << 15;
}

// Other

uint8_t FpuGetMem(const uint32_t data) { return data & 0xFFu; }

uint32_t FpuSetMem(const uint32_t opcode, const uint8_t mem) {
  return opcode | mem;
}

/* Instruction Helper Implementations */

// AddAccConst16Shift
uint8_t AddAccConst16Shift::GetShift(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t AddAccConst16Shift::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 16;
}

uint16_t AddAccConst16Shift::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t AddAccConst16Shift::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// AddAccLoc16ShiftT
uint8_t AddAccLoc16ShiftT::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t AddAccLoc16ShiftT::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// AddAccLoc16
uint8_t AddAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t AddAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// AddAccLoc16Shift1_15
uint8_t AddAccLoc16Shift1_15::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t AddAccLoc16Shift1_15::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t AddAccLoc16Shift1_15::GetShift(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t AddAccLoc16Shift1_15::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 8;
}

// AddAccLoc16Shift16
uint8_t AddAccLoc16Shift16::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t AddAccLoc16Shift16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// AddAccLoc16Shift0_15
uint8_t AddAccLoc16Shift0_15::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t AddAccLoc16Shift0_15::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t AddAccLoc16Shift0_15::GetShift(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t AddAccLoc16Shift0_15::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 8;
}

// AddAxLoc16
uint8_t AddAxLoc16::GetLoc16(const uint32_t data) { return data & 0xFF; }

uint32_t AddAxLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t AddAxLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t AddAxLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// AddLoc16Ax
uint8_t AddLoc16Ax::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t AddLoc16Ax::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t AddLoc16Ax::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t AddLoc16Ax::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// AddLoc16Const16
uint8_t AddLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t AddLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t AddLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t AddLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// AddbAccConst8
uint8_t AddbAccConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t AddbAccConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// AddbAxConst8
uint8_t AddbAxConst8::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t AddbAxConst8::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t AddbAxConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t AddbAxConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// AddbSpConst7
uint8_t AddbSpConst7::GetConst7(const uint32_t data) { return data & 0x7Fu; }

uint32_t AddbSpConst7::SetConst7(const uint8_t const7) {
  return opcode | const7 & 0x7Fu;
}

// AddbXarnConst7
uint8_t AddbXarnConst7::GetRegN(const uint32_t data) {
  return (data & 0x700u) >> 8;
}

uint32_t AddbXarnConst7::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 8;
}

uint8_t AddbXarnConst7::GetConst7(const uint32_t data) { return data & 0x7Fu; }

uint32_t AddbXarnConst7::SetConst7(const uint8_t const7) {
  return opcode | const7 & 0x7Fu;
}

// AddbSpConst7
uint8_t AddclAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t AddclAccLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// AddcuAccLoc16
uint8_t AddcuAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t AddcuAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// AddlAccLoc32
uint8_t AddlAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t AddlAccLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// AddlLoc32Acc
uint8_t AddlLoc32Acc::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t AddlLoc32Acc::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// AdduAccLoc16
uint8_t AdduAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t AdduAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// AddulPLoc32
uint8_t AddulPLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t AddulPLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// AddulAccLoc32
uint8_t AddulAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t AddulAccLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// AdrkImm8
uint8_t AdrkImm8::GetImm8(const uint32_t data) { return data & 0xFFu; }

uint32_t AdrkImm8::SetImm8(const uint8_t imm8) { return opcode | imm8; }

// AndAccConst16Shift0_15
uint16_t AndAccConst16Shift0_15::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t AndAccConst16Shift0_15::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t AndAccConst16Shift0_15::GetShift(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t AndAccConst16Shift0_15::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 16;
}

// AndAccConst16Shift16
uint16_t AndAccConst16Shift16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t AndAccConst16Shift16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// AndAccLoc16
uint8_t AndAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t AndAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// AndAxLoc16Const16
uint8_t AndAxLoc16Const16::GetRegAx(const uint32_t data) {
  return (data & 0x1000000u) >> 24;
}

uint32_t AndAxLoc16Const16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 24;
}

uint8_t AndAxLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t AndAxLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t AndAxLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t AndAxLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// AndIerConst16
uint16_t AndIerConst16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t AndIerConst16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// AndIfrConst16
uint16_t AndIfrConst16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t AndIfrConst16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// AndLoc16Ax
uint8_t AndLoc16Ax::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t AndLoc16Ax::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t AndLoc16Ax::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t AndLoc16Ax::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// AndAxLoc16
uint8_t AndAxLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t AndAxLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t AndAxLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t AndAxLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// AndLoc16Const16
uint8_t AndLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t AndLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t AndLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t AndLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// AndbAxConst8
uint8_t AndbAxConst8::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t AndbAxConst8::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t AndbAxConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t AndbAxConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// AsrAxShift
uint8_t AsrAxShift::GetRegAx(const uint32_t data) {
  return (data & 0x10u) >> 4;
}

uint32_t AsrAxShift::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 4;
}

uint8_t AsrAxShift::GetShift(const uint32_t data) { return data & 0xFu; }

uint32_t AsrAxShift::SetShift(const uint8_t shift) {
  return opcode | shift & 0xFu;
}

// AsrAxT
uint8_t AsrAxT::GetRegAx(const uint32_t data) { return data & 0x1u; }

uint32_t AsrAxT::SetRegAx(const uint8_t x) { return opcode | x & 0x1u; }

// Asr64AccPShift
uint8_t Asr64AccPShift::GetShift(const uint32_t data) { return data & 0xFu; }

uint32_t Asr64AccPShift::SetShift(const uint8_t shift) {
  return opcode | shift & 0xFu;
}

// BOff16Cond
uint16_t BOff16Cond::GetOff16(const uint32_t data) { return data & 0xFFFFu; }

uint32_t BOff16Cond::SetOff16(const uint16_t off16) { return opcode | off16; }

uint8_t BOff16Cond::GetCond(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t BOff16Cond::SetCond(const uint8_t cond) {
  return opcode | (cond & 0xFu) << 16;
}

uint64_t BOff16Cond::GetTarget(const uint32_t data, const uint64_t addr) {
  return addr + static_cast<int16_t>(GetOff16(data));
}

// BanzOff16Arn
uint16_t BanzOff16Arn::GetOff16(const uint32_t data) { return data & 0xFFFFu; }

uint32_t BanzOff16Arn::SetOff16(const uint16_t off16) { return opcode | off16; }

uint8_t BanzOff16Arn::GetRegN(const uint32_t data) {
  return (data & 0x70000u) >> 16;
}

uint32_t BanzOff16Arn::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 16;
}

uint64_t BanzOff16Arn::GetTarget(const uint32_t data, const uint64_t addr) {
  return addr + static_cast<int16_t>(GetOff16(data));
}

// BarOff16ArnArmEq
uint16_t BarOff16ArnArmEq::GetOff16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t BarOff16ArnArmEq::SetOff16(const uint16_t off16) {
  return opcode | off16;
}

uint8_t BarOff16ArnArmEq::GetRegN(const uint32_t data) {
  return (data & 0x380000u) >> 19;
}

uint32_t BarOff16ArnArmEq::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 19;
}

uint8_t BarOff16ArnArmEq::GetRegM(const uint32_t data) {
  return (data & 0x70000u) >> 16;
}

uint32_t BarOff16ArnArmEq::SetRegM(const uint8_t m) {
  return opcode | (m & 0x7u) << 16;
}

uint64_t BarOff16ArnArmEq::GetTarget(const uint32_t data, const uint64_t addr) {
  return addr + static_cast<int16_t>(GetOff16(data));
}

// BarOff16ArnArmNeq
uint16_t BarOff16ArnArmNeq::GetOff16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t BarOff16ArnArmNeq::SetOff16(const uint16_t off16) {
  return opcode | off16;
}

uint8_t BarOff16ArnArmNeq::GetRegN(const uint32_t data) {
  return (data & 0x380000u) >> 19;
}

uint32_t BarOff16ArnArmNeq::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 19;
}

uint8_t BarOff16ArnArmNeq::GetRegM(const uint32_t data) {
  return (data & 0x70000u) >> 16;
}

uint32_t BarOff16ArnArmNeq::SetRegM(const uint8_t m) {
  return opcode | (m & 0x7u) << 16;
}

uint64_t BarOff16ArnArmNeq::GetTarget(const uint32_t data,
                                      const uint64_t addr) {
  return addr + static_cast<int16_t>(GetOff16(data));
}

// BfOff16Cond
uint16_t BfOff16Cond::GetOff16(const uint32_t data) { return data & 0xFFFFu; }

uint32_t BfOff16Cond::SetOff16(const uint16_t off16) { return opcode | off16; }

uint8_t BfOff16Cond::GetCond(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t BfOff16Cond::SetCond(const uint8_t cond) {
  return opcode | (cond & 0xFu) << 16;
}

uint64_t BfOff16Cond::GetTarget(const uint32_t data, const uint64_t addr) {
  return addr + static_cast<int16_t>(GetOff16(data));
}

// ClrcMode
uint8_t ClrcMode::GetMode(const uint32_t data) { return data & 0xFFu; }

uint32_t ClrcMode::SetMode(const uint8_t mode) { return opcode | mode; }

// CmpAxLoc16
uint8_t CmpAxLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t CmpAxLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t CmpAxLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t CmpAxLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// CmpLoc16Const16
uint8_t CmpLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t CmpLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t CmpLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t CmpLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// CmpbAxConst8
uint8_t CmpbAxConst8::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t CmpbAxConst8::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t CmpbAxConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t CmpbAxConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// CmplAccLoc32
uint8_t CmplAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t CmplAccLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// DecLoc16
uint8_t DecLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t DecLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// DmacAccPLoc32Xar7
uint8_t DmacAccPLoc32Xar7::GetLoc32(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t DmacAccPLoc32Xar7::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// DmacAccPLoc32Xar7Postinc
uint8_t DmacAccPLoc32Xar7Postinc::GetLoc32(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t DmacAccPLoc32Xar7Postinc::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// DmovLoc16
uint8_t DmovLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t DmovLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// FfcXar7Const22
uint32_t FfcXar7Const22::GetConst22(const uint32_t data) {
  return data & 0x3FFFFFu;
}

uint32_t FfcXar7Const22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

uint64_t FfcXar7Const22::GetTarget(const uint32_t data, const uint64_t addr) {
  return GetConst22(data);
}

// FlipAx
uint8_t FlipAx::GetRegAx(const uint32_t data) { return data & 0x1u; }

uint32_t FlipAx::SetRegAx(const uint8_t x) { return opcode | x & 0x1u; }

// IackConst16
uint16_t IackConst16::GetConst16(const uint32_t data) { return data & 0xFFFFu; }

uint32_t IackConst16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// ImaclPLoc32Xar7
uint8_t ImaclPLoc32Xar7::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t ImaclPLoc32Xar7::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// ImaclPLoc32Xar7Postinc
uint8_t ImaclPLoc32Xar7Postinc::GetLoc32(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t ImaclPLoc32Xar7Postinc::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// ImpyalPXtLoc32
uint8_t ImpyalPXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t ImpyalPXtLoc32::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// ImpylAccXtLoc32
uint8_t ImpylAccXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t ImpylAccXtLoc32::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// ImpylPXtLoc32
uint8_t ImpylPXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t ImpylPXtLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// ImpyslPXtLoc32
uint8_t ImpyslPXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t ImpyslPXtLoc32::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// ImpyxulPXtLoc32
uint8_t ImpyxulPXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t ImpyxulPXtLoc32::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// InLoc16Pa
uint8_t InLoc16Pa::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t InLoc16Pa::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t InLoc16Pa::GetConst16(const uint32_t data) { return data & 0xFFFFu; }

uint32_t InLoc16Pa::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// IncLoc16
uint8_t IncLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t IncLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// IntrIntx
uint8_t IntrIntx::GetIntrX(const uint32_t data) { return data & 0xFu; }

uint32_t IntrIntx::SetIntrX(const uint8_t x) { return opcode | x & 0xFu; }

// LbConst22
uint32_t LbConst22::GetConst22(const uint32_t data) { return data & 0x3FFFFFu; }

uint32_t LbConst22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

uint64_t LbConst22::GetTarget(const uint32_t data, const uint64_t addr) {
  return GetConst22(data);
}

// LcConst22
uint32_t LcConst22::GetConst22(const uint32_t data) { return data & 0x3FFFFFu; }

uint32_t LcConst22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

uint64_t LcConst22::GetTarget(const uint32_t data, const uint64_t addr) {
  return GetConst22(data);
}

// LcrConst22
uint32_t LcrConst22::GetConst22(const uint32_t data) {
  return data & 0x3FFFFFu;
}

uint32_t LcrConst22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

uint64_t LcrConst22::GetTarget(const uint32_t data, const uint64_t addr) {
  return GetConst22(data);
}

// LcrXarn
uint8_t LcrXarn::GetRegN(const uint32_t data) { return data & 0x7u; }

uint32_t LcrXarn::SetRegN(const uint8_t n) { return opcode | n & 0x7u; }

// LoopnzLoc16Const16
uint8_t LoopnzLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t LoopnzLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t LoopnzLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t LoopnzLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// LoopzLoc16Const16
uint8_t LoopzLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t LoopzLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t LoopzLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t LoopzLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// LslAccShift
uint8_t LslAccShift::GetShift(const uint32_t data) { return data & 0xFu; }

uint32_t LslAccShift::SetShift(const uint8_t shift) {
  return opcode | shift & 0xFu;
}

// LslAxShift
uint8_t LslAxShift::GetRegAx(const uint32_t data) {
  return (data & 0x10u) >> 4;
}

uint32_t LslAxShift::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 4;
}

uint8_t LslAxShift::GetShift(const uint32_t data) { return data & 0xFu; }

uint32_t LslAxShift::SetShift(const uint8_t shift) {
  return opcode | shift & 0xFu;
}

// LslAxT
uint8_t LslAxT::GetRegAx(const uint32_t data) { return data & 0x1u; }

uint32_t LslAxT::SetRegAx(const uint8_t x) { return opcode | x & 0x1u; }

// Lsl64AccPShift
uint8_t Lsl64AccPShift::GetShift(const uint32_t data) { return data & 0xFu; }

uint32_t Lsl64AccPShift::SetShift(const uint8_t shift) {
  return opcode | shift & 0xFu;
}

// LsrAxShift
uint8_t LsrAxShift::GetRegAx(const uint32_t data) {
  return (data & 0x10u) >> 4;
}

uint32_t LsrAxShift::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 4;
}

uint8_t LsrAxShift::GetShift(const uint32_t data) { return data & 0xFu; }

uint32_t LsrAxShift::SetShift(const uint8_t shift) {
  return opcode | shift & 0xFu;
}

// LsrAxT
uint8_t LsrAxT::GetRegAx(const uint32_t data) { return data & 0x1u; }

uint32_t LsrAxT::SetRegAx(const uint8_t x) { return opcode | x & 0x1u; }

// Lsr64AccPShift
uint8_t Lsr64AccPShift::GetShift(const uint32_t data) { return data & 0xFu; }

uint32_t Lsr64AccPShift::SetShift(const uint8_t shift) {
  return opcode | shift & 0xFu;
}

// MacPLoc16Pma
uint8_t MacPLoc16Pma::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t MacPLoc16Pma::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t MacPLoc16Pma::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t MacPLoc16Pma::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// MacPLoc16Xar7
uint8_t MacPLoc16Xar7::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MacPLoc16Xar7::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MacPLoc16Xar7Postinc
uint8_t MacPLoc16Xar7Postinc::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MacPLoc16Xar7Postinc::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MaxAxLoc16
uint8_t MaxAxLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x10000u) >> 16;
}

uint32_t MaxAxLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 16;
}

uint8_t MaxAxLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MaxAxLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MaxculPLoc32
uint8_t MaxculPLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MaxculPLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MaxlAccLoc32
uint8_t MaxlAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MaxlAccLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MinAxLoc16
uint8_t MinAxLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x10000u) >> 16;
}

uint32_t MinAxLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 16;
}

uint8_t MinAxLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MinAxLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MinculPLoc32
uint8_t MinculPLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MinculPLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MinlAccLoc32
uint8_t MinlAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MinlAccLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovConst16Loc16
uint16_t MovMem16Loc16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t MovMem16Loc16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t MovMem16Loc16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t MovMem16Loc16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

// MovAccConst16Shift
uint16_t MovAccConst16Shift::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t MovAccConst16Shift::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t MovAccConst16Shift::GetShift(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t MovAccConst16Shift::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 16;
}

// MovAccLoc16ShiftT
uint8_t MovAccLoc16ShiftT::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovAccLoc16ShiftT::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MovAccLoc16Objmode1
uint8_t MovAccLoc16Objmode1::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovAccLoc16Objmode1::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MovAccLoc16Objmode0
uint8_t MovAccLoc16Objmode0::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovAccLoc16Objmode0::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MovAccLoc16Shift1_15Objmode1
uint8_t MovAccLoc16Shift1_15Objmode1::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovAccLoc16Shift1_15Objmode1::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t MovAccLoc16Shift1_15Objmode1::GetShift(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t MovAccLoc16Shift1_15Objmode1::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 8;
}

// MovAccLoc16Shift1_15Objmode0
uint8_t MovAccLoc16Shift1_15Objmode0::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovAccLoc16Shift1_15Objmode0::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t MovAccLoc16Shift1_15Objmode0::GetShift(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t MovAccLoc16Shift1_15Objmode0::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 8;
}

// MovAccLoc16Shift16
uint8_t MovAccLoc16Shift16::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovAccLoc16Shift16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MovAr6Loc16
uint8_t MovAr6Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovAr6Loc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovAr7Loc16
uint8_t MovAr7Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovAr7Loc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovAxLoc16
uint8_t MovAxLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t MovAxLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t MovAxLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovAxLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovDpConst10
uint16_t MovDpConst10::GetConst10(const uint32_t data) { return data & 0x3FFu; }

uint32_t MovDpConst10::SetConst10(const uint16_t const10) {
  return opcode | const10 & 0x3FFu;
  ;
}

// MovIerLoc16
uint8_t MovIerLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovIerLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovLoc16Const16
uint8_t MovLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t MovLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t MovLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t MovLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// MovLoc16Mem16
uint8_t MovLoc16Mem16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t MovLoc16Mem16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t MovLoc16Mem16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t MovLoc16Mem16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// MovLoc16_0
uint8_t MovLoc16_0::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovLoc16_0::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovLoc16AccShift1
uint8_t MovLoc16AccShift1::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovLoc16AccShift1::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MovLoc16AccShift2_8Objmode1
uint8_t MovLoc16AccShift2_8Objmode1::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovLoc16AccShift2_8Objmode1::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t MovLoc16AccShift2_8Objmode1::GetShift(const uint32_t data) {
  return (data & 0x700u) >> 8;
}

uint32_t MovLoc16AccShift2_8Objmode1::SetShift(const uint8_t shift) {
  return opcode | (shift & 0x7) << 8;
}

// MovLoc16AccShift2_8Objmode0
uint8_t MovLoc16AccShift2_8Objmode0::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovLoc16AccShift2_8Objmode0::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t MovLoc16AccShift2_8Objmode0::GetShift(const uint32_t data) {
  return (data & 0x700u) >> 8;
}

uint32_t MovLoc16AccShift2_8Objmode0::SetShift(const uint8_t shift) {
  return opcode | (shift & 0x7) << 8;
}

// MovLoc16Arn
uint8_t MovLoc16Arn::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovLoc16Arn::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t MovLoc16Arn::GetRegN(const uint32_t data) {
  return (data & 0x700u) >> 8;
}

uint32_t MovLoc16Arn::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 8;
}

// MovLoc16Ax
uint8_t MovLoc16Ax::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovLoc16Ax::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t MovLoc16Ax::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t MovLoc16Ax::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// MovLoc16AxCond
uint8_t MovLoc16AxCond::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovLoc16AxCond::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t MovLoc16AxCond::GetRegAx(const uint32_t data) {
  return (data & 0x10000u) >> 16;
}

uint32_t MovLoc16AxCond::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 16;
}

uint8_t MovLoc16AxCond::GetCond(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t MovLoc16AxCond::SetCond(const uint8_t cond) {
  return opcode | (cond & 0xFu) << 8;
}

// MovLoc16Ier
uint8_t MovLoc16Ier::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovLoc16Ier::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovLoc16Ovc
uint8_t MovLoc16Ovc::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovLoc16Ovc::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovLoc16P
uint8_t MovLoc16P::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovLoc16P::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovLoc16T
uint8_t MovLoc16T::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovLoc16T::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovOvcLoc16
uint8_t MovOvcLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovOvcLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovPhLoc16
uint8_t MovPhLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovPhLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovPlLoc16
uint8_t MovPlLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovPlLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovPmAx
uint8_t MovPmAx::GetRegAx(const uint32_t data) { return data & 0x1u; }

uint32_t MovPmAx::SetRegAx(const uint8_t x) { return opcode | x & 0x1u; }

// MovTLoc16
uint8_t MovTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovXarnPc
uint8_t MovXarnPc::GetRegN(const uint32_t data) { return data & 0x7u; }

uint32_t MovXarnPc::SetRegN(const uint8_t n) { return opcode | n & 0x7u; }

// MovaTLoc16
uint8_t MovaTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovaTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovadTLoc16
uint8_t MovadTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovadTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovbAccConst8
uint8_t MovbAccConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbAccConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbAr6Const8
uint8_t MovbAr6Const8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbAr6Const8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbAr7Const8
uint8_t MovbAr7Const8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbAr7Const8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbAxConst8
uint8_t MovbAxConst8::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t MovbAxConst8::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t MovbAxConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbAxConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbAxlsbLoc16
uint8_t MovbAxlsbLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t MovbAxlsbLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t MovbAxlsbLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbAxlsbLoc16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MovbAxmsbLoc16
uint8_t MovbAxmsbLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t MovbAxmsbLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t MovbAxmsbLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbAxmsbLoc16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MovbLoc16Const8Cond
uint8_t MovbLoc16Const8Cond::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovbLoc16Const8Cond::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t MovbLoc16Const8Cond::GetConst8(const uint32_t data) {
  return (data & 0xFF00u) >> 8;
}

uint32_t MovbLoc16Const8Cond::SetConst8(const uint8_t const8) {
  return opcode | const8 << 8;
}

uint8_t MovbLoc16Const8Cond::GetCond(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t MovbLoc16Const8Cond::SetCond(const uint8_t cond) {
  return opcode | (cond & 0xFu) << 16;
}

// MovbLoc16Axlsb
uint8_t MovbLoc16Axlsb::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t MovbLoc16Axlsb::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t MovbLoc16Axlsb::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbLoc16Axlsb::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MovbLoc16Axmsb
uint8_t MovbLoc16Axmsb::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t MovbLoc16Axmsb::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t MovbLoc16Axmsb::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbLoc16Axmsb::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MovbXar0Const8
uint8_t MovbXar0Const8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbXar0Const8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbXar1Const8
uint8_t MovbXar1Const8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbXar1Const8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbXar2Const8
uint8_t MovbXar2Const8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbXar2Const8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbXar3Const8
uint8_t MovbXar3Const8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbXar3Const8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbXar4Const8
uint8_t MovbXar4Const8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbXar4Const8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbXar5Const8
uint8_t MovbXar5Const8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbXar5Const8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbXar6Const8
uint8_t MovbXar6Const8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbXar6Const8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovbXar7Const8
uint8_t MovbXar7Const8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MovbXar7Const8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MovdlXtLoc32
uint8_t MovdlXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovdlXtLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovhLoc16AccShift1
uint8_t MovhLoc16AccShift1::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovhLoc16AccShift1::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MovhLoc16AccShift2_8Objmode1
uint8_t MovhLoc16AccShift2_8Objmode1::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovhLoc16AccShift2_8Objmode1::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t MovhLoc16AccShift2_8Objmode1::GetShift(const uint32_t data) {
  return (data & 0x700u) >> 8;
}

uint32_t MovhLoc16AccShift2_8Objmode1::SetShift(const uint8_t shift) {
  return opcode | (shift & 0x7) << 8;
}

// MovhLoc16AccShift2_8Objmode0
uint8_t MovhLoc16AccShift2_8Objmode0::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t MovhLoc16AccShift2_8Objmode0::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t MovhLoc16AccShift2_8Objmode0::GetShift(const uint32_t data) {
  return (data & 0x700u) >> 8;
}

uint32_t MovhLoc16AccShift2_8Objmode0::SetShift(const uint8_t shift) {
  return opcode | (shift & 0x7) << 8;
}

// MovhLoc16P
uint8_t MovhLoc16P::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovhLoc16P::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovlAccLoc32
uint8_t MovlAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlAccLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32Acc
uint8_t MovlLoc32Acc::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32Acc::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32AccCond
uint8_t MovlLoc32AccCond::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32AccCond::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

uint8_t MovlLoc32AccCond::GetCond(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t MovlLoc32AccCond::SetCond(const uint8_t cond) {
  return opcode | (cond & 0xFu) << 8;
}

// MovlLoc32P
uint8_t MovlLoc32P::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32P::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32Xar0
uint8_t MovlLoc32Xar0::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32Xar0::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32Xar1
uint8_t MovlLoc32Xar1::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32Xar1::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32Xar2
uint8_t MovlLoc32Xar2::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32Xar2::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32Xar3
uint8_t MovlLoc32Xar3::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32Xar3::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32Xar4
uint8_t MovlLoc32Xar4::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32Xar4::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32Xar5
uint8_t MovlLoc32Xar5::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32Xar5::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32Xar6
uint8_t MovlLoc32Xar6::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32Xar6::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32Xar7
uint8_t MovlLoc32Xar7::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32Xar7::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlLoc32Xt
uint8_t MovlLoc32Xt::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlLoc32Xt::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlPLoc32
uint8_t MovlPLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlPLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlXar0Loc32
uint8_t MovlXar0Loc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlXar0Loc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlXar1Loc32
uint8_t MovlXar1Loc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlXar1Loc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlXar2Loc32
uint8_t MovlXar2Loc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlXar2Loc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlXar3Loc32
uint8_t MovlXar3Loc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlXar3Loc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlXar4Loc32
uint8_t MovlXar4Loc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlXar4Loc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlXar5Loc32
uint8_t MovlXar5Loc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlXar5Loc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlXar6Loc32
uint8_t MovlXar6Loc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlXar6Loc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlXar7Loc32
uint8_t MovlXar7Loc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlXar7Loc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovlXar0Const22
uint32_t MovlXar0Const22::GetConst22(const uint32_t data) {
  return data & 0x3FFFFFu;
}

uint32_t MovlXar0Const22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

// MovlXar1Const22
uint32_t MovlXar1Const22::GetConst22(const uint32_t data) {
  return data & 0x3FFFFFu;
}

uint32_t MovlXar1Const22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

// MovlXar2Const22
uint32_t MovlXar2Const22::GetConst22(const uint32_t data) {
  return data & 0x3FFFFFu;
}

uint32_t MovlXar2Const22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

// MovlXar3Const22
uint32_t MovlXar3Const22::GetConst22(const uint32_t data) {
  return data & 0x3FFFFFu;
}

uint32_t MovlXar3Const22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

// MovlXar4Const22
uint32_t MovlXar4Const22::GetConst22(const uint32_t data) {
  return data & 0x3FFFFFu;
}

uint32_t MovlXar4Const22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

// MovlXar5Const22
uint32_t MovlXar5Const22::GetConst22(const uint32_t data) {
  return data & 0x3FFFFFu;
}

uint32_t MovlXar5Const22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

// MovlXar6Const22
uint32_t MovlXar6Const22::GetConst22(const uint32_t data) {
  return data & 0x3FFFFFu;
}

uint32_t MovlXar6Const22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

// MovlXar7Const22
uint32_t MovlXar7Const22::GetConst22(const uint32_t data) {
  return data & 0x3FFFFFu;
}

uint32_t MovlXar7Const22::SetConst22(const uint32_t const22) {
  return opcode | const22 & 0x3FFFFFu;
}

// MovlXtLoc32
uint8_t MovlXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t MovlXtLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// MovpTLoc16
uint8_t MovpTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovpTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovsTLoc16
uint8_t MovsTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovsTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovuAccLoc16
uint8_t MovuAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovuAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovuLoc16Ovc
uint8_t MovuLoc16Ovc::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovuLoc16Ovc::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovuOvcLoc16
uint8_t MovuOvcLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovuOvcLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovwDpConst16
uint16_t MovwDpConst16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t MovwDpConst16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// MovxTlLoc16
uint8_t MovxTlLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovxTlLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovzAr0Loc16
uint8_t MovzAr0Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovzAr0Loc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovzAr1Loc16
uint8_t MovzAr1Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovzAr1Loc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovzAr2Loc16
uint8_t MovzAr2Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovzAr2Loc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovzAr3Loc16
uint8_t MovzAr3Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovzAr3Loc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovzAr4Loc16
uint8_t MovzAr4Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovzAr4Loc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovzAr5Loc16
uint8_t MovzAr5Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovzAr5Loc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovzAr6Loc16
uint8_t MovzAr6Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovzAr6Loc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovzAr7Loc16
uint8_t MovzAr7Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MovzAr7Loc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MovzDpConst10
uint16_t MovzDpConst10::GetConst10(const uint32_t data) {
  return data & 0x3FFu;
}

uint32_t MovzDpConst10::SetConst10(const uint16_t const10) {
  return opcode | const10 & 0x3FFu;
}

// MpyAccLoc16Const16
uint8_t MpyAccLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t MpyAccLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t MpyAccLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t MpyAccLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// MpyAccTLoc16
uint8_t MpyAccTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MpyAccTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MpyPLoc16Const16
uint8_t MpyPLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t MpyPLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t MpyPLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t MpyPLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// MpyPTLoc16
uint8_t MpyPTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MpyPTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MpyaPLoc16Const16
uint8_t MpyaPLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t MpyaPLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t MpyaPLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t MpyaPLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// MpyaPTLoc16
uint8_t MpyaPTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MpyaPTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MpybAccTConst8
uint8_t MpybAccTConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MpybAccTConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MpybPTConst8
uint8_t MpybPTConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t MpybPTConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// MpysPTLoc16
uint8_t MpysPTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MpysPTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MpyuPTLoc16
uint8_t MpyuPTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MpyuPTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MpyuAccTLoc16
uint8_t MpyuAccTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MpyuAccTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// MpyxuAccTLoc16
uint8_t MpyxuAccTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MpyxuAccTLoc16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// MpyxuPTLoc16
uint8_t MpyxuPTLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t MpyxuPTLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// NegAx
uint8_t NegAx::GetRegAx(const uint32_t data) { return data & 0x1u; }

uint32_t NegAx::SetRegAx(const uint8_t x) { return opcode | x & 0x1u; }

// NopIndArpn
uint8_t NopIndArpn::GetInd(const uint32_t data) { return data & 0xFFu; }

uint32_t NopIndArpn::SetInd(const uint8_t ind) { return opcode | ind; }

// NormAccXarnPostinc
uint8_t NormAccXarnPostinc::GetRegN(const uint32_t data) { return data & 0x7u; }

uint32_t NormAccXarnPostinc::SetRegN(const uint8_t n) {
  return opcode | n & 0x7u;
}

// NormAccXarnPostdec
uint8_t NormAccXarnPostdec::GetRegN(const uint32_t data) { return data & 0x7u; }

uint32_t NormAccXarnPostdec::SetRegN(const uint8_t n) {
  return opcode | n & 0x7u;
}

// NotAx
uint8_t NotAx::GetRegAx(const uint32_t data) { return data & 0x1u; }

uint32_t NotAx::SetRegAx(const uint8_t x) { return opcode | x & 0x1u; }

// OrAccLoc16
uint8_t OrAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t OrAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// OrAccConst16Shift0_15
uint8_t OrAccConst16Shift0_15::GetShift(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t OrAccConst16Shift0_15::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 16;
}

uint16_t OrAccConst16Shift0_15::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t OrAccConst16Shift0_15::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// OrAccConst16Shift16
uint16_t OrAccConst16Shift16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t OrAccConst16Shift16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// OrAxLoc16
uint8_t OrAxLoc16::GetLoc16(const uint32_t data) { return data & 0xFF; }

uint32_t OrAxLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t OrAxLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t OrAxLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// OrIerConst16
uint16_t OrIerConst16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t OrIerConst16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// OrIfrConst16
uint16_t OrIfrConst16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t OrIfrConst16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// OrLoc16Const16
uint8_t OrLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t OrLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t OrLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t OrLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// OrLoc16Ax
uint8_t OrLoc16Ax::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t OrLoc16Ax::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t OrLoc16Ax::GetLoc16(const uint32_t data) { return data & 0xFF; }

uint32_t OrLoc16Ax::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// OrbAxConst8
uint8_t OrbAxConst8::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t OrbAxConst8::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t OrbAxConst8::GetConst8(const uint32_t data) { return data & 0xFF; }

uint32_t OrbAxConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// OutPaLoc16
uint8_t OutPaLoc16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t OutPaLoc16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t OutPaLoc16::GetConst16(const uint32_t data) { return data & 0xFFFFu; }

uint32_t OutPaLoc16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// PopLoc16
uint8_t PopLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t PopLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// PreadLoc16Xar7
uint8_t PreadLoc16Xar7::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t PreadLoc16Xar7::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// PushLoc16
uint8_t PushLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t PushLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// PwriteXar7Loc16
uint8_t PwriteXar7Loc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t PwriteXar7Loc16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// QmaclPLoc32Xar7
uint8_t QmaclPLoc32Xar7::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t QmaclPLoc32Xar7::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// QmaclPLoc32Xar7Postinc
uint8_t QmaclPLoc32Xar7Postinc::GetLoc32(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t QmaclPLoc32Xar7Postinc::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// QmpyalPXtLoc32
uint8_t QmpyalPXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t QmpyalPXtLoc32::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// QmpylPXtLoc32
uint8_t QmpylPXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t QmpylPXtLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// QmpylAccXtLoc32
uint8_t QmpylAccXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t QmpylAccXtLoc32::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// QmpyslPXtLoc32
uint8_t QmpyslPXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t QmpyslPXtLoc32::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// QmpyulPXtLoc32
uint8_t QmpyulPXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t QmpyulPXtLoc32::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// QmpyxulPXtLoc32
uint8_t QmpyxulPXtLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t QmpyxulPXtLoc32::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// RptConst8
uint8_t RptConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t RptConst8::SetConst8(const uint8_t const8) { return opcode | const8; }

// RptLoc16
uint8_t RptLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t RptLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// SbOff8Cond
uint8_t SbOff8Cond::GetOff8(const uint32_t data) { return data & 0xFFu; }

uint32_t SbOff8Cond::SetOff8(const uint8_t off8) { return opcode | off8; }

uint8_t SbOff8Cond::GetCond(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t SbOff8Cond::SetCond(const uint8_t cond) {
  return opcode | (cond & 0xFu) << 8;
}

uint64_t SbOff8Cond::GetTarget(const uint32_t data, const uint64_t addr) {
  return addr + static_cast<int16_t>(GetOff8(data));
}

// SbbuAccLoc16
uint8_t SbbuAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t SbbuAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// SbfOff8Eq
uint8_t SbfOff8Eq::GetOff8(const uint32_t data) { return data & 0xFFu; }

uint32_t SbfOff8Eq::SetOff8(const uint8_t off8) { return opcode | off8; }

uint64_t SbfOff8Eq::GetTarget(const uint32_t data, const uint64_t addr) {
  return addr + static_cast<int16_t>(GetOff8(data));
}

// SbfOff8Neq
uint8_t SbfOff8Neq::GetOff8(const uint32_t data) { return data & 0xFFu; }

uint32_t SbfOff8Neq::SetOff8(const uint8_t off8) { return opcode | off8; }

uint64_t SbfOff8Neq::GetTarget(const uint32_t data, const uint64_t addr) {
  return addr + static_cast<int16_t>(GetOff8(data));
}

// SbfOff8Tc
uint8_t SbfOff8Tc::GetOff8(const uint32_t data) { return data & 0xFFu; }

uint32_t SbfOff8Tc::SetOff8(const uint8_t off8) { return opcode | off8; }

uint64_t SbfOff8Tc::GetTarget(const uint32_t data, const uint64_t addr) {
  return addr + static_cast<int16_t>(GetOff8(data));
}

// SbfOff8Ntc
uint8_t SbfOff8Ntc::GetOff8(const uint32_t data) { return data & 0xFFu; }

uint32_t SbfOff8Ntc::SetOff8(const uint8_t off8) { return opcode | off8; }

uint64_t SbfOff8Ntc::GetTarget(const uint32_t data, const uint64_t addr) {
  return addr + static_cast<int16_t>(GetOff8(data));
}

// SbrkConst8
uint8_t SbrkConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t SbrkConst8::SetConst8(const uint8_t const8) { return opcode | const8; }

// SetcMode
uint8_t SetcMode::GetMode(const uint32_t data) { return data & 0xFFu; }

uint32_t SetcMode::SetMode(const uint8_t mode) { return opcode | mode; }

// SfrAccShift
uint8_t SfrAccShift::GetShift(const uint32_t data) { return data & 0xFu; }

uint32_t SfrAccShift::SetShift(const uint8_t shift) {
  return opcode | shift & 0xFu;
}

// SpmShift
uint8_t SpmShift::GetPM(const uint32_t data) { return data & 0x7u; }

uint32_t SpmShift::SetPM(const uint8_t pm) { return opcode | pm & 0x7u; }

// SqraLoc16
uint8_t SqraLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t SqraLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// SqrsLoc16
uint8_t SqrsLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t SqrsLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// SubAccLoc16Objmode1
uint8_t SubAccLoc16Objmode1::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t SubAccLoc16Objmode1::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// SubAccLoc16Objmode0
uint8_t SubAccLoc16Objmode0::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t SubAccLoc16Objmode0::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// SubAccLoc16Shift1_15Objmode1
uint8_t SubAccLoc16Shift1_15Objmode1::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t SubAccLoc16Shift1_15Objmode1::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t SubAccLoc16Shift1_15Objmode1::GetShift(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t SubAccLoc16Shift1_15Objmode1::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 8;
}

// SubAccLoc16Shift1_15Objmode0
uint8_t SubAccLoc16Shift1_15Objmode0::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t SubAccLoc16Shift1_15Objmode0::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

uint8_t SubAccLoc16Shift1_15Objmode0::GetShift(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t SubAccLoc16Shift1_15Objmode0::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 8;
}

// SubAccLoc16Shift16
uint8_t SubAccLoc16Shift16::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t SubAccLoc16Shift16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// SubAccLoc16ShiftT
uint8_t SubAccLoc16ShiftT::GetLoc16(const uint32_t data) {
  return data & 0xFFu;
}

uint32_t SubAccLoc16ShiftT::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// SubAccConst16Shift
uint16_t SubAccConst16Shift::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t SubAccConst16Shift::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t SubAccConst16Shift::GetShift(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t SubAccConst16Shift::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 16;
}

// SubAxLoc16
uint8_t SubAxLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t SubAxLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t SubAxLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t SubAxLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// SubLoc16Ax
uint8_t SubLoc16Ax::GetLoc16(const uint32_t data) { return data & 0xFF; }

uint32_t SubLoc16Ax::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t SubLoc16Ax::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t SubLoc16Ax::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// SubbAccConst8
uint8_t SubbAccConst8::GetConst8(const uint32_t data) { return data & 0xFFu; }

uint32_t SubbAccConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// SubSpConst7
uint8_t SubbSpConst7::GetConst7(const uint32_t data) { return data & 0x7Fu; }

uint32_t SubbSpConst7::SetConst7(const uint8_t const7) {
  return opcode | const7 & 0x7Fu;
}

// SubbXarnConst7
uint8_t SubbXarnConst7::GetRegN(const uint32_t data) {
  return (data & 0x700u) >> 8;
}

uint32_t SubbXarnConst7::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 8;
}

uint8_t SubbXarnConst7::GetConst7(const uint32_t data) { return data & 0x7Fu; }

uint32_t SubbXarnConst7::SetConst7(const uint8_t const7) {
  return opcode | const7 & 0x7Fu;
}

// SubblAccLoc32
uint8_t SubblAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t SubblAccLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// SubcuAccLoc16
uint8_t SubcuAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t SubcuAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// SubculAccLoc32
uint8_t SubculAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t SubculAccLoc32::SetLoc32(const uint8_t loc32) {
  return opcode | loc32;
}

// SublAccLoc32
uint8_t SublAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t SublAccLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// SublLoc32Acc
uint8_t SublLoc32Acc::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t SublLoc32Acc::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// SubrLoc16Ax
uint8_t SubrLoc16Ax::GetLoc16(const uint32_t data) { return data & 0xFF; }

uint32_t SubrLoc16Ax::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t SubrLoc16Ax::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t SubrLoc16Ax::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// SubrlLoc32Acc
uint8_t SubrlLoc32Acc::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t SubrlLoc32Acc::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// SubuAccLoc16
uint8_t SubuAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFF; }

uint32_t SubuAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// SubulAccLoc32
uint8_t SubulAccLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t SubulAccLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// SubulPLoc32
uint8_t SubulPLoc32::GetLoc32(const uint32_t data) { return data & 0xFFu; }

uint32_t SubulPLoc32::SetLoc32(const uint8_t loc32) { return opcode | loc32; }

// TbitLoc16Bit
uint8_t TbitLoc16Bit::GetLoc16(const uint32_t data) { return data & 0xFF; }

uint32_t TbitLoc16Bit::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t TbitLoc16Bit::GetBitIndex(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t TbitLoc16Bit::SetBitIndex(const uint8_t bit) {
  return opcode | (bit & 0xFu) << 8;
}

// TbitLoc16T
uint8_t TbitLoc16T::GetLoc16(const uint32_t data) { return data & 0xFF; }

uint32_t TbitLoc16T::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// TclrLoc16Bit
uint8_t TclrLoc16Bit::GetLoc16(const uint32_t data) { return data & 0xFF; }

uint32_t TclrLoc16Bit::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t TclrLoc16Bit::GetBitIndex(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t TclrLoc16Bit::SetBitIndex(const uint8_t bit) {
  return opcode | (bit & 0xFu) << 8;
}

// TrapVec
uint8_t TrapVec::GetVec(const uint32_t data) { return data & 0x1Fu; }

uint32_t TrapVec::SetVec(const uint8_t vec) { return opcode | vec & 0x1Fu; }

// TsetLoc16Bit
uint8_t TsetLoc16Bit::GetLoc16(const uint32_t data) { return data & 0xFF; }

uint32_t TsetLoc16Bit::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t TsetLoc16Bit::GetBitIndex(const uint32_t data) {
  return (data & 0xF00u) >> 8;
}

uint32_t TsetLoc16Bit::SetBitIndex(const uint8_t bit) {
  return opcode | (bit & 0xFu) << 8;
}

// UoutPaLoc16
uint8_t UoutPaLoc16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t UoutPaLoc16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t UoutPaLoc16::GetConst16(const uint32_t data) { return data & 0xFFFFu; }

uint32_t UoutPaLoc16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// XbPmaArpn
uint16_t XbPmaArpn::GetConst16(const uint32_t data) { return data & 0xFFFFu; }

uint32_t XbPmaArpn::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t XbPmaArpn::GetRegN(const uint32_t data) {
  return (data & 0x70000u) >> 16;
}

uint32_t XbPmaArpn::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 16;
}

uint64_t XbPmaArpn::GetTarget(const uint32_t data, const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbPmaCond
uint16_t XbPmaCond::GetConst16(const uint32_t data) { return data & 0xFFFFu; }

uint32_t XbPmaCond::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t XbPmaCond::GetCond(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t XbPmaCond::SetCond(const uint8_t cond) {
  return opcode | (cond & 0xFu) << 16;
}

uint64_t XbPmaCond::GetTarget(const uint32_t data, const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbanzPmaInd
uint16_t XbanzPmaInd::GetConst16(const uint32_t data) { return data & 0xFFFFu; }

uint32_t XbanzPmaInd::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint64_t XbanzPmaInd::GetTarget(const uint32_t data, const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbanzPmaIndPostinc
uint16_t XbanzPmaIndPostinc::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XbanzPmaIndPostinc::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint64_t XbanzPmaIndPostinc::GetTarget(const uint32_t data,
                                       const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbanzPmaIndPostdec
uint16_t XbanzPmaIndPostdec::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XbanzPmaIndPostdec::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint64_t XbanzPmaIndPostdec::GetTarget(const uint32_t data,
                                       const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbanzPmaInd0Postinc
uint16_t XbanzPmaInd0Postinc::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XbanzPmaInd0Postinc::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint64_t XbanzPmaInd0Postinc::GetTarget(const uint32_t data,
                                        const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbanzPmaInd0Postdec
uint16_t XbanzPmaInd0Postdec::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XbanzPmaInd0Postdec::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint64_t XbanzPmaInd0Postdec::GetTarget(const uint32_t data,
                                        const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbanzPmaIndArpn
uint16_t XbanzPmaIndArpn::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XbanzPmaIndArpn::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t XbanzPmaIndArpn::GetRegN(const uint32_t data) {
  return (data & 0x70000u) >> 16;
}

uint32_t XbanzPmaIndArpn::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 16;
}

uint64_t XbanzPmaIndArpn::GetTarget(const uint32_t data, const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbanzPmaIndPostincArpn
uint16_t XbanzPmaIndPostincArpn::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XbanzPmaIndPostincArpn::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t XbanzPmaIndPostincArpn::GetRegN(const uint32_t data) {
  return (data & 0x70000u) >> 16;
}

uint32_t XbanzPmaIndPostincArpn::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 16;
}

uint64_t XbanzPmaIndPostincArpn::GetTarget(const uint32_t data,
                                           const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbanzPmaIndPostdecArpn
uint16_t XbanzPmaIndPostdecArpn::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XbanzPmaIndPostdecArpn::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t XbanzPmaIndPostdecArpn::GetRegN(const uint32_t data) {
  return (data & 0x70000u) >> 16;
}

uint32_t XbanzPmaIndPostdecArpn::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 16;
}

uint64_t XbanzPmaIndPostdecArpn::GetTarget(const uint32_t data,
                                           const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbanzPmaInd0PostincArpn
uint16_t XbanzPmaInd0PostincArpn::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XbanzPmaInd0PostincArpn::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t XbanzPmaInd0PostincArpn::GetRegN(const uint32_t data) {
  return (data & 0x70000u) >> 16;
}

uint32_t XbanzPmaInd0PostincArpn::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 16;
}

uint64_t XbanzPmaInd0PostincArpn::GetTarget(const uint32_t data,
                                            const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XbanzPmaInd0PostdecArpn
uint16_t XbanzPmaInd0PostdecArpn::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XbanzPmaInd0PostdecArpn::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t XbanzPmaInd0PostdecArpn::GetRegN(const uint32_t data) {
  return (data & 0x70000u) >> 16;
}

uint32_t XbanzPmaInd0PostdecArpn::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 16;
}

uint64_t XbanzPmaInd0PostdecArpn::GetTarget(const uint32_t data,
                                            const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XcallPmaArpn
uint16_t XcallPmaArpn::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XcallPmaArpn::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t XcallPmaArpn::GetRegN(const uint32_t data) {
  return (data & 0x70000u) >> 16;
}

uint32_t XcallPmaArpn::SetRegN(const uint8_t n) {
  return opcode | (n & 0x7u) << 16;
}

uint64_t XcallPmaArpn::GetTarget(const uint32_t data, const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XcallPmaCond
uint16_t XcallPmaCond::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XcallPmaCond::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t XcallPmaCond::GetCond(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t XcallPmaCond::SetCond(const uint8_t cond) {
  return opcode | (cond & 0xFu) << 16;
}

uint64_t XcallPmaCond::GetTarget(const uint32_t data, const uint64_t addr) {
  return 0x3F0000u + GetConst16(data);
}

// XmacPLoc16Pma
uint8_t XmacPLoc16Pma::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t XmacPLoc16Pma::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t XmacPLoc16Pma::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XmacPLoc16Pma::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// XmacdPLoc16Pma
uint8_t XmacdPLoc16Pma::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t XmacdPLoc16Pma::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t XmacdPLoc16Pma::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XmacdPLoc16Pma::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// XorAccLoc16
uint8_t XorAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t XorAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// XorAccConst16Shift0_15
uint16_t XorAccConst16Shift0_15::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XorAccConst16Shift0_15::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

uint8_t XorAccConst16Shift0_15::GetShift(const uint32_t data) {
  return (data & 0xF0000u) >> 16;
}

uint32_t XorAccConst16Shift0_15::SetShift(const uint8_t shift) {
  return opcode | (shift & 0xFu) << 16;
}

// XorAccConst16Shift16
uint16_t XorAccConst16Shift16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XorAccConst16Shift16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// XorAxLoc16
uint8_t XorAxLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t XorAxLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t XorAxLoc16::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t XorAxLoc16::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// XorLoc16Ax
uint8_t XorLoc16Ax::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t XorLoc16Ax::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

uint8_t XorLoc16Ax::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t XorLoc16Ax::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

// XorLoc16Const16
uint8_t XorLoc16Const16::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t XorLoc16Const16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t XorLoc16Const16::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XorLoc16Const16::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// XorbAxConst8
uint8_t XorbAxConst8::GetRegAx(const uint32_t data) {
  return (data & 0x100u) >> 8;
}

uint32_t XorbAxConst8::SetRegAx(const uint8_t x) {
  return opcode | (x & 0x1u) << 8;
}

uint8_t XorbAxConst8::GetConst8(const uint32_t data) { return data & 0xFF; }

uint32_t XorbAxConst8::SetConst8(const uint8_t const8) {
  return opcode | const8;
}

// XpreadLoc16Pma
uint8_t XpreadLoc16Pma::GetLoc16(const uint32_t data) {
  return (data & 0xFF0000u) >> 16;
}

uint32_t XpreadLoc16Pma::SetLoc16(const uint8_t loc16) {
  return opcode | loc16 << 16;
}

uint16_t XpreadLoc16Pma::GetConst16(const uint32_t data) {
  return data & 0xFFFFu;
}

uint32_t XpreadLoc16Pma::SetConst16(const uint16_t const16) {
  return opcode | const16;
}

// XpreadLoc16Al
uint8_t XpreadLoc16Al::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t XpreadLoc16Al::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

// XpwriteAlLoc16
uint8_t XpwriteAlLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t XpwriteAlLoc16::SetLoc16(const uint8_t loc16) {
  return opcode | loc16;
}

// XretcCond
uint8_t XretcCond::GetCond(const uint32_t data) { return data & 0xFu; }

uint32_t XretcCond::SetCond(const uint8_t cond) { return opcode | cond & 0xFu; }

// ZalrAccLoc16
uint8_t ZalrAccLoc16::GetLoc16(const uint32_t data) { return data & 0xFFu; }

uint32_t ZalrAccLoc16::SetLoc16(const uint8_t loc16) { return opcode | loc16; }

/* Extended Instruction Set */

// FPU Instructions

// Absf32RahRbh
uint8_t Absf32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t Absf32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t Absf32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t Absf32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// Addf32Rah16fhiRbh
uint8_t Addf32Rah16fhiRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t Addf32Rah16fhiRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t Addf32Rah16fhiRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t Addf32Rah16fhiRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

uint16_t Addf32Rah16fhiRbh::Get16fhi(const uint32_t data) {
  return FpuGet16FHi_I(data);
}

uint32_t Addf32Rah16fhiRbh::Set16fhi(const uint16_t i) {
  return FpuSet16FHi_I(opcode, i);
}

// Addf32RahRbhRch
uint8_t Addf32RahRbhRch::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t Addf32RahRbhRch::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t Addf32RahRbhRch::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t Addf32RahRbhRch::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

uint8_t Addf32RahRbhRch::GetRegC(const uint32_t data) {
  return FpuGetRegC_I(data);
}

uint32_t Addf32RahRbhRch::SetRegC(const uint8_t c) {
  return FpuSetRegC_I(opcode, c);
}

// Addf32RdhRehRfhMov32Mem32Rah
uint8_t Addf32RdhRehRfhMov32Mem32Rah::GetRegA(const uint32_t data) {
  return FpuGetRegA_II(data);
}

uint32_t Addf32RdhRehRfhMov32Mem32Rah::SetRegA(const uint8_t a) {
  return FpuSetRegA_II(opcode, a);
}

uint8_t Addf32RdhRehRfhMov32Mem32Rah::GetRegD(const uint32_t data) {
  return FpuGetRegD_II(data);
}

uint32_t Addf32RdhRehRfhMov32Mem32Rah::SetRegD(const uint8_t d) {
  return FpuSetRegD_II(opcode, d);
}

uint8_t Addf32RdhRehRfhMov32Mem32Rah::GetRegE(const uint32_t data) {
  return FpuGetRegE_II(data);
}

uint32_t Addf32RdhRehRfhMov32Mem32Rah::SetRegE(const uint8_t e) {
  return FpuSetRegE_II(opcode, e);
}

uint8_t Addf32RdhRehRfhMov32Mem32Rah::GetRegF(const uint32_t data) {
  return FpuGetRegF_II(data);
}

uint32_t Addf32RdhRehRfhMov32Mem32Rah::SetRegF(const uint8_t f) {
  return FpuSetRegF_II(opcode, f);
}

uint16_t Addf32RdhRehRfhMov32Mem32Rah::GetMem32(const uint32_t data) {
  return FpuGetMem(data);
}

uint32_t Addf32RdhRehRfhMov32Mem32Rah::SetMem32(const uint16_t mem32) {
  return FpuSetMem(opcode, mem32);
}

// Addf32RdhRehRfhMov32RahMem32
uint8_t Addf32RdhRehRfhMov32RahMem32::GetRegA(const uint32_t data) {
  return FpuGetRegA_II(data);
}

uint32_t Addf32RdhRehRfhMov32RahMem32::SetRegA(const uint8_t a) {
  return FpuSetRegA_II(opcode, a);
}

uint8_t Addf32RdhRehRfhMov32RahMem32::GetRegD(const uint32_t data) {
  return FpuGetRegD_II(data);
}

uint32_t Addf32RdhRehRfhMov32RahMem32::SetRegD(const uint8_t d) {
  return FpuSetRegD_II(opcode, d);
}

uint8_t Addf32RdhRehRfhMov32RahMem32::GetRegE(const uint32_t data) {
  return FpuGetRegE_II(data);
}

uint32_t Addf32RdhRehRfhMov32RahMem32::SetRegE(const uint8_t e) {
  return FpuSetRegE_II(opcode, e);
}

uint8_t Addf32RdhRehRfhMov32RahMem32::GetRegF(const uint32_t data) {
  return FpuGetRegF_II(data);
}

uint32_t Addf32RdhRehRfhMov32RahMem32::SetRegF(const uint8_t f) {
  return FpuSetRegF_II(opcode, f);
}

uint16_t Addf32RdhRehRfhMov32RahMem32::GetMem32(const uint32_t data) {
  return FpuGetMem(data);
}

uint32_t Addf32RdhRehRfhMov32RahMem32::SetMem32(const uint16_t mem32) {
  return FpuSetMem(opcode, mem32);
}

// Cmpf32RahRbh
uint8_t Cmpf32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t Cmpf32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t Cmpf32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t Cmpf32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// Cmpf32Rah16fhi
uint8_t Cmpf32Rah16fhi::GetRegA(const uint32_t data) {
  return FpuGetRegA_III(data);
}

uint32_t Cmpf32Rah16fhi::SetRegA(const uint8_t a) {
  return FpuSetRegA_III(opcode, a);
}

uint16_t Cmpf32Rah16fhi::Get16fhi(const uint32_t data) {
  return FpuGet16FHi_III(data);
}

uint32_t Cmpf32Rah16fhi::Set16fhi(const uint16_t i) {
  return FpuSet16FHi_III(opcode, i);
}

// Cmpf32Rah0
uint8_t Cmpf32Rah0::GetRegA(const uint32_t data) {
  return FpuGetRegA_III(data);
}

uint32_t Cmpf32Rah0::SetRegA(const uint8_t a) {
  return FpuSetRegA_III(opcode, a);
}

// Einvf32RahRbh
uint8_t Einvf32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t Einvf32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t Einvf32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t Einvf32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// Eisqrtf32RahRbh
uint8_t Eisqrtf32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t Eisqrtf32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t Eisqrtf32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t Eisqrtf32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// F32toi16RahRbh
uint8_t F32toi16RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t F32toi16RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t F32toi16RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t F32toi16RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// F32toi16rRahRbh
uint8_t F32toi16rRahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t F32toi16rRahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t F32toi16rRahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t F32toi16rRahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// F32toi32RahRbh
uint8_t F32toi32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t F32toi32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t F32toi32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t F32toi32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// F32toui16RahRbh
uint8_t F32toui16RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t F32toui16RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t F32toui16RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t F32toui16RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// F32toui16rRahRbh
uint8_t F32toui16rRahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t F32toui16rRahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t F32toui16rRahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t F32toui16rRahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// F32toui32RahRbh
uint8_t F32toui32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t F32toui32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t F32toui32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t F32toui32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// Fracf32RahRbh
uint8_t Fracf32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t Fracf32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t Fracf32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t Fracf32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// I16tof32RahRbh
uint8_t I16tof32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t I16tof32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t I16tof32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t I16tof32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// I16tof32RahMem16
uint8_t I16tof32RahMem16::GetRegA(const uint32_t data) {
  return FpuGetRegA_II(data);
}

uint32_t I16tof32RahMem16::SetRegA(const uint8_t a) {
  return FpuSetRegA_II(opcode, a);
}

uint16_t I16tof32RahMem16::GetMem16(const uint32_t data) {
  return FpuGetMem(data);
}

uint32_t I16tof32RahMem16::SetMem16(const uint16_t mem16) {
  return FpuSetMem(opcode, mem16);
}

// I32tof32RahMem32
uint8_t I32tof32RahMem32::GetRegA(const uint32_t data) {
  return FpuGetRegA_II(data);
}

uint32_t I32tof32RahMem32::SetRegA(const uint8_t a) {
  return FpuSetRegA_II(opcode, a);
}

uint16_t I32tof32RahMem32::GetMem32(const uint32_t data) {
  return FpuGetMem(data);
}

uint32_t I32tof32RahMem32::SetMem32(const uint16_t mem32) {
  return FpuSetMem(opcode, mem32);
}

// I32tof32RahRbh
uint8_t I32tof32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t I32tof32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t I32tof32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t I32tof32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// Macf32R3hR2hRdhRehRfh
uint8_t Macf32R3hR2hRdhRehRfh::GetRegD(const uint32_t data) {
  return FpuGetRegD_IV(data);
}

uint32_t Macf32R3hR2hRdhRehRfh::SetRegD(const uint8_t d) {
  return FpuSetRegD_IV(opcode, d);
}

uint8_t Macf32R3hR2hRdhRehRfh::GetRegE(const uint32_t data) {
  return FpuGetRegE_IV(data);
}

uint32_t Macf32R3hR2hRdhRehRfh::SetRegE(const uint8_t e) {
  return FpuSetRegE_IV(opcode, e);
}

uint8_t Macf32R3hR2hRdhRehRfh::GetRegF(const uint32_t data) {
  return FpuGetRegF_IV(data);
}

uint32_t Macf32R3hR2hRdhRehRfh::SetRegF(const uint8_t f) {
  return FpuSetRegF_IV(opcode, f);
}

// Macf32R3hR2hRdhRehRfhMov32RahMem32
uint8_t Macf32R3hR2hRdhRehRfhMov32RahMem32::GetRegA(const uint32_t data) {
  return FpuGetRegA_II(data);
}

uint32_t Macf32R3hR2hRdhRehRfhMov32RahMem32::SetRegA(const uint8_t a) {
  return FpuSetRegA_II(opcode, a);
}

uint8_t Macf32R3hR2hRdhRehRfhMov32RahMem32::GetRegD(const uint32_t data) {
  return FpuGetRegD_II(data);
}

uint32_t Macf32R3hR2hRdhRehRfhMov32RahMem32::SetRegD(const uint8_t d) {
  return FpuSetRegD_II(opcode, d);
}

uint8_t Macf32R3hR2hRdhRehRfhMov32RahMem32::GetRegE(const uint32_t data) {
  return FpuGetRegE_II(data);
}

uint32_t Macf32R3hR2hRdhRehRfhMov32RahMem32::SetRegE(const uint8_t e) {
  return FpuSetRegE_II(opcode, e);
}

uint8_t Macf32R3hR2hRdhRehRfhMov32RahMem32::GetRegF(const uint32_t data) {
  return FpuGetRegF_II(data);
}

uint32_t Macf32R3hR2hRdhRehRfhMov32RahMem32::SetRegF(const uint8_t f) {
  return FpuSetRegF_II(opcode, f);
}

uint16_t Macf32R3hR2hRdhRehRfhMov32RahMem32::GetMem32(const uint32_t data) {
  return FpuGetMem(data);
}

uint32_t Macf32R3hR2hRdhRehRfhMov32RahMem32::SetMem32(const uint16_t mem32) {
  return FpuSetMem(opcode, mem32);
}

// Macf32R7hR3hMem32Xar7Postinc
uint16_t Macf32R7hR3hMem32Xar7Postinc::GetMem32(const uint32_t data) {
  return FpuGetMem(data);
}

uint32_t Macf32R7hR3hMem32Xar7Postinc::SetMem32(const uint16_t mem32) {
  return FpuSetMem(opcode, mem32);
}

// Macf32R7hR6hRdhRehRfh
uint8_t Macf32R7hR6hRdhRehRfh::GetRegD(const uint32_t data) {
  return FpuGetRegD_IV(data);
}

uint32_t Macf32R7hR6hRdhRehRfh::SetRegD(const uint8_t d) {
  return FpuSetRegD_IV(opcode, d);
}

uint8_t Macf32R7hR6hRdhRehRfh::GetRegE(const uint32_t data) {
  return FpuGetRegE_IV(data);
}

uint32_t Macf32R7hR6hRdhRehRfh::SetRegE(const uint8_t e) {
  return FpuSetRegE_IV(opcode, e);
}

uint8_t Macf32R7hR6hRdhRehRfh::GetRegF(const uint32_t data) {
  return FpuGetRegF_IV(data);
}

uint32_t Macf32R7hR6hRdhRehRfh::SetRegF(const uint8_t f) {
  return FpuSetRegF_IV(opcode, f);
}

// Macf32R7hR6hRdhRehRfhMov32RahMem32
uint8_t Macf32R7hR6hRdhRehRfhMov32RahMem32::GetRegA(const uint32_t data) {
  return FpuGetRegA_II(data);
}

uint32_t Macf32R7hR6hRdhRehRfhMov32RahMem32::SetRegA(const uint8_t a) {
  return FpuSetRegA_II(opcode, a);
}

uint8_t Macf32R7hR6hRdhRehRfhMov32RahMem32::GetRegD(const uint32_t data) {
  return FpuGetRegD_II(data);
}

uint32_t Macf32R7hR6hRdhRehRfhMov32RahMem32::SetRegD(const uint8_t d) {
  return FpuSetRegD_II(opcode, d);
}

uint8_t Macf32R7hR6hRdhRehRfhMov32RahMem32::GetRegE(const uint32_t data) {
  return FpuGetRegE_II(data);
}

uint32_t Macf32R7hR6hRdhRehRfhMov32RahMem32::SetRegE(const uint8_t e) {
  return FpuSetRegE_II(opcode, e);
}

uint8_t Macf32R7hR6hRdhRehRfhMov32RahMem32::GetRegF(const uint32_t data) {
  return FpuGetRegF_II(data);
}

uint32_t Macf32R7hR6hRdhRehRfhMov32RahMem32::SetRegF(const uint8_t f) {
  return FpuSetRegF_II(opcode, f);
}

uint16_t Macf32R7hR6hRdhRehRfhMov32RahMem32::GetMem32(const uint32_t data) {
  return FpuGetMem(data);
}

uint32_t Macf32R7hR6hRdhRehRfhMov32RahMem32::SetMem32(const uint16_t mem32) {
  return FpuSetMem(opcode, mem32);
}

// Maxf32RahRbh
uint8_t Maxf32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t Maxf32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t Maxf32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t Maxf32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// Maxf32Rah16fhi
uint8_t Maxf32Rah16fhi::GetRegA(const uint32_t data) {
  return FpuGetRegA_III(data);
}

uint32_t Maxf32Rah16fhi::SetRegA(const uint8_t a) {
  return FpuSetRegA_III(opcode, a);
}

uint16_t Maxf32Rah16fhi::Get16fhi(const uint32_t data) {
  return FpuGet16FHi_III(data);
}

uint32_t Maxf32Rah16fhi::Set16fhi(const uint16_t i) {
  return FpuSet16FHi_III(opcode, i);
}

// Maxf32RahRbhMov32RchRdh
uint8_t Maxf32RahRbhMov32RchRdh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t Maxf32RahRbhMov32RchRdh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t Maxf32RahRbhMov32RchRdh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t Maxf32RahRbhMov32RchRdh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

uint8_t Maxf32RahRbhMov32RchRdh::GetRegC(const uint32_t data) {
  return FpuGetRegC_I(data);
}

uint32_t Maxf32RahRbhMov32RchRdh::SetRegC(const uint8_t c) {
  return FpuSetRegC_I(opcode, c);
}

uint8_t Maxf32RahRbhMov32RchRdh::GetRegD(const uint32_t data) {
  return FpuGetRegD_IV(data);
}

uint32_t Maxf32RahRbhMov32RchRdh::SetRegD(const uint8_t d) {
  return FpuSetRegD_IV(opcode, d);
}

// Minf32RahRbh
uint8_t Minf32RahRbh::GetRegA(const uint32_t data) {
  return FpuGetRegA_I(data);
}

uint32_t Minf32RahRbh::SetRegA(const uint8_t a) {
  return FpuSetRegA_I(opcode, a);
}

uint8_t Minf32RahRbh::GetRegB(const uint32_t data) {
  return FpuGetRegB_I(data);
}

uint32_t Minf32RahRbh::SetRegB(const uint8_t b) {
  return FpuSetRegB_I(opcode, b);
}

// Minf32Rah16fhi
uint8_t Minf32Rah16fhi::GetRegA(const uint32_t data) {
  return FpuGetRegA_III(data);
}

uint32_t Minf32Rah16fhi::SetRegA(const uint8_t a) {
  return FpuSetRegA_III(opcode, a);
}

uint16_t Minf32Rah16fhi::Get16fhi(const uint32_t data) {
  return FpuGet16FHi_III(data);
}

uint32_t Minf32Rah16fhi::Set16fhi(const uint16_t i) {
  return FpuSet16FHi_III(opcode, i);
}

}  // namespace TIC28X
