// Copyright (c) 2025. Battelle Energy Alliance, LLC
// ALL RIGHTS RESERVED

#ifndef TIC28X_OPCODES_H
#define TIC28X_OPCODES_H

#include <cstdint>

namespace TIC28X::Opcodes {
/* Instruction Opcodes */
constexpr uint32_t ABORTI = 0x1;
constexpr uint32_t ABS_ACC = 0xFF56;
constexpr uint32_t ABSTC_ACC = 0x565F;
constexpr uint32_t ADD_ACC_CONST16_SHIFT = 0xFF100000;
constexpr uint32_t ADD_ACC_LOC16_SHIFT_T = 0x56230000;
constexpr uint32_t ADD_ACC_LOC16 = 0x8100;
constexpr uint32_t ADD_ACC_LOC16_SHIFT1_15 = 0x56040000;
constexpr uint32_t ADD_ACC_LOC16_SHIFT16 = 0x0500;
constexpr uint32_t ADD_ACC_LOC16_SHIFT0_15 = 0xA000;
constexpr uint32_t ADD_AX_LOC16 = 0x9400;
constexpr uint32_t ADD_LOC16_AX = 0x7200;
constexpr uint32_t ADD_LOC16_CONST16 = 0x08000000;
constexpr uint32_t ADDB_ACC_CONST8 = 0x0900;
constexpr uint32_t ADDB_AX_CONST8 = 0x9C00;
constexpr uint32_t ADDB_SP_CONST7 = 0xFE00;
constexpr uint32_t ADDB_XARN_CONST7 = 0xD800;
constexpr uint32_t ADDCL_ACC_LOC32 = 0x56400000;
constexpr uint32_t ADDCU_ACC_LOC16 = 0x0C00;
constexpr uint32_t ADDL_ACC_LOC32 = 0x0700;
constexpr uint32_t ADDL_ACC_P_SHIFT_PM = 0x10AC;
constexpr uint32_t ADDL_LOC32_ACC = 0x56010000;
constexpr uint32_t ADDU_ACC_LOC16 = 0x0D00;
constexpr uint32_t ADDUL_P_LOC32 = 0x56570000;
constexpr uint32_t ADDUL_ACC_LOC32 = 0x56530000;
constexpr uint32_t ADRK_IMM8 = 0xFC00;
constexpr uint32_t AND_ACC_CONST16_SHIFT0_15 = 0x3E000000;
constexpr uint32_t AND_ACC_CONST16_SHIFT16 = 0x56080000;
constexpr uint32_t AND_ACC_LOC16 = 0x8900;
constexpr uint32_t AND_AX_LOC16_CONST16 = 0xCC000000;
constexpr uint32_t AND_IER_CONST16 = 0x76260000;
constexpr uint32_t AND_IFR_CONST16 = 0x762F0000;
constexpr uint32_t AND_LOC16_AX = 0xC000;
constexpr uint32_t AND_AX_LOC16 = 0xCE00;
constexpr uint32_t AND_LOC16_CONST16 = 0x18000000;
constexpr uint32_t ANDB_AX_CONST8 = 0x9000;
constexpr uint32_t ASP = 0x761B;
constexpr uint32_t ASR_AX_SHIFT = 0xFFA0;
constexpr uint32_t ASR_AX_T = 0xFF64;
constexpr uint32_t ASR64_ACC_P_SHIFT = 0x5680;
constexpr uint32_t ASR64_ACC_P_T = 0x562C;
constexpr uint32_t ASRL_ACC_T = 0x5610;
constexpr uint32_t B_OFF16_COND = 0xFFE00000;
constexpr uint32_t BANZ_OFF16_ARN = 0x00080000;
constexpr uint32_t BAR_OFF16_ARN_ARM_EQ = 0x8F800000;
constexpr uint32_t BAR_OFF16_ARN_ARM_NEQ = 0x8FC00000;
constexpr uint32_t BF_OFF16_COND = 0x56C00000;
constexpr uint32_t CLRC_AMODE = 0x5616;    // alias: C28ADDR
constexpr uint32_t CLRC_M0M1MAP = 0x563F;  // alias: C27MAP
constexpr uint32_t CLRC_OBJMODE = 0x5636;  // alias: C27OBJ
constexpr uint32_t CLRC_OVC = 0x565C;
constexpr uint32_t CLRC_XF = 0x561B;
constexpr uint32_t CLRC_MODE = 0x2900;
constexpr uint32_t CMP_AX_LOC15 = 0x5400;
constexpr uint32_t CMP_LOC16_CONST16 = 0x1B000000;
constexpr uint32_t CMP64_ACC_P = 0x565E;
constexpr uint32_t CMPB_AX_CONST8 = 0x5200;
constexpr uint32_t CMPL_ACC_LOC32 = 0x0F00;
constexpr uint32_t CMPL_ACC_P_SHIFT_PM = 0xFF59;
constexpr uint32_t CMPR_0 = 0x561D;
constexpr uint32_t CMPR_1 = 0x5619;
constexpr uint32_t CMPR_2 = 0x5618;
constexpr uint32_t CMPR_3 = 0x561C;
constexpr uint32_t CSB_ACC = 0x5635;
constexpr uint32_t DEC_LOC16 = 0x0B00;
constexpr uint32_t DMAC_ACCC_P_LOC32_XAR7 = 0x564BC700;
constexpr uint32_t DMAC_ACCC_P_LOC32_XAR7_POSTINC = 0x564B8700;
constexpr uint32_t DMOV_LOC16 = 0xA500;
constexpr uint32_t EALLOW = 0x7622;
constexpr uint32_t EDIS = 0x761A;
constexpr uint32_t ESTOP0 = 0x7625;
constexpr uint32_t ESTOP1 = 0x7624;
constexpr uint32_t FFC_XAR7_CONST22 = 0x00C00000;
constexpr uint32_t FLIP_AX = 0x5670;
constexpr uint32_t IACK_CONST16 = 0x763F0000;
constexpr uint32_t IDLE = 0x7621;
constexpr uint32_t IMACL_P_LOC32_XAR7 = 0x564DC700;
constexpr uint32_t IMACL_P_LOC32_XAR7_POSTINC = 0x564D8700;
constexpr uint32_t IMPYAL_P_XT_LOC32 = 0x564C0000;
constexpr uint32_t IMPYL_ACC_XT_LOC32 = 0x56440000;
constexpr uint32_t IMPYL_P_XT_LOC32 = 0x56050000;
constexpr uint32_t IMPYSL_P_XT_LOC32 = 0x56430000;
constexpr uint32_t IMPYXUL_P_XT_LOC32 = 0x56650000;
constexpr uint32_t IN_LOC16_PA = 0xB4000000;
constexpr uint32_t INC_LOC16 = 0x0A00;
constexpr uint32_t INTR_INTX = 0x0010;  // alias: INTR DLOGINT, INTR RTOSINT
constexpr uint32_t INTR_NMI = 0x7616;
constexpr uint32_t INTR_EMUINT = 0x761C;
constexpr uint32_t IRET = 0x7602;
constexpr uint32_t LB_XAR7 = 0x7620;
constexpr uint32_t LB_CONST22 = 0x00400000;
constexpr uint32_t LC_XAR7 = 0x7604;
constexpr uint32_t LC_CONST22 = 0x00800000;
constexpr uint32_t LCR_CONST22 = 0x76400000;
constexpr uint32_t LCR_XARN = 0x3E60;
constexpr uint32_t LOOPNZ_LOC16_CONST16 = 0x2E000000;
constexpr uint32_t LOOPZ_LOC16_CONST16 = 0x2C000000;
constexpr uint32_t LPADDR = 0x561E;
constexpr uint32_t LRET = 0x7614;
constexpr uint32_t LRETE = 0x7610;
constexpr uint32_t LRETR = 0x0006;
constexpr uint32_t LSL_ACC_SHIFT = 0xFF30;
constexpr uint32_t LSL_ACC_T = 0xFF50;
constexpr uint32_t LSL_AX_SHIFT = 0xFF80;
constexpr uint32_t LSL_AX_T = 0xFF66;
constexpr uint32_t LSL64_ACC_P_SHIFT = 0x56A0;
constexpr uint32_t LSL64_ACC_P_T = 0x5652;
constexpr uint32_t LSLL_ACC_T = 0x563B;
constexpr uint32_t LSR_AX_SHIFT = 0xFFC0;
constexpr uint32_t LSR_AX_T = 0xFF62;
constexpr uint32_t LSR64_ACC_P_SHIFT = 0x5690;
constexpr uint32_t LSR64_ACC_P_T = 0x565B;
constexpr uint32_t LSRL_ACC_T = 0x5622;
constexpr uint32_t MAC_P_LOC16_PMA = 0x14000000;
constexpr uint32_t MAC_P_LOC16_XAR7 = 0x5607C700;
constexpr uint32_t MAC_P_LOC16_XAR7_POSTINC = 0x56078700;
constexpr uint32_t MAX_AX_LOC16 = 0x56720000;
constexpr uint32_t MAXCUL_P_LOC32 = 0x56510000;
constexpr uint32_t MAXL_ACC_LOC32 = 0x56610000;
constexpr uint32_t MIN_AX_LOC16 = 0x56740000;
constexpr uint32_t MINCUL_P_LOC32 = 0x56590000;
constexpr uint32_t MINL_ACC_LOC32 = 0x56500000;
constexpr uint32_t MOV_MEM16_LOC16 = 0xF4000000;
constexpr uint32_t MOV_ACC_CONST16_SHIFT = 0xFF200000;
constexpr uint32_t MOV_ACC_LOC16_SHIFT_T = 0x56060000;
constexpr uint32_t MOV_ACC_LOC16_OBJMDOE1 = 0x8500;
constexpr uint32_t MOV_ACC_LOC16_OBJMDOE0 = 0xE000;
constexpr uint32_t MOV_ACC_LOC16_SHIFT1_15_OBJMODE1 = 0x56030000;
constexpr uint32_t MOV_ACC_LOC16_SHIFT1_15_OBJMODE0 = 0xE000;
constexpr uint32_t MOV_ACC_LOC16_SHIFT16 = 0x2500;
constexpr uint32_t MOV_AR6_LOC16 = 0x5E00;
constexpr uint32_t MOV_AR7_LOC16 = 0x5F00;
constexpr uint32_t MOV_AX_LOC16 = 0x9200;
constexpr uint32_t MOV_DP_CONST10 = 0xF800;
constexpr uint32_t MOV_IER_LOC16 = 0x2300;
constexpr uint32_t MOV_LOC16_CONST16 = 0x28000000;
constexpr uint32_t MOV_LOC16_MEM16 = 0xF5000000;
constexpr uint32_t MOV_LOC16_0 = 0x2B00;
constexpr uint32_t MOV_LOC16_ACC_SHIFT1 = 0xB100;
constexpr uint32_t MOV_LOC16_ACC_SHIFT2_8_OBJMODE_1 = 0x562D0000;
constexpr uint32_t MOV_LOC16_ACC_SHIFT2_8_OBJMODE_0 = 0xB800;
constexpr uint32_t MOV_LOC16_ARN = 0x7800;
constexpr uint32_t MOV_LOC16_AX = 0x9600;
constexpr uint32_t MOV_LOC16_AX_COND = 0x562A0000;
constexpr uint32_t MOV_LOC16_IER = 0x2000;
constexpr uint32_t MOV_LOC16_OVC = 0x56290000;
constexpr uint32_t MOV_LOC16_P = 0x3F00;
constexpr uint32_t MOV_LOC16_T = 0x2100;
constexpr uint32_t MOV_OVC_LOC16 = 0x56020000;
constexpr uint32_t MOV_PH_LOC16 = 0x2F00;
constexpr uint32_t MOV_PL_LOC16 = 0x2700;
constexpr uint32_t MOV_PM_AX = 0x5638;
constexpr uint32_t MOV_T_LOC16 = 0x2D00;
constexpr uint32_t MOV_TL_0 = 0x5656;
constexpr uint32_t MOV_XARN_PC = 0x3E58;
constexpr uint32_t MOVA_T_LOC16 = 0x1000;
constexpr uint32_t MOVAD_T_LOC16 = 0xA700;
constexpr uint32_t MOVB_ACC_CONST8 = 0x0200;
constexpr uint32_t MOVB_AR6_CONST8 = 0xD600;
constexpr uint32_t MOVB_AR7_CONST8 = 0xD700;
constexpr uint32_t MOVB_AX_CONST8 = 0x9A00;
constexpr uint32_t MOVB_AXLSB_LOC16 = 0xC600;
constexpr uint32_t MOVB_AXMSB_LOC16 = 0x3800;
constexpr uint32_t MOVB_LOC16_CONST8_COND = 0x56B00000;
constexpr uint32_t MOVB_LOC16_AXLSB = 0x3C00;
constexpr uint32_t MOVB_LOC16_AXMSB = 0xC800;
constexpr uint32_t MOVB_XAR0_CONST8 = 0xD000;
constexpr uint32_t MOVB_XAR1_CONST8 = 0xD100;
constexpr uint32_t MOVB_XAR2_CONST8 = 0xD200;
constexpr uint32_t MOVB_XAR3_CONST8 = 0xD300;
constexpr uint32_t MOVB_XAR4_CONST8 = 0xD400;
constexpr uint32_t MOVB_XAR5_CONST8 = 0xD500;
constexpr uint32_t MOVB_XAR6_CONST8 = 0xBE00;
constexpr uint32_t MOVB_XAR7_CONST8 = 0xB600;
constexpr uint32_t MOVDL_XT_LOC32 = 0xA600;
constexpr uint32_t MOVH_LOC16_ACC_SHIFT1 = 0xB300;
constexpr uint32_t MOVH_LOC16_ACC_SHIFT2_8_OBJMODE_1 = 0x562F0000;
constexpr uint32_t MOVH_LOC16_ACC_SHIFT2_8_OBJMODE_0 = 0xB000;
constexpr uint32_t MOVH_LOC16_P = 0x5700;
constexpr uint32_t MOVL_ACC_LOC32 = 0x0600;
constexpr uint32_t MOVL_ACC_P_SHIFT_PM = 0x16AC;
constexpr uint32_t MOVL_LOC32_ACC = 0x1E00;
constexpr uint32_t MOVL_LOC32_ACC_COND = 0x56480000;
constexpr uint32_t MOVL_LOC32_P = 0xA900;
constexpr uint32_t MOVL_LOC32_XAR0 = 0x3A00;
constexpr uint32_t MOVL_LOC32_XAR1 = 0xB200;
constexpr uint32_t MOVL_LOC32_XAR2 = 0xAA00;
constexpr uint32_t MOVL_LOC32_XAR3 = 0xA200;
constexpr uint32_t MOVL_LOC32_XAR4 = 0xA800;
constexpr uint32_t MOVL_LOC32_XAR5 = 0xA000;
constexpr uint32_t MOVL_LOC32_XAR6 = 0xC200;
constexpr uint32_t MOVL_LOC32_XAR7 = 0xC300;
constexpr uint32_t MOVL_LOC32_XT = 0xAB00;
constexpr uint32_t MOVL_P_ACC = 0xFF5A;
constexpr uint32_t MOVL_P_LOC32 = 0xA300;
constexpr uint32_t MOVL_XAR0_LOC32 = 0x8E00;
constexpr uint32_t MOVL_XAR1_LOC32 = 0x8B00;
constexpr uint32_t MOVL_XAR2_LOC32 = 0x8600;
constexpr uint32_t MOVL_XAR3_LOC32 = 0x8200;
constexpr uint32_t MOVL_XAR4_LOC32 = 0x8A00;
constexpr uint32_t MOVL_XAR5_LOC32 = 0x8300;
constexpr uint32_t MOVL_XAR6_LOC32 = 0xC400;
constexpr uint32_t MOVL_XAR7_LOC32 = 0xC500;
constexpr uint32_t MOVL_XAR0_CONST22 = 0x8D000000;
constexpr uint32_t MOVL_XAR1_CONST22 = 0x8D400000;
constexpr uint32_t MOVL_XAR2_CONST22 = 0x8D800000;
constexpr uint32_t MOVL_XAR3_CONST22 = 0x8DC00000;
constexpr uint32_t MOVL_XAR4_CONST22 = 0x8F000000;
constexpr uint32_t MOVL_XAR5_CONST22 = 0x8F400000;
constexpr uint32_t MOVL_XAR6_CONST22 = 0x76800000;
constexpr uint32_t MOVL_XAR7_CONST22 = 0x76C00000;
constexpr uint32_t MOVL_XT_LOC32 = 0x8700;
constexpr uint32_t MOVP_T_LOC16 = 0x1600;
constexpr uint32_t MOVS_T_LOC16 = 0x1100;
constexpr uint32_t MOVU_ACC_LOC16 = 0x0E00;
constexpr uint32_t MOVU_LOC16_OVC = 0x56280000;
constexpr uint32_t MOVU_OVC_LOC16 = 0x56620000;
constexpr uint32_t MOVW_DP_CONST16 = 0x761F0000;
constexpr uint32_t MOVX_TL_LOC16 = 0x56210000;
constexpr uint32_t MOVZ_AR0_LOC16 = 0x5800;
constexpr uint32_t MOVZ_AR1_LOC16 = 0x5900;
constexpr uint32_t MOVZ_AR2_LOC16 = 0x5A00;
constexpr uint32_t MOVZ_AR3_LOC16 = 0x5B00;
constexpr uint32_t MOVZ_AR4_LOC16 = 0x5C00;
constexpr uint32_t MOVZ_AR5_LOC16 = 0x5D00;
constexpr uint32_t MOVZ_AR6_LOC16 = 0x8800;
constexpr uint32_t MOVZ_AR7_LOC16 = 0x8000;
constexpr uint32_t MOVZ_DP_CONST10 = 0xB800;
constexpr uint32_t MPY_ACC_LOC16_CONST16 = 0x34000000;
constexpr uint32_t MPY_ACC_T_LOC16 = 0x1200;
constexpr uint32_t MPY_P_LOC16_CONST16 = 0x8C000000;
constexpr uint32_t MPY_P_T_LOC16 = 0x3300;
constexpr uint32_t MPYA_P_LOC16_CONST16 = 0x15000000;
constexpr uint32_t MPYA_P_T_LOC16 = 0x1700;
constexpr uint32_t MPYB_ACC_T_CONST8 = 0x3500;
constexpr uint32_t MPYB_P_T_CONST8 = 0x3100;
constexpr uint32_t MPYS_P_T_LOC16 = 0x1300;
constexpr uint32_t MPYU_P_T_LOC16 = 0x3700;
constexpr uint32_t MPYU_ACC_T_LOC16 = 0x3600;
constexpr uint32_t MPYXU_ACC_T_LOC16 = 0x3000;
constexpr uint32_t MPYXU_P_T_LOC16 = 0x3200;
constexpr uint32_t NASP = 0x7617;
constexpr uint32_t NEG_ACC = 0xFF54;
constexpr uint32_t NEG_AX = 0xFF5C;
constexpr uint32_t NEG64_ACC_P = 0x5658;
constexpr uint32_t NEGTC_ACC = 0x5632;
constexpr uint32_t NOP_IND_ARPN = 0x7700;
constexpr uint32_t NORM_ACC_IND = 0x5624;
constexpr uint32_t NORM_ACC_IND_POSTINC = 0x565A;
constexpr uint32_t NORM_ACC_IND_POSTDEC = 0x5620;
constexpr uint32_t NORM_ACC_IND0_POSTINC = 0x5677;
constexpr uint32_t NORM_ACC_IND0_POSTDEC = 0x5630;
constexpr uint32_t NORM_ACC_XARN_POSTINC = 0xFF78;
constexpr uint32_t NORM_ACC_XARN_POSTDEC = 0xFF70;
constexpr uint32_t NOT_ACC = 0xFF55;
constexpr uint32_t NOT_AX = 0xFF5E;
constexpr uint32_t OR_ACC_LOC16 = 0xAF00;
constexpr uint32_t OR_ACC_CONST16_SHIFT0_15 = 0x3E100000;
constexpr uint32_t OR_ACC_CONST16_SHIFT16 = 0x564A0000;
constexpr uint32_t OR_AX_LOC16 = 0xCA00;
constexpr uint32_t OR_IER_CONST16 = 0x76230000;
constexpr uint32_t OR_IFR_CONST16 = 0x76270000;
constexpr uint32_t OR_LOC16_CONST16 = 0x1A000000;
constexpr uint32_t OR_LOC16_AX = 0x9800;
constexpr uint32_t ORB_AX_CONST8 = 0x5000;
constexpr uint32_t OUT_PA_LOC16 = 0xBC000000;
// POP_ACC, alias for: MOVL ACC, *--SP
constexpr uint32_t POP_AR1_AR0 = 0x7607;
constexpr uint32_t POP_AR3_AR2 = 0x7605;
constexpr uint32_t POP_AR5_AR4 = 0x7606;
constexpr uint32_t POP_AR1H_AR0H = 0x0003;
constexpr uint32_t POP_DBGIER = 0x7612;
constexpr uint32_t POP_DP = 0x7603;
constexpr uint32_t POP_DP_ST1 = 0x7601;
constexpr uint32_t POP_IFR = 0x0002;
constexpr uint32_t POP_LOC16 = 0x2A00;
constexpr uint32_t POP_P = 0x7611;
constexpr uint32_t POP_RPC = 0x0007;
constexpr uint32_t POP_ST0 = 0x7613;
constexpr uint32_t POP_ST1 = 0x7600;
constexpr uint32_t POP_T_ST0 = 0x7615;
// POP_XAR0-7, alias for MOVL XAR0-7, *--SP
constexpr uint32_t POP_XT = 0x87BE;
constexpr uint32_t PREAD_LOC16_XAR7 = 0x2400;
// PUSH_ACC, alias for: MOV *SP++, ACC
constexpr uint32_t PUSH_AR1_AR0 = 0x760D;
constexpr uint32_t PUSH_AR3_AR2 = 0x760F;
constexpr uint32_t PUSH_AR5_AR4 = 0x760C;
constexpr uint32_t PUSH_AR1H_AR0H = 0x0005;
constexpr uint32_t PUSH_DBGIER = 0x760E;
constexpr uint32_t PUSH_DP = 0x760B;
constexpr uint32_t PUSH_DP_ST1 = 0x7609;
constexpr uint32_t PUSH_IFR = 0x760A;
constexpr uint32_t PUSH_LOC16 = 0x2200;
constexpr uint32_t PUSH_P = 0x761D;
constexpr uint32_t PUSH_RPC = 0x0004;
constexpr uint32_t PUSH_ST0 = 0x7618;
constexpr uint32_t PUSH_ST1 = 0x7608;
constexpr uint32_t PUSH_T_ST0 = 0x7619;
// PUSH_XAR0-7, alias for MOVL *SP++, XAR0-7
constexpr uint32_t PUSH_XT = 0xABBD;
constexpr uint32_t PWRITE_XAR7_LOC16 = 0x2600;
constexpr uint32_t QMACL_P_LOC32_XAR7 = 0x564FC700;
constexpr uint32_t QMACL_P_LOC32_XAR7_POSTINC = 0x564F8700;
constexpr uint32_t QMPYAL_P_XT_LOC32 = 0x56460000;
constexpr uint32_t QMPYL_P_XT_LOC32 = 0x56670000;
constexpr uint32_t QMPYL_ACC_XT_LOC32 = 0x56630000;
constexpr uint32_t QMPYSL_P_XT_LOC32 = 0x56450000;
constexpr uint32_t QMPYUL_P_XT_LOC32 = 0x56470000;
constexpr uint32_t QMPYXUL_P_XT_LOC32 = 0x56420000;
constexpr uint32_t ROL_ACC = 0xFF53;
constexpr uint32_t ROR_ACC = 0xFF52;
constexpr uint32_t RPT_CONST8 = 0xF600;
constexpr uint32_t RPT_LOC16 = 0xF700;
constexpr uint32_t SAT_ACC = 0xFF57;
constexpr uint32_t SAT64_ACC_P = 0x563E;
constexpr uint32_t SB_OFF8_COND = 0x6000;
constexpr uint32_t SBBU_ACC_LOC16 = 0x1D00;
constexpr uint32_t SBF_OFF8_EQ = 0xEC00;
constexpr uint32_t SBF_OFF8_NEQ = 0xED00;
constexpr uint32_t SBF_OFF8_TC = 0xEE00;
constexpr uint32_t SBF_OFF8_NTC = 0xEF00;
constexpr uint32_t SBRK_CONST8 = 0xFD00;
constexpr uint32_t SETC_MODE = 0x3B00;
constexpr uint32_t SETC_M0M1MAP = 0x561A;  // alias: C28MAP
constexpr uint32_t SETC_OBJMODE = 0x561F;  // alias: C28OBJ
constexpr uint32_t SETC_XF = 0x5626;
constexpr uint32_t SFR_ACC_SHIFT = 0xFF40;
constexpr uint32_t SFR_ACC_T = 0xFF51;
constexpr uint32_t SPM_SHIFT = 0xFF68;
constexpr uint32_t SQRA_LOC16 = 0x56150000;
constexpr uint32_t SQRS_LOC16 = 0x56110000;
constexpr uint32_t SUB_ACC_LOC16_OBJMODE1 = 0xAE00;
constexpr uint32_t SUB_ACC_LOC16_OBJMODE0 = 0x8000;
constexpr uint32_t SUB_ACC_LOC16_SHIFT1_15_OBJMODE1 = 0x56000000;
constexpr uint32_t SUB_ACC_LOC16_SHIFT1_15_OBJMODE0 = 0x8000;
constexpr uint32_t SUB_ACC_LOC16_SHIFT16 = 0x0400;
constexpr uint32_t SUB_ACC_LOC16_SHIFT_T = 0x56270000;
constexpr uint32_t SUB_ACC_CONST16_SHIFT = 0xFF000000;
constexpr uint32_t SUB_AX_LOC16 = 0x9E00;
constexpr uint32_t SUB_LOC16_AX = 0x7400;
constexpr uint32_t SUBB_ACC_CONST8 = 0x1900;
constexpr uint32_t SUBB_SP_CONST7 = 0xFE80;
constexpr uint32_t SUBB_XARN_CONST7 = 0xD880;
constexpr uint32_t SUBBL_ACC_LOC32 = 0x56540000;
constexpr uint32_t SUBCU_ACC_LOC16 = 0x1F00;
constexpr uint32_t SUBCUL_ACC_LOC32 = 0x56170000;
constexpr uint32_t SUBL_ACC_LOC32 = 0x0300;
// SUBL_ACC_P_SHIFT_PM, alias for MOVS T, loc16 where loc16 = @T
constexpr uint32_t SUBL_LOC32_ACC = 0x56410000;
constexpr uint32_t SUBR_LOC16_AX = 0xEA00;
constexpr uint32_t SUBRL_LOC32_ACC = 0x56490000;
constexpr uint32_t SUBU_ACC_LOC16 = 0x0100;
constexpr uint32_t SUBUL_ACC_LOC32 = 0x56550000;
constexpr uint32_t SUBUL_P_LOC32 = 0x565D0000;
constexpr uint32_t TBIT_LOC16_BIT = 0x4000;
constexpr uint32_t TBIT_LOC16_T = 0x56250000;
constexpr uint32_t TCLR_LOC16_BIT = 0x56090000;
constexpr uint32_t TEST_ACC = 0xFF58;
constexpr uint32_t TRAP_VEC = 0x0020;
constexpr uint32_t TSET_LOC16_BIT = 0x560D0000;
constexpr uint32_t UOUT_PA_LOC16 = 0xB0000000;
constexpr uint32_t XB_AL = 0x5614;
constexpr uint32_t XB_PMA_APRN = 0x3EE00000;
constexpr uint32_t XB_PMA_COND = 0x56D00000;
constexpr uint32_t XBANZ_PMA_IND = 0x560C0000;
constexpr uint32_t XBANZ_PMA_IND_POSTINC = 0x560A0000;
constexpr uint32_t XBANZ_PMA_IND_POSTDEC = 0x560B0000;
constexpr uint32_t XBANZ_PMA_IND0_POSTINC = 0x560E0000;
constexpr uint32_t XBANZ_PMA_IND0_POSTDEC = 0x560F0000;
constexpr uint32_t XBANZ_PMA_IND_ARPN = 0x3E300000;
constexpr uint32_t XBANZ_PMA_IND_POSTINC_ARPN = 0x3E380000;
constexpr uint32_t XBANZ_PMA_IND_POSTDEC_ARPN = 0x3E400000;
constexpr uint32_t XBANZ_PMA_IND0_POSTINC_ARPN = 0x3E480000;
constexpr uint32_t XBANZ_PMA_IND0_POSTDEC_ARPN = 0x3E500000;
constexpr uint32_t XCALL_AL = 0x5634;
constexpr uint32_t XCALL_PMA_ARPN = 0x3E680000;
constexpr uint32_t XCALL_PMA_COND = 0x56E00000;
constexpr uint32_t XMAC_P_LOC16_PMA = 0x84000000;
constexpr uint32_t XMACD_P_LOC16_PMA = 0xA4000000;
constexpr uint32_t XOR_ACC_LOC16 = 0xB700;
constexpr uint32_t XOR_ACC_CONST16_SHIFT0_15 = 0x3E200000;
constexpr uint32_t XOR_ACC_CONST16_SHIFT16 = 0x564E0000;
constexpr uint32_t XOR_AX_LOC16 = 0x7000;
constexpr uint32_t XOR_LOC16_AX = 0xF200;
constexpr uint32_t XOR_LOC16_CONST16 = 0x1C000000;
constexpr uint32_t XORB_AX_CONST8 = 0xF000;
constexpr uint32_t XPREAD_LOC16_PMA = 0xAC000000;
constexpr uint32_t XPREAD_LOC16_AL = 0x563C0000;
constexpr uint32_t XPWRITE_AL_LOC16 = 0x563D0000;
// XRET, alias for XRETC UNCONDITIONAL
constexpr uint32_t XRETC_COND = 0x56F0;
constexpr uint32_t ZALR_ACC_LOC16 = 0x56130000;
// ZAP OVC, alias for CLRC OVC
constexpr uint32_t ZAPA = 0x5633;

/**
 * C28x Extended Instructions
 *
 * The extended instruction set consists of instructions for 7 different
 * processor accelerators:
 * - FPU32 (32-bit floating point unit)
 * - FPU64 (64-bit floating point unit)
 * - VCU (Viterbi, complex math & CRC unit)
 * - VCRC (Cyclic redundancy check unit)
 * - VCU-II (C28 Viterbi, complex math & CRC unit II)
 * - FINTDIV (Fast integer division unit)
 * - TMU (Trigonometric math unit)
 */

/* FPU Instructions  */
constexpr uint32_t ABSF32_RAH_RBH = 0xE6950000;
constexpr uint32_t ADDF32_RAH_16FHI_RBH = 0xE8800000;
// constexpr uint32_t ADDF32_RAH_RBH_16FHI = 0xE8800000; // alias for
// ADDF32_RAH_16FHI_RBH
constexpr uint32_t ADDF32_RAH_RBH_RCH = 0xE7100000;
constexpr uint32_t ADDF32_RDH_REH_RFH_MOV32_MEM32_RAH = 0xE0100000;
constexpr uint32_t ADDF32_RDH_REH_RFH_MOV32_RAH_MEM32 = 0xE3100000;
constexpr uint32_t CMPF32_RAH_RBH = 0xE6940000;
constexpr uint32_t CMPF32_RAH_16FHI = 0xE8100000;
constexpr uint32_t CMPF32_RAH_0 = 0xE5A0;
constexpr uint32_t EINVF32_RAH_RBH = 0xE6930000;
constexpr uint32_t EISQRTF32_RAH_RBH = 0xE6920000;
constexpr uint32_t F32TOI16_RAH_RBH = 0xE68C0000;
constexpr uint32_t F32TOI16R_RAH_RBH = 0xE68C8000;
constexpr uint32_t F32TOI32_RAH_RBH = 0xE6880000;
constexpr uint32_t F32TOUI16_RAH_RBH = 0xE68E0000;
constexpr uint32_t F32TOUI16R_RAH_RBH = 0xE68E8000;
constexpr uint32_t F32TOUI32_RAH_RBH = 0xE68A0000;
constexpr uint32_t FRACF32_RAH_RBH = 0xE6F10000;
constexpr uint32_t I16TOF32_RAH_RBH = 0xE68D0000;
constexpr uint32_t I16TOF32_RAH_MEM16 = 0xE2C80000;
constexpr uint32_t I32TOF32_RAH_MEM32 = 0xE2880000;
constexpr uint32_t I32TOF32_RAH_RBH = 0xE6890000;
constexpr uint32_t MACF32_R3H_R2H_RDH_REH_RFH = 0xE7400000;
constexpr uint32_t MACF32_R3H_R2H_RDH_REH_RFH_MOV32_RAH_MEM32 = 0xE3300000;
constexpr uint32_t MACF32_R7H_R3H_MEM32_XAR7_POSTINC = 0xE2501F00;
constexpr uint32_t MACF32_R7H_R6H_RDH_REH_RFH = 0xE3C00000;
constexpr uint32_t MACF32_R7H_R6H_RDH_REH_RFH_MOV32_RAH_MEM32 = 0xE3C00000;
constexpr uint32_t MAXF32_RAH_RBH = 0xE6960000;
constexpr uint32_t MAXF32_RAH_16FHI = 0xE8200000;
constexpr uint32_t MAXF32_RAH_RBH_MOV32_RCH_RDH = 0xE69C0000;
constexpr uint32_t MINF32_RAH_RBH = 0xE6970000;
constexpr uint32_t MINF32_RAH_16FHI = 0xE8300000;
constexpr uint32_t MINF32_RAH_RBH_MOV32_RCH_RDH = 0xE69D0000;
constexpr uint32_t MOV16_MEM16_RAH = 0xE2130000;

/* VCU Instructions  */
// TODO

/* VCRC Instructions  */
// TODO

/* VCU-II Instructions  */
// TODO

/* FINTDIV Instructions  */
// TODO

/* TMU Instructions  */
// TODO

}  // namespace TIC28X::Opcodes

