/*
 *  ======== ti_radio_config.h ========
 *  Configured RadioConfig module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC1312R1F3RGZ
 *  by the SysConfig tool.
 *
 *  Radio Config module version : 1.13
 *  SmartRF Studio data version : 2.25.0
 */
#ifndef _TI_RADIO_CONFIG_CUSTOM_H_
#define _TI_RADIO_CONFIG_CUSTOM_H_

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
#include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_prop_cmd.h)
#include <ti/drivers/rf/RF.h>

/* SmartRF Studio version that the RF data is fetched from */
//#define SMARTRF_STUDIO_VERSION "2.25.0" MUST BE COMMENTED
// *********************************************************************************
//   RF Frontend configuration
// *********************************************************************************
// RF design based on: LAUNCHXL-CC1312R1 (CC1352EM-XD7793-XD24)
#define LAUNCHXL_CC1312R1

// RF frontend configuration
#define FRONTEND_SUB1G_DIFF_RF
#define FRONTEND_SUB1G_EXT_BIAS

// Supported frequency bands
#define SUPPORT_FREQBAND_169

// TX power table size definitions
#define TXPOWERTABLE_169_PA13_SIZE 22 // 169 MHz, 13 dBm

// TX power tables
extern RF_TxPowerTable_Entry txPowerTable_169[]; // 169 MHz, 10 dBm

#define txPowerTable txPowerTable_169


//*********************************************************************************
//  RF Setting:   SimpleLink Long Range, 5 kbps (20 ksps), 5 kHz Deviation, 2-GFSK, 34 kHz RX Bandwidth, FEC = 1:2, DSSS = 1:2
//
//  PHY:          slr5kbps2gfsk
//  Setting file: setting_tc480.json
//*********************************************************************************

// PA table usage
//#define RF_PROP_TX_POWER_TABLE_SIZE TXPOWERTABLE_169_PA13_SIZE
#define TX_POWER_TABLE_SIZE TXPOWERTABLE_169_PA13_SIZE


// TI-RTOS RF Mode object
extern RF_Mode RF_prop;
// RF Core API commands
extern rfc_CMD_PROP_RADIO_DIV_SETUP_t RF_cmdPropRadioDivSetup;
extern rfc_CMD_FS_t RF_cmdFs;
extern rfc_CMD_PROP_TX_t RF_cmdPropTx;
extern rfc_CMD_PROP_RX_ADV_t RF_cmdPropRxAdv;
//different specs used for this keep alive move
extern RF_Mode RF_prop_KA;
// RF Core API commands
extern rfc_CMD_PROP_RADIO_DIV_SETUP_t RF_cmdPropRadioDivSetup_KA;
extern rfc_CMD_FS_t RF_cmdFs_KA;
extern rfc_CMD_PROP_TX_t RF_cmdPropTx_KA;
extern rfc_CMD_PROP_RX_ADV_t RF_cmdPropRxAdv_KA;

// RF Core API overrides
extern uint32_t pOverrides[];

#endif // _TI_RADIO_CONFIG_H_
