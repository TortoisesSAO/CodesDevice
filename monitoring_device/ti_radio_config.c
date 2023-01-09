/*
 *  ======== ti_radio_config.c ========
 *  Configured RadioConfig module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC1312R1F3RGZ
 *  by the SysConfig tool.
 *
 *  Radio Config module version : 1.13
 *  SmartRF Studio data version : 2.25.0
 */

#include "ti_radio_config.h"
#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_multi_protocol.h)


// *********************************************************************************
//   RF Frontend configuration
// *********************************************************************************
// RF design based on: LAUNCHXL-CC1312R1 (CC1352EM-XD7793-XD24)

// TX Power tables
// The RF_TxPowerTable_DEFAULT_PA_ENTRY and RF_TxPowerTable_HIGH_PA_ENTRY macros are defined in RF.h.
// The following arguments are required:
// RF_TxPowerTable_DEFAULT_PA_ENTRY(bias, gain, boost, coefficient)
// RF_TxPowerTable_HIGH_PA_ENTRY(bias, ibboost, boost, coefficient, ldoTrim)
// See the Technical Reference Manual for further details about the "txPower" Command field.
// The PA settings require the CCFG_FORCE_VDDR_HH = 0 unless stated otherwise.

// 868 MHz, 13 dBm
// 169 MHz, 10 dBm
RF_TxPowerTable_Entry txPowerTable_169[TX_POWER_TABLE_SIZE] =
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

//txPowerTable=txPowerTable_866;


//*********************************************************************************
//  RF Setting:   SimpleLink Long Range, 5 kbps (20 ksps), 5 kHz Deviation, 2-GFSK, 34 kHz RX Bandwidth, FEC = 1:2, DSSS = 1:2
//
//  PHY:          slr5kbps2gfsk
//  Setting file: setting_tc480.json
//*********************************************************************************

// Parameter summary
// RX Address0: 0x00
// RX Address1: 0x00
// RX Address Mode: No address check
// Frequency: 169.45001 MHz
// Data Format: Serial mode disable
// Deviation: 0.000 kHz
// Fixed Packet Length: 5
// Packet Length Config: Fixed
// Max Packet Length: 64
// Packet Data: 255
// Preamble Count: 1 Bit
// Preamble Mode: Send 1 as the first preamble bit
// RX Filter BW: 8.5 kHz
// Symbol Rate: 4.29998 kBaud
// Sync Word: 0x0000f68d
// Sync Word Length: 16 Bits
// TX Power: 10 dBm
// Whitening: No whitening

// TI-RTOS RF Mode Object
RF_Mode RF_prop =
{
    .rfMode = RF_MODE_AUTO,
    .cpePatchFxn = &rf_patch_cpe_multi_protocol,
    .mcePatchFxn = 0,
    .rfePatchFxn = 0
};
// Overrides for CMD_PROP_RADIO_DIV_SETUP

// Overrides for CMD_PROP_RADIO_DIV_SETUP
uint32_t pOverrides[] =
{
    // override_prop_common.xml
    // DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0x7 (DITHER_EN=0 and IPEAK=7).
    (uint32_t)0x00F788D3,
    // override_prop_common_sub1g.xml
    // Set RF_FSCA.ANADIV.DIV_SEL_BIAS = 1. Bits [0:16, 24, 30] are don't care..
    (uint32_t)0x4001405D,
    // Set RF_FSCA.ANADIV.DIV_SEL_BIAS = 1. Bits [0:16, 24, 30] are don't care..
    (uint32_t)0x08141131,
    // override_tc220_tc221_ch1-5_tx.xml
    // Synth: Set loop bandwidth after lock to 75 kHz with damping = 1.1 (K2)
    (uint32_t)0x777C0583,
    // Synth: Set loop bandwidth after lock to 75 kHz with damping = 1.1 (K2)
    (uint32_t)0x000005A3,
    // Synth: Set loop bandwidth after lock to 75 kHz (K3, LSB)
    (uint32_t)0xCC320603,
    // Synth: Set loop bandwidth after lock to 75 kHz (K3, MSB)
    (uint32_t)0x00010623,
    // Synth: Set FREF = 6.857 (48 MHz/7) MHz
    (uint32_t)0x000784A3,
    // Tx: Configure PA ramp time, PACTL2.RC=0x1 (in ADI0, set PACTL2[4:3]=0x1)
    ADI_2HALFREG_OVERRIDE(0,16,0x8,0x8,17,0x1,0x1),
    // Tx: Set wait time before turning off ramp to 0x1A (default: 0x1F)
    HW_REG_OVERRIDE(0x6028,0x001A),
    // Tx: Set repetition factor = 2
    HW_REG_OVERRIDE(0x5324,0x0002),
    (uint32_t)0xFFFFFFFF
};



