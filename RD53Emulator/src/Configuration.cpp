#include "RD53Emulator/Configuration.h"

using namespace std;
using namespace RD53A;

std::map<std::string, uint32_t> Configuration::m_name2index={
    {"AdcRead", ADC_READ},
    {"AdcRefTrim", ADC_TRIM},
    {"AdcTrim", ADC_TRIM},
    {"AiPixCol", AI_PIX_COL},
    {"AiPixRow", AI_PIX_ROW},
    {"AuroraCbSend", AURORA_CB_SEND},
    {"AuroraCbWaitHigh", AURORA_CB_WAIT_1},
    {"AuroraCbWaitLow", AURORA_CB_WAIT_0},
    {"AuroraCcSend", AURORA_CC_SEND},
    {"AuroraCcWait", AURORA_CC_WAIT},
    {"AuroraInitWait", AURORA_INIT_WAIT},
    {"AutoReadA0", AUTO_READ_A0},
    {"AutoReadA1", AUTO_READ_A1},
    {"AutoReadA2", AUTO_READ_A2},
    {"AutoReadA3", AUTO_READ_A3},
    {"AutoReadB0", AUTO_READ_B0},
    {"AutoReadB1", AUTO_READ_B1},
    {"AutoReadB2", AUTO_READ_B2},
    {"AutoReadB3", AUTO_READ_B3},
    {"BcCounter", BC_CTR},
    {"BflipErrCounter", BFLIP_ERR_CTR},
    {"BflipWarnCounter", BFLIP_WARN_CTR},
    {"CalColprDiff1", CAL_COLPR_DIFF_1},
    {"CalColprDiff2", CAL_COLPR_DIFF_2},
    {"CalColprDiff3", CAL_COLPR_DIFF_3},
    {"CalColprDiff4", CAL_COLPR_DIFF_4},
    {"CalColprDiff5", CAL_COLPR_DIFF_5},
    {"CalColprLin1", CAL_COLPR_LIN_1},
    {"CalColprLin2", CAL_COLPR_LIN_2},
    {"CalColprLin3", CAL_COLPR_LIN_3},
    {"CalColprLin4", CAL_COLPR_LIN_4},
    {"CalColprLin5", CAL_COLPR_LIN_5},
    {"CalColprSync1", CAL_COLPR_SYNC_1},
    {"CalColprSync2", CAL_COLPR_SYNC_2},
    {"CalColprSync3", CAL_COLPR_SYNC_3},
    {"CalColprSync4", CAL_COLPR_SYNC_4},
    {"CdrCpIbias", CDR_CP_IBIAS},
    {"CdrEnGck", CDR_CONFIG},
    {"CdrPdDel", CDR_CONFIG},
    {"CdrPdSel", CDR_CONFIG},
    {"CdrSelDelClk", CDR_CONFIG},
    {"CdrSelSerClk", CDR_CONFIG},
    {"CdrVcoGain", CDR_CONFIG},
    {"ChSyncLock", CH_SYNC_LOCK},
    {"ChSyncPhase", CH_SYNC_PHASE},
    {"ChSyncUnlock", CH_SYNC_UNLOCK},
    {"ClkDelay", CLK_DELAY},
    {"ClkDelaySel", CLK_DELAY_SEL},
    {"CmdDelay", CMD_DELAY},
    {"CmdErrCounter", CMD_ERR_CTR},
    {"CmlEn", CML_EN},
    {"CmlEnTap", CML_EN_TAP},
    {"CmlInvTap", CML_INV_TAP},
    {"CmlTapBias0", CML_TAP_BIAS_1},
    {"CmlTapBias1", CML_TAP_BIAS_2},
    {"CmlTapBias2", CML_TAP_BIAS_3},
    {"DebugConfig", DEBUG_CONFIG},
    {"DiffComp", COMP_DIFF},
    {"DiffFbCapEn", DIFF_FB_CAP_EN},
    {"DiffFol", FOL_DIFF},
    {"DiffLcc", LCC_DIFF},
    {"DiffLccEn", LCC_EN},
    {"DiffPrecomp", PRECOMP_DIFF},
    {"DiffPrmp", PRMP_DIFF},
    {"DiffVff", VFF_DIFF},
    {"DiffVth1", VTH1_DIFF},
    {"DiffVth2", VTH2_DIFF},
    {"EnCoreColDiff1", EN_CORE_COL_DIFF_1},
    {"EnCoreColDiff2", EN_CORE_COL_DIFF_2},
    {"EnCoreColLin1", EN_CORE_COL_LIN_1},
    {"EnCoreColLin2", EN_CORE_COL_LIN_2},
    {"EnCoreColSync", EN_CORE_COL_SYNC},
    {"ErrMask", ERR_MASK},
    {"FifoFullCounter0", FIFO_FULL_CTR_1},
    {"FifoFullCounter1", FIFO_FULL_CTR_2},
    {"FifoFullCounter2", FIFO_FULL_CTR_3},
    {"FifoFullCounter3", FIFO_FULL_CTR_4},
    {"GlobalPulseRt", GLOBAL_PULSE_RT},
    {"GpLvdsRoute", GP_LVDS_ROUTE},
    {"HitOr0MaskDiff0", HITOR_MASK_DIFF_11},
    {"HitOr0MaskDiff1", HITOR_MASK_DIFF_12},
    {"HitOr0MaskLin0", HITOR_MASK_LIN_11},
    {"HitOr0MaskLin1", HITOR_MASK_LIN_12},
    {"HitOr0MaskSync", HITOR_MASK_SYNC_1},
    {"HitOr1MaskDiff0", HITOR_MASK_DIFF_21},
    {"HitOr1MaskDiff1", HITOR_MASK_DIFF_22},
    {"HitOr1MaskLin0", HITOR_MASK_LIN_21},
    {"HitOr1MaskLin1", HITOR_MASK_LIN_22},
    {"HitOr1MaskSync", HITOR_MASK_SYNC_2},
    {"HitOr2MaskDiff0", HITOR_MASK_DIFF_31},
    {"HitOr2MaskDiff1", HITOR_MASK_DIFF_32},
    {"HitOr2MaskLin0", HITOR_MASK_LIN_31},
    {"HitOr2MaskLin1", HITOR_MASK_LIN_32},
    {"HitOr2MaskSync", HITOR_MASK_SYNC_3},
    {"HitOr3MaskDiff0", HITOR_MASK_DIFF_41},
    {"HitOr3MaskDiff1", HITOR_MASK_DIFF_42},
    {"HitOr3MaskLin0", HITOR_MASK_LIN_41},
    {"HitOr3MaskLin1", HITOR_MASK_LIN_42},
    {"HitOr3MaskSync", HITOR_MASK_SYNC_4},
    {"HitOrCounter0", HITOR_CNT_1},
    {"HitOrCounter1", HITOR_CNT_2},
    {"HitOrCounter2", HITOR_CNT_3},
    {"HitOrCounter3", HITOR_CNT_4},
    {"InjAnaMode", INJ_MODE_ANA},
    {"InjDelay", INJ_FINE_DELAY},
    {"InjEnDig", INJ_MODE_DIG},
    {"InjVcalHigh", VCAL_HIGH},
    {"InjVcalMed", VCAL_MED},
    {"LatencyConfig", LATENCY_CONFIG},
    {"LinComp", COMP_LIN},
    {"LinFcBias", FC_BIAS_LIN},
    {"LinKrumCurr", KRUM_CURR_LIN},
    {"LinLdac", LDAC_LIN},
    {"LinPaInBias", PA_IN_BIAS_LIN},
    {"LinRefKrum", REF_KRUM_LIN},
    {"LinVth", VTH_LIN},
    {"LockLossCounter", LCK_LOSS_CTR},
    {"MonFrameSkip", MON_FRAME_SKIP},
    {"MonitorEnable", MONITOR_EN},
    {"MonitorImonMux", MONITOR_IMON_MUX},
    {"MonitorVmonMux", MONITOR_VMON_MUX},
    {"OutPadConfig", OUT_PAD_CONFIG},
    {"OutputActiveLanes", OUT_ACTIVE_LANES},
    {"OutputDataReadDelay", OUT_DATA_DELAY},
    {"OutputFmt", OUT_FORMAT},
    {"OutputSerType", OUT_SERIAL_TYPE},
    {"PixAutoCol", PIX_AUTO_COL},
    {"PixAutoRow", PIX_AUTO_ROW},
    {"PixBroadcastEn", PIX_BCAST_EN},
    {"PixBroadcastMask", PIX_BCAST_MASK},
    {"PixDefaultConfig", PIX_DEFAULT_CONFIG},
    {"PixPortal", PIX_PORTAL},
    {"PixRegionCol", REGION_COL},
    {"PixRegionRow", REGION_ROW},
    {"RingOsc0", RING_OSC_1},
    {"RingOsc1", RING_OSC_2},
    {"RingOsc2", RING_OSC_3},
    {"RingOsc3", RING_OSC_4},
    {"RingOsc4", RING_OSC_5},
    {"RingOsc5", RING_OSC_6},
    {"RingOsc6", RING_OSC_7},
    {"RingOsc7", RING_OSC_8},
    {"RingOscEn", RING_OSC_ENABLE},
    {"SelfTrigEn", SELF_TRIG_EN},
    {"SensorCfg0", SENSOR_CONFIG_0},
    {"SensorCfg1", SENSOR_CONFIG_1},
    {"SensorBias0", SENSOR_BIAS_0},
    {"SensorBias1", SENSOR_BIAS_1},
    {"SensorBias2", SENSOR_BIAS_2},
    {"SensorBias3", SENSOR_BIAS_3},
    {"SensorCurrent0",SENSOR_CURRENT_0},
    {"SensorCurrent1",SENSOR_CURRENT_1},
    {"SensorCurrent2",SENSOR_CURRENT_2},
    {"SensorCurrent3",SENSOR_CURRENT_3},
    {"SensorEnable0",SENSOR_ENABLE_0},
    {"SensorEnable1",SENSOR_ENABLE_1},
    {"SensorEnable2",SENSOR_ENABLE_2},
    {"SensorEnable3",SENSOR_ENABLE_3},
    {"SerSelOut0", SER_SEL_OUT_0},
    {"SerSelOut1", SER_SEL_OUT_1},
    {"SerSelOut2", SER_SEL_OUT_2},
    {"SerSelOut3", SER_SEL_OUT_3},
    {"SkipTriggerCounter", SKP_TRIG_CNT},
    {"SldoAnalogTrim", SLDO_ANALOG_TRIM},
    {"SldoDigitalTrim", SLDO_DIGITAL_TRIM},
    {"SyncAutoZero", AUTO_ZERO},
    {"SyncFastTot", FAST_TOT},
    {"SyncIbiasDisc", IBIAS_DISC_SYNC},
    {"SyncIbiasKrum", IBIAS_KRUM_SYNC},
    {"SyncIbiasSf", IBIAS_SF_SYNC},
    {"SyncIbiasp1", IBIASP1_SYNC},
    {"SyncIbiasp2", IBIASP2_SYNC},
    {"SyncIctrlSynct", ICTRL_SYNCT_SYNC},
    {"SyncSelC2F", SEL_C2F},
    {"SyncSelC4F", SEL_C4F},
    {"SyncVbl", VBL_SYNC},
    {"SyncVrefKrum", VREF_KRUM_SYNC},
    {"SyncVth", VTH_SYNC},
    {"TrigCounter", TRIG_CTR},
    {"VcoBuffBias", VCO_BUFF_BIAS},
    {"VcoIbias", VCO_IBIAS},
    {"WrSyncDelaySync", WR_SYNC_DELAY_SYNC}
};

