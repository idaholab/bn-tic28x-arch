// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#include "info.h"

#include <binaryninjaapi.h>

#include "instructions.h"
#include "util.h"

namespace TIC28X {
/* Default Info Methods */
bool Instruction2Byte::Info(const uint8_t* data, uint64_t addr,
                            BN::InstructionInfo& result,
                            TIC28XArchitecture* arch) {
  result.length = GetLength();
  return true;
}

bool Instruction4Byte::Info(const uint8_t* data, uint64_t addr,
                            BN::InstructionInfo& result,
                            TIC28XArchitecture* arch) {
  result.length = GetLength();
  return true;
}

/* Instruction-specific info methods */

bool BOff16Cond::Info(const uint8_t* data, const uint64_t addr,
                      BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool BanzOff16Arn::Info(const uint8_t* data, const uint64_t addr,
                        BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool BarOff16ArnArmEq::Info(const uint8_t* data, const uint64_t addr,
                            BN::InstructionInfo& result,
                            TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool BarOff16ArnArmNeq::Info(const uint8_t* data, const uint64_t addr,
                             BN::InstructionInfo& result,
                             TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool BfOff16Cond::Info(const uint8_t* data, const uint64_t addr,
                       BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool FfcXar7Const22::Info(const uint8_t* data, const uint64_t addr,
                          BN::InstructionInfo& result,
                          TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(CallDestination, GetTarget(dataOp, addr));

  return true;
}

bool IntrIntx::Info(const uint8_t* data, const uint64_t addr,
                    BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(ExceptionBranch);

  return true;
}

bool IntrNmi::Info(const uint8_t* data, const uint64_t addr,
                   BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(ExceptionBranch);

  return true;
}

bool IntrEmuint::Info(const uint8_t* data, const uint64_t addr,
                      BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(ExceptionBranch);

  return true;
}

bool Iret::Info(const uint8_t* data, const uint64_t addr,
                BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(FunctionReturn);

  return true;
}

bool LbXar7::Info(const uint8_t* data, const uint64_t addr,
                  BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(IndirectBranch);

  return true;
}

bool LbConst22::Info(const uint8_t* data, const uint64_t addr,
                     BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(UnconditionalBranch, GetTarget(dataOp, addr));

  return true;
}

bool LcXar7::Info(const uint8_t* data, const uint64_t addr,
                  BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(IndirectBranch);

  return true;
}

bool LcConst22::Info(const uint8_t* data, const uint64_t addr,
                     BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(CallDestination, GetTarget(dataOp, addr));

  return true;
}

bool LcrConst22::Info(const uint8_t* data, const uint64_t addr,
                      BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(CallDestination, GetTarget(dataOp, addr));

  return true;
}

bool LcrXarn::Info(const uint8_t* data, const uint64_t addr,
                   BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(IndirectBranch);

  return true;
}

bool LoopnzLoc16Const16::Info(const uint8_t* data, const uint64_t addr,
                              BN::InstructionInfo& result,
                              TIC28XArchitecture* arch) {
  result.length = GetLength();

  // If I'm understanding the loop instruction correctly, it basically waits
  // until a true condition to continue with execution. True conditions can be
  // triggered using interrupts.
  result.AddBranch(TrueBranch, addr + result.length);
  result.AddBranch(FalseBranch, addr);

  return true;
}

bool LoopzLoc16Const16::Info(const uint8_t* data, const uint64_t addr,
                             BN::InstructionInfo& result,
                             TIC28XArchitecture* arch) {
  result.length = GetLength();

  // If I'm understanding the loop instruction correctly, it basically waits
  // until a true condition to continue with execution. True conditions can be
  // triggered using interrupts.
  result.AddBranch(TrueBranch, addr + result.length);
  result.AddBranch(FalseBranch, addr);

  return true;
}

bool Lret::Info(const uint8_t* data, const uint64_t addr,
                BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(FunctionReturn);

  return true;
}

bool Lrete::Info(const uint8_t* data, const uint64_t addr,
                 BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(FunctionReturn);

  return true;
}

bool Lretr::Info(const uint8_t* data, const uint64_t addr,
                 BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(FunctionReturn);

  return true;
}

bool RptConst8::Info(const uint8_t* data, const uint64_t addr,
                     BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  // The next instruction will be repeated if possible
  arch->AddRepeatAddr(addr + result.length);

  return true;
}

bool RptLoc16::Info(const uint8_t* data, const uint64_t addr,
                    BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  // The next instruction will be repeated if possible
  arch->AddRepeatAddr(addr + result.length);

  return true;
}

bool SbOff8Cond::Info(const uint8_t* data, const uint64_t addr,
                      BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool SbfOff8Eq::Info(const uint8_t* data, const uint64_t addr,
                     BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool SbfOff8Neq::Info(const uint8_t* data, const uint64_t addr,
                      BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool SbfOff8Tc::Info(const uint8_t* data, const uint64_t addr,
                     BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool SbfOff8Ntc::Info(const uint8_t* data, const uint64_t addr,
                      BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool TrapVec::Info(const uint8_t* data, const uint64_t addr,
                   BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(ExceptionBranch);

  return true;
}

bool XbAl::Info(const uint8_t* data, const uint64_t addr,
                BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(IndirectBranch);

  return true;
}

bool XbPmaArpn::Info(const uint8_t* data, const uint64_t addr,
                     BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(UnconditionalBranch, GetTarget(dataOp, addr));

  return true;
}

bool XbPmaCond::Info(const uint8_t* data, const uint64_t addr,
                     BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XbanzPmaInd::Info(const uint8_t* data, const uint64_t addr,
                       BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XbanzPmaIndPostinc::Info(const uint8_t* data, const uint64_t addr,
                              BN::InstructionInfo& result,
                              TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XbanzPmaIndPostdec::Info(const uint8_t* data, const uint64_t addr,
                              BN::InstructionInfo& result,
                              TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XbanzPmaInd0Postinc::Info(const uint8_t* data, const uint64_t addr,
                               BN::InstructionInfo& result,
                               TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XbanzPmaInd0Postdec::Info(const uint8_t* data, const uint64_t addr,
                               BN::InstructionInfo& result,
                               TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XbanzPmaIndArpn::Info(const uint8_t* data, const uint64_t addr,
                           BN::InstructionInfo& result,
                           TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XbanzPmaIndPostincArpn::Info(const uint8_t* data, const uint64_t addr,
                                  BN::InstructionInfo& result,
                                  TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XbanzPmaIndPostdecArpn::Info(const uint8_t* data, const uint64_t addr,
                                  BN::InstructionInfo& result,
                                  TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XbanzPmaInd0PostincArpn::Info(const uint8_t* data, const uint64_t addr,
                                   BN::InstructionInfo& result,
                                   TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XbanzPmaInd0PostdecArpn::Info(const uint8_t* data, const uint64_t addr,
                                   BN::InstructionInfo& result,
                                   TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XcallAl::Info(const uint8_t* data, const uint64_t addr,
                   BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  result.length = GetLength();

  result.AddBranch(IndirectBranch);

  return true;
}

bool XcallPmaArpn::Info(const uint8_t* data, const uint64_t addr,
                        BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(UnconditionalBranch, GetTarget(dataOp, addr));

  return true;
}

bool XcallPmaCond::Info(const uint8_t* data, const uint64_t addr,
                        BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(TrueBranch, GetTarget(dataOp, addr));
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

bool XretcCond::Info(const uint8_t* data, const uint64_t addr,
                     BN::InstructionInfo& result, TIC28XArchitecture* arch) {
  const auto dataOp = DataToOpcode(data, GetLength());
  result.length = GetLength();

  result.AddBranch(FunctionReturn);
  result.AddBranch(FalseBranch, addr + result.length);

  return true;
}

}  // namespace TIC28X