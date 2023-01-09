/*
 *  ======== ti_radio_config.c ========
 *  Configured RadioConfig module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC1312R1F3RGZ
 *  by the SysConfig tool.
 *
 *  Radio Config module version : 1.14
 *  SmartRF Studio data version : 2.27.0
 */

#include "ti_radio_config.h"
#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_multi_protocol.h)


// *********************************************************************************
//   RF Frontend configuration
// *********************************************************************************
// RF design based on: LAUNCHXL-CC1312R1

// TX Power tables
// The RF_TxPowerTable_DEFAULT_PA_ENTRY and RF_TxPowerTable_HIGH_PA_ENTRY macros are defined in RF.h.
// The following arguments are required:
// RF_TxPowerTable_DEFAULT_PA_ENTRY(bias, gain, boost, coefficient)
// RF_TxPowerTable_HIGH_PA_ENTRY(bias, ibboost, boost, coefficient, ldoTrim)
// See the Technical Reference Manual for further details about the "txPower" Command field.
// The PA settings require the CCFG_FORCE_VDDR_HH = 0 unless stated otherwise.

// 169 MHz, 10 dBm
RF_TxPowerTable_Entry txPowerTable_169_pa10[TXPOWERTABLE_169_PA10_SIZE] =
{
    {-13, RF_TxPowerTable_DEFAULT_PA_ENTRY(6, 3, 0, 4) }, // 0x08C6
    {-11, RF_TxPowerTable_DEFAULT_PA_ENTRY(7, 3, 0, 4) }, // 0x08C7
    {-7, RF_TxPowerTable_DEFAULT_PA_ENTRY(10, 3, 0, 4) }, // 0x08CA
    {-3, RF_TxPowerTable_DEFAULT_PA_ENTRY(8, 2, 0, 7) }, // 0x0E88
    {-2, RF_TxPowerTable_DEFAULT_PA_ENTRY(9, 2, 0, 7) }, // 0x0E89
    {-1, RF_TxPowerTable_DEFAULT_PA_ENTRY(10, 2, 0, 7) }, // 0x0E8A
    {0, RF_TxPowerTable_DEFAULT_PA_ENTRY(20, 3, 0, 7) }, // 0x0ED4
    {1, RF_TxPowerTable_DEFAULT_PA_ENTRY(22, 3, 0, 7) }, // 0x0ED6
    {2, RF_TxPowerTable_DEFAULT_PA_ENTRY(24, 3, 0, 7) }, // 0x0ED8
    {3, RF_TxPowerTable_DEFAULT_PA_ENTRY(27, 3, 0, 7) }, // 0x0EDB
    {4, RF_TxPowerTable_DEFAULT_PA_ENTRY(30, 3, 0, 7) }, // 0x0EDE
    {5, RF_TxPowerTable_DEFAULT_PA_ENTRY(20, 2, 0, 12) }, // 0x1894
    {6, RF_TxPowerTable_DEFAULT_PA_ENTRY(18, 1, 0, 12) }, // 0x1852
    {7, RF_TxPowerTable_DEFAULT_PA_ENTRY(12, 0, 0, 12) }, // 0x180C
    {8, RF_TxPowerTable_DEFAULT_PA_ENTRY(27, 1, 0, 19) }, // 0x265B
    {9, RF_TxPowerTable_DEFAULT_PA_ENTRY(25, 0, 0, 28) }, // 0x3819
    {10, RF_TxPowerTable_DEFAULT_PA_ENTRY(63, 0, 0, 0) }, // 0x003F
    RF_TxPowerTable_TERMINATION_ENTRY
};



//*********************************************************************************
//  RF Setting:   Custom (WMBUS N-MODE, 2.4 kbps, 2.4 kHz Deviation, 2-GFSK, 10 kHz RX Bandwidth, RX)
//
//  PHY:          custom169
//  Setting file: setting_tc220_rx_custom.json
//*********************************************************************************

// PARAMETER SUMMARY
// RX Address Mode: No address check
// Frequency (MHz): 169.4500
// Deviation (kHz): 2.4
// Packet Length Config: Fixed
// Max Packet Length: 64
// Preamble Count: 4 Bytes
// Preamble Mode: Send 0 as the first preamble bit
// RX Filter BW (kHz): 8.5
// Symbol Rate (kBaud): 2.400
// Sync Word: 0xF68D
// Sync Word Length: 16 Bits
// TX Power (dBm): 10
// Whitening: No whitening

// TI-RTOS RF Mode Object
RF_Mode RF_prop_custom169_0 =
{
    .rfMode = RF_MODE_AUTO,
    .cpePatchFxn = &rf_patch_cpe_multi_protocol,
    .mcePatchFxn = 0,
    .rfePatchFxn = 0
};

