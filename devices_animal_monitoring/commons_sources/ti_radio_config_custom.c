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

#include "ti_radio_config_custom.h"

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_multi_protocol.h)
#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_prop.h)
#include DeviceFamily_constructPath(rf_patches/rf_patch_rfe_genook.h)
#include DeviceFamily_constructPath(rf_patches/rf_patch_mce_genook.h)
// *********************************************************************************
//   RF Frontend configuration
// *********************************************************************************
// RF design based on: CC1312R1
// TX Power tables
// The RF_TxPowerTable_DEFAULT_PA_ENTRY and RF_TxPowerTable_HIGH_PA_ENTRY macros are defined in RF.h.
// The following arguments are required:
// See the Technical Reference Manual for further details about the "txPower" Command field.
// The PA settings require the CCFG_FORCE_VDDR_HH = 0 unless stated otherwise.
// This power has been testeted for the 143-176 MHz band (AKA 169 MHz), 13/14 dBm. DO NOT CHANGE
RF_TxPowerTable_Entry txPowerTable_169[TX_POWER_TABLE_SIZE] =
{
	    {-20, RF_TxPowerTable_DEFAULT_PA_ENTRY(0, 3, 0, 2) }, // 0x04C0
	    {-15, RF_TxPowerTable_DEFAULT_PA_ENTRY(1, 3, 0, 3) }, // 0x06C1
	    {-10, RF_TxPowerTable_DEFAULT_PA_ENTRY(2, 3, 0, 5) }, // 0x0AC2
	    {-7, RF_TxPowerTable_DEFAULT_PA_ENTRY(3, 3, 0, 5) }, // 0x0AC3
	    {-5, RF_TxPowerTable_DEFAULT_PA_ENTRY(4, 3, 0, 5) }, // 0x0AC4
	    {-3, RF_TxPowerTable_DEFAULT_PA_ENTRY(5, 3, 0, 6) }, // 0x0CC5
	    {0, RF_TxPowerTable_DEFAULT_PA_ENTRY(8, 3, 0, 8) }, // 0x10C8
	    {1, RF_TxPowerTable_DEFAULT_PA_ENTRY(9, 3, 0, 9) }, // 0x12C9
	    {2, RF_TxPowerTable_DEFAULT_PA_ENTRY(10, 3, 0, 9) }, // 0x12CA
	    {3, RF_TxPowerTable_DEFAULT_PA_ENTRY(11, 3, 0, 10) }, // 0x14CB
	    {4, RF_TxPowerTable_DEFAULT_PA_ENTRY(13, 3, 0, 11) }, // 0x16CD
	    {5, RF_TxPowerTable_DEFAULT_PA_ENTRY(14, 3, 0, 14) }, // 0x1CCE
	    {6, RF_TxPowerTable_DEFAULT_PA_ENTRY(17, 3, 0, 16) }, // 0x20D1
	    {7, RF_TxPowerTable_DEFAULT_PA_ENTRY(20, 3, 0, 19) }, // 0x26D4
	    {8, RF_TxPowerTable_DEFAULT_PA_ENTRY(24, 3, 0, 22) }, // 0x2CD8
	    {9, RF_TxPowerTable_DEFAULT_PA_ENTRY(28, 3, 0, 31) }, // 0x3EDC
	    {10, RF_TxPowerTable_DEFAULT_PA_ENTRY(18, 2, 0, 31) }, // 0x3E92
	    {11, RF_TxPowerTable_DEFAULT_PA_ENTRY(26, 2, 0, 51) }, // 0x669A
	    {12, RF_TxPowerTable_DEFAULT_PA_ENTRY(16, 0, 0, 82) }, // 0xA410
        // The original PA value (12.5 dBm) has been rounded to an integer value.
        {13, RF_TxPowerTable_DEFAULT_PA_ENTRY(36, 0, 0, 89) }, // 0xB224
        // This setting requires CCFG_FORCE_VDDR_HH = 1 to properly work. This is set inside yoour poryect .syscfg file
	    {14, RF_TxPowerTable_DEFAULT_PA_ENTRY(63, 0, 1, 0) }, // 0x013F// This setting requires CCFG_FORCE_VDDR_HH = 1.
		RF_TxPowerTable_TERMINATION_ENTRY
};