namespace TIC28X::OpcodeMasks {
// 2-byte instruction masks
constexpr uint32_t MASK_F000 = 0xF000;
constexpr uint32_t MASK_F800 = 0xF800;
constexpr uint32_t MASK_FC00 = 0xFC00;
constexpr uint32_t MASK_FE00 = 0xFE00;
constexpr uint32_t MASK_F880 = 0xF880;
constexpr uint32_t MASK_FF00 = 0xFF00;
constexpr uint32_t MASK_FF80 = 0xFF80;
constexpr uint32_t MASK_FFE0 = 0xFFE0;
constexpr uint32_t MASK_FFF0 = 0xFFF0;
constexpr uint32_t MASK_FFF8 = 0xFFF8;
constexpr uint32_t MASK_FFFE = 0xFFFE;
constexpr uint32_t MASK_FFFF = 0xFFFF;

// 4-byte instruction masks
constexpr uint32_t MASK_FE000000 = 0xFE000000;
constexpr uint32_t MASK_FF000000 = 0xFF000000;
constexpr uint32_t MASK_FFC00000 = 0xFFC00000;
constexpr uint32_t MASK_FFF00000 = 0xFFF00000;
constexpr uint32_t MASK_FFF80000 = 0xFFF80000;
constexpr uint32_t MASK_FFFC0000 = 0xFFFC0000;
constexpr uint32_t MASK_FFFF0000 = 0xFFFF0000;
constexpr uint32_t MASK_FFFEF000 = 0xFFFEF000;
constexpr uint32_t MASK_FFFFF000 = 0xFFFFF000;
constexpr uint32_t MASK_FFFFF800 = 0xFFFFF800;
constexpr uint32_t MASK_FFFFFE00 = 0xFFFFFE00;
constexpr uint32_t MASK_FFFEFF00 = 0xFFFEFF00;
constexpr uint32_t MASK_FFFFFF00 = 0xFFFFFF00;
constexpr uint32_t MASK_FFFFFFC0 = 0xFFFFFFC0;
}  // namespace TIC28X::OpcodeMasks

#endif  // TIC28X_OPCODES_H