// CMD_PROP_RADIO_DIV_SETUP
// Proprietary Mode Radio Setup Command for All Frequency Bands
rfc_CMD_PROP_RADIO_DIV_SETUP_t RF_cmdPropRadioDivSetup =
{
    .commandNo = 0x3807,
    .status = 0x0000,
    .pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .modulation.modType = 0x5,
    .modulation.deviation = 0x0,
    .modulation.deviationStepSz = 0x2,
    .symbolRate.preScale = 0xFC,
    .symbolRate.rateWord = 0x171DE,
    .symbolRate.decimMode = 0x0,
    .rxBw = 0x44,
    .preamConf.nPreamBytes = 0x0,
    .preamConf.preamMode = 0x1,
    .formatConf.nSwBits = 0x10,
    .formatConf.bBitReversal = 0x0,
    .formatConf.bMsbFirst = 0x1,
    .formatConf.fecMode = 0x2,
    .formatConf.whitenMode = 0x0,
    .config.frontEndMode = 0x0,
    .config.biasMode = 0x1,
    .config.analogCfgMode = 0x0,
    .config.bNoFsPowerUp = 0x0,
    .config.bSynthNarrowBand = 0x1,
    .txPower = 0x003F,
    .pRegOverride = pOverrides,
    .centerFreq = 0x00A9,
    .intFreq = 0x8000,
    .loDivider = 0x1E
};


// CMD_FS
// Frequency Synthesizer Programming Command
rfc_CMD_FS_t RF_cmdFs =
{
    .commandNo = 0x0803,
    .status = 0x0000,
    .pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
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



// CMD_PROP_TX
// Proprietary Mode Transmit Command
rfc_CMD_PROP_TX_t RF_cmdPropTx =
{
    .commandNo = 0x3801,
    .status = 0x0000,
    .pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .pktConf.bFsOff = 0x0,
    .pktConf.bUseCrc = 0x1,
    .pktConf.bVarLen = 0x0,
    .pktLen = 0x07,
    .syncWord = 0x0000F68D,
    .pPkt = 0 // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
};
// CMD_PROP_RX_ADV
// Proprietary Mode Advanced Receive Command
rfc_CMD_PROP_RX_ADV_t RF_cmdPropRxAdv =
{
    .commandNo = 0x3804,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x0,
    .condition.nSkip = 0x0,
    .pktConf.bFsOff = 0x0,
    .pktConf.bRepeatOk = 0x0,
    .pktConf.bRepeatNok = 0x0,
    .pktConf.bUseCrc = 0x0,
    .pktConf.bCrcIncSw = 0x0,
    .pktConf.bCrcIncHdr = 0x0,
    .pktConf.endType = 0x0,
    .pktConf.filterOp = 0x0,
    .rxConf.bAutoFlushIgnored = 0x0,
    .rxConf.bAutoFlushCrcErr = 0x0,
    .rxConf.bIncludeHdr = 0x0,
    .rxConf.bIncludeCrc = 0x0,
    .rxConf.bAppendRssi = 0x0,
    .rxConf.bAppendTimestamp = 0x0,
    .rxConf.bAppendStatus = 0x0,
    .syncWord0 = 0x00000000,
    .syncWord1 = 0x00000000,
    .maxPktLen = 0x00FF,
    .hdrConf.numHdrBits = 0x0,
    .hdrConf.lenPos = 0x0,
    .hdrConf.numLenBits = 0x0,
    .addrConf.addrType = 0x0,
    .addrConf.addrSize = 0x0,
    .addrConf.addrPos = 0x0,
    .addrConf.numAddr = 0x0,
    .lenOffset = 0x00,
    .endTrigger.triggerType = 0x0,
    .endTrigger.bEnaCmd = 0x0,
    .endTrigger.triggerNo = 0x0,
    .endTrigger.pastTrig = 0x0,
    .endTime = 0x00000000,
    .pAddr = 0,
    .pQueue = 0,
    .pOutput = 0
};