//*********************************************************************************
//  RF Setting:   SimpleLink Long Range, 5 kbps (20 ksps), 5 kHz Deviation, 2-GFSK, 34 kHz RX Bandwidth, FEC = 1:2, DSSS = 1:2
//
//  PHY:          slr5kbps2gfsk
//  Setting file: setting_tc480.json
//*********************************************************************************

// TI-RTOS RF Mode Object
RF_Mode RF_prop =
{
    .rfMode = RF_MODE_AUTO,
    .cpePatchFxn = &rf_patch_cpe_multi_protocol,
    .mcePatchFxn = 0,
    .rfePatchFxn = 0
};
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
    // override_tc221_rx.xml
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
    // Rx: Set AGC reference level to 0x1B (default: 0x2E)
    HW_REG_OVERRIDE(0x609C,0x001B),
    // Rx: Set AGC max gain to -6 dB
    HW_REG_OVERRIDE(0x6098,0x34D1),
    (uint32_t)0xFFFFFFFF
};
// CMD_PROP_RADIO_DIV_SETUP
rfc_CMD_PROP_RADIO_DIV_SETUP_t RF_cmdPropRadioDivSetup =
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
    .modulation.deviation = 0x14,
    .modulation.deviationStepSz = 0x0,
    .symbolRate.preScale = 0xF,
    .symbolRate.rateWord = 0x3333,
    .symbolRate.decimMode = 0x0,
    .rxBw = 0x4C,//0x4c 39 Khz minimum
    .preamConf.nPreamBytes = 0x2,
    .preamConf.preamMode = 0x0,
    .formatConf.nSwBits = 0x20,
    .formatConf.bBitReversal = 0x0,
    .formatConf.bMsbFirst = 0x0,
    .formatConf.fecMode = 0x8,
    .formatConf.whitenMode = 0x1,
    .config.frontEndMode = 0x0,
    .config.biasMode = 0x1,
    .config.analogCfgMode = 0x0,
    .config.bNoFsPowerUp = 0x0,
    .config.bSynthNarrowBand = 0x0,
    .txPower = 0x003F,
    .pRegOverride = pOverrides,
    .centerFreq = 0x0097,
    .intFreq = 0x8000,
    .loDivider = 0x1E
};

// CMD_FS
// Frequency Synthesizer Programming Command
rfc_CMD_FS_t RF_cmdFs =
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
    .frequency = 0x0097,
    .fractFreq = 0x7334,
    .synthConf.bTxMode = 0x0,
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
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .pktConf.bFsOff = 0x0,
    .pktConf.bUseCrc = 0x1,
    .pktConf.bVarLen = 0x1,
    .pktLen = 0x40,//0x14 original
    .syncWord = 0x930B51DE,
    .pPkt = 0
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
    .syncWord0 = 0x930B51DE,
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
//*********************************************************************************
//*********************************************************************************
//*********************************************************************************
//*********************************************************************************
// KA PULSE PARAMETERS
//**TESTED 03/03/2023, WORK ON SMART RF STUDIO, DO NOT CHANGE RX BANDWIDTH BELOW 34K or packet error rate will increase drastically*//
//*tested: 19.4 k is the minimum bandbidth witch allows reception but with crc errror *//
//*this app has a fixed message size of 1 byte and no CRC correction. If you wish to add CRC correction, please, change setup*//
//*********************************************************************************
// PARAMETER SUMMARY
// RX Address0: 0xAA
// RX Address1: 0xBB
// RX Address Mode: No address check
// Frequency (MHz): 433.0000
// Deviation (kHz): 0.0
// Fixed Packet Length: 20
// Packet Length Config: Variable
// Max Packet Length: 255
// Preamble Count: 4 Bytes
// Preamble Mode: Send 0 as the first preamble bit
// RX Filter BW (kHz): 34.1
// Symbol Rate (kBaud): 4.800
// Sync Word: 0x930B51DE
// Sync Word Length: 32 Bits
// TX Power (dBm): 12
// Whitening: No whitening

