
/*
 *  ======== ti_easylink_config.h ========
 *  Configured EasyLink module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC1312R1_LAUNCHXL
 *  by the SysConfig tool.
 */

#ifndef EASYLINK_CONFIG_H_
#define EASYLINK_CONFIG_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <ti/drivers/rf/RF.h>
//select your RTOS
//#define TI_RTOS 1
#define RTOS_AL 1

#if defined(TI_RTOS)+defined(RTOS_AL)>1
      !!!COMPILE ERRROR: YOU CAN ONLY HAVE ONE RTOS AT THE SAME TIME
#endif
//#include <ti_devices_config.c> //for CCFG_FORCE_VDDR_HH define
//#include <ti/devices/DeviceFamily.h>
/*
#ifndef CCFG_FORCE_VDDR_HH
// Use default VDDR trim
#define CCFG_FORCE_VDDR_HH  0x01//0x0 disablem,
#endif
*/
//! \brief Defines the address that will accompany each packet sent from this
//! device when EASYLINK_USE_DEFAULT_ADDR is true. It can also be thought of as
//! this device's own address. Organized in little endian format (e.g.an address
//! of 0xABCD should be defined as {0xCD,0xAB})
#define EASYLINK_DEFAULT_ADDR {0xAA}

//! \brief When false, the Easylink stack expects the application to provide an
//! accompanying address for each EasyLink_TxPacket passed to the
//! EasyLink_transmit(), EasyLink_transmitAsync(), and
//! EasyLink_transmitCcaAsync() APIs. Otherwise, these APIs will use the address
//! defined in EASYLINK_DEFAULT_ADDR
#define EASYLINK_USE_DEFAULT_ADDR           true

//! \brief Defines the largest Tx/Rx payload that the interface can support
#define EASYLINK_MAX_DATA_LENGTH            128  // is the biggest value tested.

//! \brief Defines the Tx/Rx Max Address Size
#define EASYLINK_ADDR_SIZE                  1 //8 was the original

//! \brief Enables or disables address filtering
#define EASYLINK_ENABLE_ADDR_FILTERING      false

//! \brief Defines the number of Rx address filters
#define EASYLINK_NUM_ADDR_FILTER          8

//! \brief The table for setting the Rx address filters
#define EASYLINK_ADDR_FILTER_TABLE \
    {\
        0xAA,\
    }

//! \brief Defines the time for the radio to return to idle after inactivity
#define EASYLINK_IDLE_TIMEOUT_MS            1
#define EASYLINK_IDLE_TIMEOUT               EasyLink_ms_To_RadioTime(EASYLINK_IDLE_TIMEOUT_MS)

//! \brief Enables or disables the application to use multiple RF clients
#define EASYLINK_ENABLE_MULTI_CLIENT        true //do not change

//! \brief Defines the relative time from async RX start to timeout. A value of
//! 0 means no timeout
#define EASYLINK_ASYNC_RX_TIMEOUT           EasyLink_ms_To_RadioTime(0)

//! \brief Minimum CCA back-off window in units of
//! EASYLINK_CCA_BACKOFF_TIMEUNITS, as a power of 2
#define EASYLINK_MIN_CCA_BACKOFF_WINDOW     4

//! \brief  Maximum CCA back-off window in units of
//! EASYLINK_CCA_BACKOFF_TIMEUNITS, as a power of 2
#define EASYLINK_MAX_CCA_BACKOFF_WINDOW     8

//! \brief The back-off time units in microseconds
#define EASYLINK_CCA_BACKOFF_TIMEUNITS      250

//! \brief RSSI threshold for Clear Channel Assessment (CCA)
#define EASYLINK_CS_RSSI_THRESHOLD_DBM      -70//70 is ok on this bandwidth

//! \brief Time for which the channel RSSI must remain below the specified
//! threshold for the channel to be considered idle
#define EASYLINK_CHANNEL_IDLE_TIME_US		1000 //original val  5000.
#define EASYLINK_CHANNEL_IDLE_TIME_US       1000 //original val  5000.
//! \brief EasyLink default parameter configuration
#define EASYLINK_PARAM_CONFIG  \
    {\
     .ui32ModType           = EasyLink_Phy_Custom,          \
     .pClientEventCb        = NULL,                         \
     .nClientEventMask      = 0,                            \
     .pGrnFxn               = (EasyLink_GetRandomNumber)rand\
    }

#ifdef __cplusplus
}
#endif

#endif //EASYLINK_CONFIG_H_
