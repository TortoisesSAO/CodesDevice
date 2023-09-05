
/*
 *  ======== ti_easylink_config.c ========
 *  Configured EasyLink module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC1312R1_LAUNCHXL
 *  by the SysConfig tool.
 */

/***** Includes *****/
#include <easylink_custom/EasyLink.h>
#include <stdint.h>

/* TI Drivers */
#include <ti/drivers/rf/RF.h>

/* Radio Config */
#include <ti_radio_config_custom.h>

EasyLink_RfSetting EasyLink_supportedPhys[] = {
    {   //first physical cape
        .EasyLink_phyType = EasyLink_Phy_Custom,
        .RF_pProp = &RF_prop,
        .RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup = &RF_cmdPropRadioDivSetup,
        .RF_pCmdFs = &RF_cmdFs,
        .RF_pCmdPropTx = &RF_cmdPropTx,
        .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv,
        .RF_pTxPowerTable = txPowerTable,
        .RF_txPowerTableSize = TX_POWER_TABLE_SIZE
    }
};
EasyLink_RfSetting EasyLink_KA_Phys={   //first physical cape
  .EasyLink_phyType = EasyLink_Phy_Custom,
  .RF_pProp = &RF_prop_KA,
  .RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup = &RF_cmdPropRadioDivSetup_KA,
  .RF_pCmdFs = &RF_cmdFs_KA,
  .RF_pCmdPropTx = &RF_cmdPropTx_KA,
  .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv_KA,
  .RF_pTxPowerTable = txPowerTable,
  .RF_txPowerTableSize = TX_POWER_TABLE_SIZE
};
EasyLink_RfSetting EasyLink_TX_DATA_Phys= {   //first physical cape
  .EasyLink_phyType = EasyLink_Phy_Custom,
  .RF_pProp = &RF_prop,
  .RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup = &RF_cmdPropRadioDivSetup,
  .RF_pCmdFs = &RF_cmdFs,
  .RF_pCmdPropTx = &RF_cmdPropTx,
  .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv,
  .RF_pTxPowerTable = txPowerTable,
  .RF_txPowerTableSize = TX_POWER_TABLE_SIZE
};

const uint8_t EasyLink_numSupportedPhys = sizeof(EasyLink_supportedPhys)/sizeof(EasyLink_RfSetting);