RF_Mode RF_prop_KA =
{
    .rfMode = RF_MODE_AUTO,
    .cpePatchFxn = &rf_patch_cpe_multi_protocol,
    .mcePatchFxn =&rf_patch_mce_genook,//&rf_patch_mce_genook, //&rf_patch_mce_genook,
    .rfePatchFxn =&rf_patch_mce_genook,//&rf_patch_rfe_genook, //&rf_patch_rfe_genook
};
// Overrides for CMD_PROP_RADIO_DIV_SETUP
 uint32_t pOverrides_KA[] =
{
 // override_tc599.json
 // PHY: Use MCE RAM patch (mode 0), RFE RAM patch (mode 0)
 MCE_RFE_OVERRIDE(1,0,0,1,0,0),
 // Tx: Configure PA ramp time, PACTL2.RC=0x3 (in ADI0, set PACTL2[4:3]=0x3)
 ADI_2HALFREG_OVERRIDE(0,16,0x8,0x8,17,0x1,0x1),
 // Rx: Set AGC reference level to 0x1E (default: 0x2E)
 HW_REG_OVERRIDE(0x609C,0x001E),
 // Rx: Set RSSI offset to adjust reported RSSI by 0 dB (default: -2), trimmed for external bias and differential configuration
 (uint32_t)0x000088A3,
 // Rx: Set anti-aliasing filter bandwidth to 0xD (in ADI0, set IFAMPCTL3[7:4]=0xD)
 ADI_HALFREG_OVERRIDE(0,61,0xF,0xF),
 // TX: Set FSCA divider bias to 1
 HW32_ARRAY_OVERRIDE(0x405C,1),
 // TX: Set FSCA divider bias to 1
 (uint32_t)0x08141131,
 // OOK: Set duty cycle to compensate for PA ramping
 HW_REG_OVERRIDE(0x51E4,0x80AF),
 // Set code length k=7 in viterbi
 HW_REG_OVERRIDE(0x5270,0x0002),
 // Tx: Configure PA ramping, set wait time before turning off (0x1A ticks of 16/24 us = 17.3 us).
 HW_REG_OVERRIDE(0x6028,0x001A),
 // override_prop_common_sub1g.json
 // Set RF_FSCA.ANADIV.DIV_SEL_BIAS = 1. Bits [0:16, 24, 30] are don't care..
 (uint32_t)0x4001405D,
 // Set RF_FSCA.ANADIV.DIV_SEL_BIAS = 1. Bits [0:16, 24, 30] are don't care..
 (uint32_t)0x08141131,
 // override_prop_common.json
 // DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0x7 (DITHER_EN=0 and IPEAK=7).
 (uint32_t)0x00F788D3,
 (uint32_t)0xFFFFFFFF
};


 // CMD_PROP_RADIO_DIV_SETUP
 rfc_CMD_PROP_RADIO_DIV_SETUP_t RF_cmdPropRadioDivSetup_KA =
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
     .modulation.deviation = 0x14,
     .modulation.deviationStepSz = 0x0,
     .symbolRate.preScale = 0xF,
     .symbolRate.rateWord = 0x3333,
     .symbolRate.decimMode = 0x0,
     .rxBw = 0x4C,//0x4c 39 Khz minimum
     .preamConf.nPreamBytes = 0x2,
     .preamConf.preamMode = 0x0,
     .formatConf.nSwBits = 0x20,
     .formatConf.bBitReversal = 0x0,
     .formatConf.bMsbFirst = 0x0,
     .formatConf.fecMode = 0x8,
     .formatConf.whitenMode = 0x1,
     .config.frontEndMode = 0x0,
     .config.biasMode = 0x1,
     .config.analogCfgMode = 0x0,
     .config.bNoFsPowerUp = 0x0,
     .config.bSynthNarrowBand = 0x0,
     .txPower = 0x003F,
     .pRegOverride = pOverrides,
     .centerFreq = 0x0097,
     .intFreq = 0x8000,
     .loDivider = 0x1E
 };

 // CMD_FS
 // Frequency Synthesizer Programming Command
 rfc_CMD_FS_t RF_cmdFs_KA =
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
     .frequency = 0x0097,
     .fractFreq = 0x7334,
     .synthConf.bTxMode = 0x0,
     .synthConf.refFreq = 0x0,
     .__dummy0 = 0x00,
     .__dummy1 = 0x00,
     .__dummy2 = 0x00,
     .__dummy3 = 0x0000
 };
// CMD_PROP_TX
// Proprietary Mode Transmit Command
 rfc_CMD_PROP_TX_t RF_cmdPropTx_KA =
{
 .commandNo = 0x3801,
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
 .pktConf.bUseCrc = 0x1,
 .pktConf.bVarLen = 0x1,
 .pktLen = 0x64,
 .syncWord = 0x930B51DE,
 .pPkt = 0
};
 rfc_CMD_PROP_RX_ADV_t RF_cmdPropRxAdv_KA =
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
 .syncWord0 = 0x930B51DE,
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
//define the easylink table to make it available
