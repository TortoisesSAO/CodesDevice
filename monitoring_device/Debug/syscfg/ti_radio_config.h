/*
 *  ======== ti_radio_config.h ========
 *  Configured RadioConfig module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC1312R1F3RGZ
 *  by the SysConfig tool.
 *
 *  Radio Config module version : 1.14
 *  SmartRF Studio data version : 2.27.0
 */
#ifndef _TI_RADIO_CONFIG_H_
#define _TI_RADIO_CONFIG_H_

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
#include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_prop_cmd.h)
#include <ti/drivers/rf/RF.h>

/* SmartRF Studio version that the RF data is fetched from */
#define SMARTRF_STUDIO_VERSION "2.27.0"

// *********************************************************************************
//   RF Frontend configuration
// *********************************************************************************
// RF design based on: LAUNCHXL-CC1312R1
#define LAUNCHXL_CC1312R1

// RF frontend configuration
#define FRONTEND_SUB1G_DIFF_RF
#define FRONTEND_SUB1G_EXT_BIAS

// Supported frequency bands
#define SUPPORT_FREQBAND_169

// TX power table size definitions
#define TXPOWERTABLE_169_PA10_SIZE 18 // 169 MHz, 10 dBm

// TX power tables
extern RF_TxPowerTable_Entry txPowerTable_169_pa10[]; // 169 MHz, 10 dBm



//*********************************************************************************
//  RF Setting:   Custom (WMBUS N-MODE, 2.4 kbps, 2.4 kHz Deviation, 2-GFSK, 10 kHz RX Bandwidth, RX)
//
//  PHY:          custom169
//  Setting file: setting_tc220_rx_custom.json
//*********************************************************************************

// PA table usage
#define TX_POWER_TABLE_SIZE_custom169_0 TXPOWERTABLE_169_PA13_SIZE

#define txPowerTable_custom169_0 txPowerTable_169_pa13

// TI-RTOS RF Mode object
extern RF_Mode RF_prop_custom169_0;

// RF Core API commands
extern rfc_CMD_PROP_RADIO_DIV_SETUP_t RF_cmdPropRadioDivSetup_custom169_0;
extern rfc_CMD_FS_t RF_cmdFs_custom169_0;
extern rfc_CMD_PROP_RX_t RF_cmdPropRx_custom169_0;

// RF Core API overrides
extern uint32_t pOverrides_custom169_0[];

#endif // _TI_RADIO_CONFIG_H_