Configuration::Configuration(){

  m_verbose = 1;

  m_registers.resize(138);

  m_fields[PIX_PORTAL]           = new Field(&m_registers[  0], 0,16,0);
  m_fields[REGION_COL]           = new Field(&m_registers[  1], 0, 8,0);
  m_fields[REGION_ROW]           = new Field(&m_registers[  2], 0, 9,0);
  m_fields[PIX_MODE]             = new Field(&m_registers[  3], 3, 3,0);
  m_fields[PIX_BCAST_EN]         = new Field(&m_registers[  3], 5, 1,0);
  m_fields[PIX_AUTO_COL]         = new Field(&m_registers[  3], 4, 1,0);
  m_fields[PIX_AUTO_ROW]         = new Field(&m_registers[  3], 3, 1,0);
  m_fields[PIX_BCAST_MASK]       = new Field(&m_registers[  3], 0, 3,0);
  m_fields[PIX_DEFAULT_CONFIG]   = new Field(&m_registers[  4], 0,16,0x9CE2);

  m_fields[IBIASP1_SYNC]         = new Field(&m_registers[  5], 0,16,100);
  m_fields[IBIASP2_SYNC]         = new Field(&m_registers[  6], 0,16,150);
  m_fields[IBIAS_SF_SYNC]        = new Field(&m_registers[  7], 0,16,100);
  m_fields[IBIAS_KRUM_SYNC]      = new Field(&m_registers[  8], 0,16,140);
  m_fields[IBIAS_DISC_SYNC]      = new Field(&m_registers[  9], 0,16,200);
  m_fields[ICTRL_SYNCT_SYNC]     = new Field(&m_registers[ 10], 0,16,100);
  m_fields[VBL_SYNC]             = new Field(&m_registers[ 11], 0,16,450);
  m_fields[VTH_SYNC]             = new Field(&m_registers[ 12], 0,16,300);
  m_fields[VREF_KRUM_SYNC]       = new Field(&m_registers[ 13], 0,16,490);
  m_fields[AUTO_ZERO]            = new Field(&m_registers[ 30], 3, 1,0);
  m_fields[SEL_C2F]              = new Field(&m_registers[ 30], 2, 1,1);
  m_fields[SEL_C4F]              = new Field(&m_registers[ 30], 1, 1,0);
  m_fields[FAST_TOT]             = new Field(&m_registers[ 30], 0, 1,0);

  m_fields[PA_IN_BIAS_LIN]       = new Field(&m_registers[ 14], 0, 9,300);
  m_fields[FC_BIAS_LIN]          = new Field(&m_registers[ 15], 0, 8,20);
  m_fields[KRUM_CURR_LIN]        = new Field(&m_registers[ 16], 0, 9,50);
  m_fields[LDAC_LIN]             = new Field(&m_registers[ 17], 0,10,80);
  m_fields[COMP_LIN]             = new Field(&m_registers[ 18], 0, 9,110);
  m_fields[REF_KRUM_LIN]         = new Field(&m_registers[ 19], 0,10,300);
  m_fields[VTH_LIN]              = new Field(&m_registers[ 20], 0,10,408);

  m_fields[PRMP_DIFF]            = new Field(&m_registers[ 21], 0,10,533);
  m_fields[FOL_DIFF]             = new Field(&m_registers[ 22], 0,10,542);
  m_fields[PRECOMP_DIFF]         = new Field(&m_registers[ 23], 0,10,551);
  m_fields[COMP_DIFF]            = new Field(&m_registers[ 24], 0,10,528);
  m_fields[VFF_DIFF]             = new Field(&m_registers[ 25], 0,10,164);
  m_fields[VTH1_DIFF]            = new Field(&m_registers[ 26], 0,10,1023);
  m_fields[VTH2_DIFF]            = new Field(&m_registers[ 27], 0,10,0);
  m_fields[LCC_DIFF]             = new Field(&m_registers[ 28], 0,10,20);
  m_fields[LCC_EN]               = new Field(&m_registers[ 29], 1, 1,1);
  m_fields[DIFF_FB_CAP_EN]       = new Field(&m_registers[ 29], 0, 1,0);

  m_fields[SLDO_ANALOG_TRIM]     = new Field(&m_registers[ 31], 5,5,16);
  m_fields[SLDO_DIGITAL_TRIM]    = new Field(&m_registers[ 31], 0,5,16);

  m_fields[EN_CORE_COL_SYNC]     = new Field(&m_registers[ 32], 0,16,0xFF);
  m_fields[EN_CORE_COL_LIN_1]    = new Field(&m_registers[ 33], 0,16,0xFF);
  m_fields[EN_CORE_COL_LIN_2]    = new Field(&m_registers[ 34], 0, 1,1);
  m_fields[EN_CORE_COL_DIFF_1]   = new Field(&m_registers[ 35], 0,16,0xFF);
  m_fields[EN_CORE_COL_DIFF_2]   = new Field(&m_registers[ 36], 0, 1,1);
  m_fields[LATENCY_CONFIG]       = new Field(&m_registers[ 37], 0, 9,500);
  m_fields[WR_SYNC_DELAY_SYNC]   = new Field(&m_registers[ 38], 0, 5,16);

  m_fields[INJ_MODE_ANA]         = new Field(&m_registers[ 39], 5, 1,0);
  m_fields[INJ_MODE_DIG]         = new Field(&m_registers[ 39], 4, 1,1);
  m_fields[INJ_FINE_DELAY]       = new Field(&m_registers[ 39], 0, 4,0);
  m_fields[VCAL_HIGH]            = new Field(&m_registers[ 41], 0,12,500);
  m_fields[VCAL_MED]             = new Field(&m_registers[ 42], 0,12,300);
  m_fields[CAL_COLPR_SYNC_1]     = new Field(&m_registers[ 46], 0,16,0xFFFF);
  m_fields[CAL_COLPR_SYNC_2]     = new Field(&m_registers[ 47], 0,16,0xFFFF);
  m_fields[CAL_COLPR_SYNC_3]     = new Field(&m_registers[ 48], 0,16,0xFFFF);
  m_fields[CAL_COLPR_SYNC_4]     = new Field(&m_registers[ 49], 0,16,0xFFFF);
  m_fields[CAL_COLPR_LIN_1]      = new Field(&m_registers[ 50], 0,16,0xFFFF);
  m_fields[CAL_COLPR_LIN_2]      = new Field(&m_registers[ 51], 0,16,0xFFFF);
  m_fields[CAL_COLPR_LIN_3]      = new Field(&m_registers[ 52], 0,16,0xFFFF);
  m_fields[CAL_COLPR_LIN_4]      = new Field(&m_registers[ 53], 0,16,0xFFFF);
  m_fields[CAL_COLPR_LIN_5]      = new Field(&m_registers[ 54], 0, 4,0xF);
  m_fields[CAL_COLPR_DIFF_1]     = new Field(&m_registers[ 55], 0,16,0xFFFF);
  m_fields[CAL_COLPR_DIFF_2]     = new Field(&m_registers[ 56], 0,16,0xFFFF);
  m_fields[CAL_COLPR_DIFF_3]     = new Field(&m_registers[ 57], 0,16,0xFFFF);
  m_fields[CAL_COLPR_DIFF_4]     = new Field(&m_registers[ 58], 0,16,0xFFFF);
  m_fields[CAL_COLPR_DIFF_5]     = new Field(&m_registers[ 59], 0, 4,0xF);

  m_fields[CLK_DATA_DELAY]       = new Field(&m_registers[ 40], 0, 9,0);
  m_fields[CLK_DELAY]            = new Field(&m_registers[ 40], 8, 1,0);
  m_fields[CLK_DELAY_SEL]        = new Field(&m_registers[ 40], 4, 4,0);
  m_fields[CMD_DELAY]            = new Field(&m_registers[ 40], 0, 4,0);
  m_fields[CH_SYNC_CONF]         = new Field(&m_registers[ 43], 0,11,0);
  m_fields[CH_SYNC_PHASE]        = new Field(&m_registers[ 43],10, 1,0);
  m_fields[CH_SYNC_LOCK]         = new Field(&m_registers[ 43], 5, 5,0);
  m_fields[CH_SYNC_UNLOCK]       = new Field(&m_registers[ 43], 0, 5,0);
  m_fields[GLOBAL_PULSE_RT]      = new Field(&m_registers[ 44], 0,16,0);

  m_fields[DEBUG_CONFIG]         = new Field(&m_registers[ 60], 0, 2,0);
  m_fields[OUT_DATA_DELAY]       = new Field(&m_registers[ 61], 7, 2,0);
  m_fields[OUT_SERIAL_TYPE]      = new Field(&m_registers[ 61], 6, 1,0);
  m_fields[OUT_ACTIVE_LANES]     = new Field(&m_registers[ 61], 2, 4,1);
  m_fields[OUT_FORMAT]           = new Field(&m_registers[ 61], 0,1,0);
  m_fields[OUT_PAD_CONFIG]       = new Field(&m_registers[ 62], 0,14,0);
  m_fields[GP_LVDS_ROUTE]        = new Field(&m_registers[ 63], 0, 3,0);
  m_fields[CDR_CONFIG]           = new Field(&m_registers[ 64], 0,14,0);
  m_fields[VCO_BUFF_BIAS]        = new Field(&m_registers[ 65], 0,10,0);
  m_fields[CDR_CP_IBIAS]         = new Field(&m_registers[ 66], 0,10,0);
  m_fields[VCO_IBIAS]            = new Field(&m_registers[ 67], 0,14,0);
  m_fields[SER_SEL_OUT]          = new Field(&m_registers[ 68], 0, 8,0);
  m_fields[CML_CONFIG]           = new Field(&m_registers[ 69], 0, 8,0xF);
  m_fields[CML_INV_TAP]          = new Field(&m_registers[ 69], 6, 2,0);
  m_fields[CML_EN_TAP]           = new Field(&m_registers[ 69], 4, 2,0);
  m_fields[CML_EN]               = new Field(&m_registers[ 69], 0, 4,0xF);
  m_fields[CML_TAP_BIAS_1]       = new Field(&m_registers[ 70], 0,10,500);
  m_fields[CML_TAP_BIAS_2]       = new Field(&m_registers[ 71], 0,10,0);
  m_fields[CML_TAP_BIAS_3]       = new Field(&m_registers[ 72], 0,10,0);
  m_fields[AURORA_CC_CFG]        = new Field(&m_registers[ 73], 0, 8,103);
  m_fields[AURORA_CC_WAIT]       = new Field(&m_registers[ 73], 2, 6,25);
  m_fields[AURORA_CC_SEND]       = new Field(&m_registers[ 73], 0, 2,3);
  m_fields[AURORA_CB_CFG_1]      = new Field(&m_registers[ 74], 0, 8,60);
  m_fields[AURORA_CB_WAIT_1]     = new Field(&m_registers[ 74], 4, 4,15);
  m_fields[AURORA_CB_SEND]       = new Field(&m_registers[ 74], 0, 4,0);
  m_fields[AURORA_CB_CFG_0]      = new Field(&m_registers[ 75], 0,16,60);
  m_fields[AURORA_CB_WAIT_0]     = new Field(&m_registers[ 75], 0,16,15);
  m_fields[AURORA_INIT_WAIT]     = new Field(&m_registers[ 76], 0,16,32);
  m_fields[MON_FRAME_SKIP]       = new Field(&m_registers[ 45], 0, 8,50);
  m_fields[AUTO_READ_A0]         = new Field(&m_registers[101], 0, 8,136);
  m_fields[AUTO_READ_B0]         = new Field(&m_registers[102], 0, 8,130);
  m_fields[AUTO_READ_A1]         = new Field(&m_registers[103], 0, 8,118);
  m_fields[AUTO_READ_B1]         = new Field(&m_registers[104], 0, 8,119);
  m_fields[AUTO_READ_A2]         = new Field(&m_registers[105], 0, 8,120);
  m_fields[AUTO_READ_B2]         = new Field(&m_registers[106], 0, 8,121);
  m_fields[AUTO_READ_A3]         = new Field(&m_registers[107], 0, 8,122);
  m_fields[AUTO_READ_B3]         = new Field(&m_registers[108], 0, 8,123);

  m_fields[MONITOR_VMON_MUX]     = new Field(&m_registers[ 77], 0,16,0);
  m_fields[HITOR_MASK_SYNC_1]    = new Field(&m_registers[ 78], 0,16,0);
  m_fields[HITOR_MASK_SYNC_2]    = new Field(&m_registers[ 79], 0,16,0);
  m_fields[HITOR_MASK_SYNC_3]    = new Field(&m_registers[ 80], 0,16,0);
  m_fields[HITOR_MASK_SYNC_4]    = new Field(&m_registers[ 81], 0,16,0);
  m_fields[HITOR_MASK_LIN_11]    = new Field(&m_registers[ 82], 0,16,0);
  m_fields[HITOR_MASK_LIN_12]    = new Field(&m_registers[ 83], 0, 1,0);
  m_fields[HITOR_MASK_LIN_21]    = new Field(&m_registers[ 84], 0,16,0);
  m_fields[HITOR_MASK_LIN_22]    = new Field(&m_registers[ 85], 0, 1,0);
  m_fields[HITOR_MASK_LIN_31]    = new Field(&m_registers[ 86], 0,16,0);
  m_fields[HITOR_MASK_LIN_32]    = new Field(&m_registers[ 87], 0, 1,0);
  m_fields[HITOR_MASK_LIN_41]    = new Field(&m_registers[ 88], 0,16,0);
  m_fields[HITOR_MASK_LIN_42]    = new Field(&m_registers[ 89], 0, 1,0);
  m_fields[HITOR_MASK_DIFF_11]   = new Field(&m_registers[ 90], 0,16,0);
  m_fields[HITOR_MASK_DIFF_12]   = new Field(&m_registers[ 91], 0, 1,0);
  m_fields[HITOR_MASK_DIFF_21]   = new Field(&m_registers[ 92], 0,16,0);
  m_fields[HITOR_MASK_DIFF_22]   = new Field(&m_registers[ 93], 0, 1,0);
  m_fields[HITOR_MASK_DIFF_31]   = new Field(&m_registers[ 94], 0,16,0);
  m_fields[HITOR_MASK_DIFF_32]   = new Field(&m_registers[ 95], 0, 1,0);
  m_fields[HITOR_MASK_DIFF_41]   = new Field(&m_registers[ 96], 0,16,0);
  m_fields[HITOR_MASK_DIFF_42]   = new Field(&m_registers[ 97], 0, 1,0);
  m_fields[BANDGAP_TRIM]         = new Field(&m_registers[ 98], 6, 5,0);
  m_fields[ADC_TRIM]             = new Field(&m_registers[ 98], 0, 6,0);
  m_fields[SENSOR_CONFIG_0]      = new Field(&m_registers[ 99], 0,16,0);
  m_fields[SENSOR_BIAS_0]        = new Field(&m_registers[ 99], 0, 1,0);
  m_fields[SENSOR_CURRENT_0]     = new Field(&m_registers[ 99], 1, 4,0);
  m_fields[SENSOR_ENABLE_0]      = new Field(&m_registers[ 99], 5, 1,0);
  m_fields[SENSOR_BIAS_1]        = new Field(&m_registers[ 99], 6, 1,0);
  m_fields[SENSOR_CURRENT_1]     = new Field(&m_registers[ 99], 7, 4,0);
  m_fields[SENSOR_ENABLE_1]      = new Field(&m_registers[ 99],11, 1,0);
  m_fields[SENSOR_CONFIG_1]      = new Field(&m_registers[100], 0,16,0);
  m_fields[SENSOR_BIAS_2]        = new Field(&m_registers[100], 0, 1,0);
  m_fields[SENSOR_CURRENT_2]     = new Field(&m_registers[100], 1, 4,0);
  m_fields[SENSOR_ENABLE_2]      = new Field(&m_registers[100], 5, 1,0);
  m_fields[SENSOR_BIAS_3]        = new Field(&m_registers[100], 6, 1,0);
  m_fields[SENSOR_CURRENT_3]     = new Field(&m_registers[100], 7, 4,0);
  m_fields[SENSOR_ENABLE_3]      = new Field(&m_registers[100],11, 1,0);
  m_fields[RING_OSC_ENABLE]      = new Field(&m_registers[109], 0, 8,0);
  m_fields[RING_OSC_1]           = new Field(&m_registers[110], 0,16,0);
  m_fields[RING_OSC_2]           = new Field(&m_registers[111], 0,16,0);
  m_fields[RING_OSC_3]           = new Field(&m_registers[112], 0,16,0);
  m_fields[RING_OSC_4]           = new Field(&m_registers[113], 0,16,0);
  m_fields[RING_OSC_5]           = new Field(&m_registers[114], 0,16,0);
  m_fields[RING_OSC_6]           = new Field(&m_registers[115], 0,16,0);
  m_fields[RING_OSC_7]           = new Field(&m_registers[116], 0,16,0);
  m_fields[RING_OSC_8]           = new Field(&m_registers[117], 0,16,0);
  m_fields[BC_CTR]               = new Field(&m_registers[118], 0,16,0);
  m_fields[TRIG_CTR]             = new Field(&m_registers[119], 0,16,0);
  m_fields[LCK_LOSS_CTR]         = new Field(&m_registers[120], 0,16,0);
  m_fields[BFLIP_WARN_CTR]       = new Field(&m_registers[121], 0,16,0);
  m_fields[BFLIP_ERR_CTR]        = new Field(&m_registers[122], 0,16,0);
  m_fields[CMD_ERR_CTR]          = new Field(&m_registers[123], 0,16,0);
  m_fields[FIFO_FULL_CTR_1]      = new Field(&m_registers[124], 8, 8,0);
  m_fields[FIFO_FULL_CTR_2]      = new Field(&m_registers[124], 0, 8,0);
  m_fields[FIFO_FULL_CTR_3]      = new Field(&m_registers[125], 8, 8,0);
  m_fields[FIFO_FULL_CTR_4]      = new Field(&m_registers[125], 0, 8,0);
  m_fields[FIFO_FULL_CTR_5]      = new Field(&m_registers[126], 8, 8,0);
  m_fields[FIFO_FULL_CTR_6]      = new Field(&m_registers[126], 0, 8,0);
  m_fields[FIFO_FULL_CTR_7]      = new Field(&m_registers[127], 8, 8,0);
  m_fields[FIFO_FULL_CTR_8]      = new Field(&m_registers[127], 0, 8,0);
  m_fields[AI_PIX_COL]           = new Field(&m_registers[128], 0, 8,0);
  m_fields[AI_PIX_ROW]           = new Field(&m_registers[129], 0, 9,0);
  m_fields[HITOR_CNT_1]          = new Field(&m_registers[130], 0,16,0);
  m_fields[HITOR_CNT_2]          = new Field(&m_registers[131], 0,16,0);
  m_fields[HITOR_CNT_3]          = new Field(&m_registers[132], 0,16,0);
  m_fields[HITOR_CNT_4]          = new Field(&m_registers[133], 0,16,0);
  m_fields[SKP_TRIG_CNT]         = new Field(&m_registers[134], 0,16,0);
  m_fields[ERR_MASK]             = new Field(&m_registers[135], 0,14,0);
  m_fields[ADC_READ]             = new Field(&m_registers[136], 0,16,0);
  m_fields[SELF_TRIG_EN]         = new Field(&m_registers[137], 0,16,0);


  m_names_yarr={
      "AdcRead", "AdcRefTrim", "AdcTrim", "AiPixCol", "AiPixRow",
      "AuroraCbSend", "AuroraCbWaitHigh", "AuroraCbWaitLow", "AuroraCcSend", "AuroraCcWait", "AuroraInitWait",
      "AutoReadA0", "AutoReadA1", "AutoReadA2", "AutoReadA3", "AutoReadB0", "AutoReadB1", "AutoReadB2", "AutoReadB3",
      "BcCounter", "BflipErrCounter", "BflipWarnCounter",
      "CalColprDiff1", "CalColprDiff2", "CalColprDiff3", "CalColprDiff4", "CalColprDiff5",
      "CalColprLin1", "CalColprLin2", "CalColprLin3", "CalColprLin4", "CalColprLin5",
      "CalColprSync1", "CalColprSync2", "CalColprSync3", "CalColprSync4",
      "CdrCpIbias", "CdrEnGck", "CdrPdDel", "CdrPdSel", "CdrSelDelClk", "CdrSelSerClk", "CdrVcoGain", "ChSyncLock",
      "ChSyncPhase", "ChSyncUnlock", "ClkDelay", "ClkDelaySel", "CmdDelay", "CmdErrCounter", "CmlEn", "CmlEnTap",
      "CmlInvTap", "CmlTapBias0", "CmlTapBias1", "CmlTapBias2", "DebugConfig", "DiffComp", "DiffFbCapEn",
      "DiffFol", "DiffLcc", "DiffLccEn", "DiffPrecomp", "DiffPrmp", "DiffVff", "DiffVth1", "DiffVth2",
      "EnCoreColDiff1", "EnCoreColDiff2", "EnCoreColLin1", "EnCoreColLin2", "EnCoreColSync", "ErrMask",
      "FifoFullCounter0", "FifoFullCounter1", "FifoFullCounter2", "FifoFullCounter3", "GlobalPulseRt",
      "GpLvdsRoute",
      "HitOr0MaskDiff0", "HitOr0MaskDiff1", "HitOr0MaskLin0", "HitOr0MaskLin1", "HitOr0MaskSync",
      "HitOr1MaskDiff0", "HitOr1MaskDiff1", "HitOr1MaskLin0", "HitOr1MaskLin1", "HitOr1MaskSync",
      "HitOr2MaskDiff0", "HitOr2MaskDiff1", "HitOr2MaskLin0", "HitOr2MaskLin1", "HitOr2MaskSync",
      "HitOr3MaskDiff0", "HitOr3MaskDiff1", "HitOr3MaskLin0", "HitOr3MaskLin1", "HitOr3MaskSync",
      "HitOrCounter0", "HitOrCounter1", "HitOrCounter2", "HitOrCounter3",
      "InjAnaMode", "InjDelay", "InjEnDig", "InjVcalDiff", "InjVcalHigh", "InjVcalMed",
      "LatencyConfig", "LinComp", "LinFcBias", "LinKrumCurr", "LinLdac", "LinPaInBias", "LinRefKrum", "LinVth",
      "LockLossCounter", "MonFrameSkip", "MonitorEnable", "MonitorImonMux", "MonitorVmonMux",
      "OutPadConfig", "OutputActiveLanes", "OutputDataReadDelay", "OutputFmt", "OutputSerType",
      "PixAutoCol", "PixAutoRow", "PixBroadcastEn", "PixBroadcastMask", "PixDefaultConfig",
      "PixPortal", "PixRegionCol", "PixRegionRow",
      "RingOsc0", "RingOsc1", "RingOsc2", "RingOsc3", "RingOsc4", "RingOsc5", "RingOsc6", "RingOsc7", "RingOscEn",
      "SelfTrigEn",
      "SensorCfg0", "SensorCfg1", "SerSelOut0", "SerSelOut1", "SerSelOut2", "SerSelOut3",
      "SkipTriggerCounter", "SldoAnalogTrim", "SldoDigitalTrim", "SyncAutoZero", "SyncFastTot",
      "SyncIbiasDisc", "SyncIbiasKrum", "SyncIbiasSf", "SyncIbiasp1", "SyncIbiasp2",
      "SyncIctrlSynct", "SyncSelC2F", "SyncSelC4F", "SyncVbl", "SyncVrefKrum", "SyncVth",
      "TrigCounter", "VcoBuffBias", "VcoIbias", "WrSyncDelaySync"
  };

}