// Overrides for CMD_PROP_RADIO_DIV_SETUP
uint32_t pOverrides_custom169_0[] =
{
    // override_prop_common.json
    // DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0x7 (DITHER_EN=0 and IPEAK=7).
    (uint32_t)0x00F788D3,
    // override_prop_common_sub1g.json
    // Set RF_FSCA.ANADIV.DIV_SEL_BIAS = 1. Bits [0:16, 24, 30] are don't care..
    (uint32_t)0x4001405D,
    // Set RF_FSCA.ANADIV.DIV_SEL_BIAS = 1. Bits [0:16, 24, 30] are don't care..
    (uint32_t)0x08141131,
    // override_tc220_rx.json
    // Set filter overrides
    (uint32_t)0x40024029,
    // IIR EN, 2nd order, IIR_FILT_BW=1
    (uint32_t)0x38000000,
    // Set IIR clk div
    (uint32_t)0x01608402,
    // Synth: Set loop bandwidth after lock to 150 kHz (K2)
    (uint32_t)0x424C0583,
    // Synth: Set loop bandwidth after lock to 150 kHz (K2)
    (uint32_t)0x000205A3,
    // Synth: Set loop bandwidth after lock to 150 kHz (K3, LSB)
    (uint32_t)0x98630603,
    // Synth: Set loop bandwidth after lock to 150 kHz (K3, MSB)
    (uint32_t)0x00030623,
    // Synth: Set FREF = 8 (48 MHz/6) MHz
    (uint32_t)0x000684A3,
    // Rx: Set anti-aliasing filter bandwidth to 0xD (in ADI0, set IFAMPCTL3[7:4]=0xD)
    ADI_HALFREG_OVERRIDE(0,61,0xF,0xD),
    // Rx: Set RSSI offset to adjust reported RSSI by -2 dB (default: -2), trimmed for external bias and differential configuration
    (uint32_t)0x000E88A3,
    // Rx: Set AGC reference level to 0x19 (default: 0x2E)
    HW_REG_OVERRIDE(0x609C,0x0019),
    // Rx: Set AGC max gain to -6 dB
    HW_REG_OVERRIDE(0x6098,0x34D1),
    (uint32_t)0xFFFFFFFF
};



// CMD_PROP_RADIO_DIV_SETUP
// Proprietary Mode Radio Setup Command for All Frequency Bands
rfc_CMD_PROP_RADIO_DIV_SETUP_t RF_cmdPropRadioDivSetup_custom169_0 =
{
    .commandNo = 0x3807,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .modulation.modType = 0x1,
    .modulation.deviation = 0x99,
    .modulation.deviationStepSz = 0x2,
    .symbolRate.preScale = 0xFC,
    .symbolRate.rateWord = 0x6738,
    .symbolRate.decimMode = 0x0,
    .rxBw = 0x44,
    .preamConf.nPreamBytes = 0x4,
    .preamConf.preamMode = 0x0,
    .formatConf.nSwBits = 0x10,
    .formatConf.bBitReversal = 0x0,
    .formatConf.bMsbFirst = 0x1,
    .formatConf.fecMode = 0x0,
    .formatConf.whitenMode = 0x0,
    .config.frontEndMode = 0x0,
    .config.biasMode = 0x1,
    .config.analogCfgMode = 0x0,
    .config.bNoFsPowerUp = 0x0,
    .config.bSynthNarrowBand = 0x1,
    .txPower = 0x003F,
    .pRegOverride = pOverrides_custom169_0,
    .centerFreq = 0x00A9,
    .intFreq = 0x8000,
    .loDivider = 0x1E
};

// CMD_FS
// Frequency Synthesizer Programming Command
rfc_CMD_FS_t RF_cmdFs_custom169_0 =
{
    .commandNo = 0x0803,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .frequency = 0x00A9,
    .fractFreq = 0x7334,
    .synthConf.bTxMode = 0x1,
    .synthConf.refFreq = 0x0,
    .__dummy0 = 0x00,
    .__dummy1 = 0x00,
    .__dummy2 = 0x00,
    .__dummy3 = 0x0000
};

// CMD_PROP_RX
// Proprietary Mode Receive Command
rfc_CMD_PROP_RX_t RF_cmdPropRx_custom169_0 =
{
    .commandNo = 0x3802,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .pktConf.bFsOff = 0x0,
    .pktConf.bRepeatOk = 0x0,
    .pktConf.bRepeatNok = 0x0,
    .pktConf.bUseCrc = 0x1,
    .pktConf.bVarLen = 0x0,
    .pktConf.bChkAddress = 0x0,
    .pktConf.endType = 0x0,
    .pktConf.filterOp = 0x0,
    .rxConf.bAutoFlushIgnored = 0x0,
    .rxConf.bAutoFlushCrcErr = 0x0,
    .rxConf.bIncludeHdr = 0x0,
    .rxConf.bIncludeCrc = 0x0,
    .rxConf.bAppendRssi = 0x0,
    .rxConf.bAppendTimestamp = 0x0,
    .rxConf.bAppendStatus = 0x0,
    .syncWord = 0x0000F68D,
    .maxPktLen = 0x40,
    .address0 = 0x00,
    .address1 = 0x00,
    .endTrigger.triggerType = 0x1,
    .endTrigger.bEnaCmd = 0x0,
    .endTrigger.triggerNo = 0x0,
    .endTrigger.pastTrig = 0x0,
    .endTime = 0x00000000,
    .pQueue = 0,
    .pOutput = 0
};