Configuration::~Configuration(){
  for(auto it : m_fields){
    delete it.second;
  }
  m_fields.clear();
}

void Configuration::SetVerbose(bool enable){
  m_verbose = enable;
}

uint32_t Configuration::Size(){
  return m_registers.size();
}

uint16_t Configuration::GetRegister(uint32_t index){
  if(index>m_registers.size()-1){return 0;}
  return m_registers[index].GetValue();
}

void Configuration::SetRegister(uint32_t index, uint16_t value){
  if(index>m_registers.size()-1){return;}
  m_registers[index].SetValue(value);
}

void Configuration::SetField(string name, uint16_t value){

  if(name == "InjVcalDiff") return ; // EJS 2020-10-27, let's ignore it for now
  if(name == "MonitorEnable") return ; // EJS 2020-10-27, let's ignore it for now
  if(name == "MonitorImonMux") return ; // EJS 2020-10-27, let's ignore it for now
  if(name == "MonitorVmonMux") return ; // EJS 2020-10-27, let's ignore it for now
  if(name == "SerSelOut0") return ; // EJS 2020-10-27, let's ignore it for now
  if(name == "SerSelOut1") return ; // EJS 2020-10-27, let's ignore it for now
  if(name == "SerSelOut2") return ; // EJS 2020-10-27, let's ignore it for now
  if(name == "SerSelOut3") return ; // EJS 2020-10-27, let's ignore it for now

  //  if(m_verbose) std::cout << __PRETTY_FUNCTION__ << "Setting field: " << name << ", " << value << std::endl;
  auto it=m_name2index.find(name);
  if(it==m_name2index.end()){
    if(m_verbose) std::cout << __PRETTY_FUNCTION__ << " WARNING!!! - Reached end of name2index dictionary" << std::endl;
    return;
  }
  else{
    //    if(m_verbose) std::cout << __PRETTY_FUNCTION__ << " Found in dictionary: " << it -> first << ", " << it -> second << std::endl;
  }
  m_fields[it->second]->SetValue(value);
}

void Configuration::SetField(uint32_t index, uint16_t value){
  m_fields[index]->SetValue(value);
}

Field * Configuration::GetField(uint32_t index){
  return m_fields[index];
}

Field * Configuration::GetField(string name){
  auto it=m_name2index.find(name);
  if(it==m_name2index.end()){return 0;}
  return m_fields[it->second];
}

vector<uint32_t> Configuration::GetUpdatedRegisters(){
  vector<uint32_t> ret;
  for(uint32_t i=0;i<m_registers.size();i++){
    if(m_registers[i].IsUpdated()){ret.push_back(i);m_registers[i].Update(false);}
  }
  return ret;
}

map<string,uint32_t> Configuration::GetRegisters(){
  map<string,uint32_t> ret;
  for(auto name : m_names_yarr){
    if(name == "InjVcalDiff") continue ; // EJS 2020-10-27, let's ignore it for now
    if(name == "MonitorEnable") continue ; // EJS 2020-10-27, let's ignore it for now
    if(name == "MonitorImonMux") continue ; // EJS 2020-10-27, let's ignore it for now
    if(name == "MonitorVmonMux") continue ; // EJS 2020-10-27, let's ignore it for now
    if(name == "SerSelOut0") continue ; // EJS 2020-10-27, let's ignore it for now
    if(name == "SerSelOut1") continue ; // EJS 2020-10-27, let's ignore it for now
    if(name == "SerSelOut2") continue ; // EJS 2020-10-27, let's ignore it for now
    if(name == "SerSelOut3") continue ; // EJS 2020-10-27, let's ignore it for now

    if(m_fields.count(m_name2index[name])==0){
      cout << "Skipping register: " << m_name2index[name] << endl;
      continue;
    }
    ret[name]=m_fields[m_name2index[name]]->GetValue();
  }
  return ret;
}

void Configuration::SetRegisters(map<string,uint32_t> config){
  for(auto it : config){
    //    if(m_verbose) std::cout << __PRETTY_FUNCTION__ << "Setting field: " << it.first << ", " << it.second << std::endl;
    SetField(it.first,it.second);
  }
}
