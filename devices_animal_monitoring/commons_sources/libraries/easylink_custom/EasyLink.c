/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/***** Includes *****/
#include "EasyLink.h"
#include "ti_easylink_config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#ifndef USE_DMM
#include <ti/drivers/rf/RF.h>
#else
#include <dmm/dmm_rfmap.h>
#endif //USE_DMM

/* TI Drivers */
#include <ti_drivers_config.h>
#include "ti_radio_config.h"

#ifdef TI_RTOS //original version of the library
/* BIOS Header files */
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
/* XDCtools Header files */
#include <xdc/runtime/Error.h>

#endif
#ifdef RTOS_AL //original version of the library
#include <RTOS_AL/rtos_AL.h>
#include <RTOS_AL/semaphore_AL.h> //allow the use of an operative system
#endif
//-----------------------------------------------
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_data_entry.h)
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)
#include DeviceFamily_constructPath(driverlib/rf_prop_cmd.h)
#include DeviceFamily_constructPath(driverlib/chipinfo.h)
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_fcfg1.h)
#include DeviceFamily_constructPath(inc/hw_ccfg.h)
#include DeviceFamily_constructPath(inc/hw_ccfg_simple_struct.h)
/*
union setupCmd_t{
#if ((defined LAUNCHXL_CC1352P1) || (defined LAUNCHXL_CC1352P_2) || \
     (defined LAUNCHXL_CC1352P_4)|| (defined LP_CC1352P7_1)      || \
	 (defined LP_CC1352P7_4)     || (defined CONFIG_LP_CC2652PSIP) || \
     (defined CONFIG_CC2652P1FSIP))
    rfc_CMD_PROP_RADIO_DIV_SETUP_PA_t divSetup;
#else
    rfc_CMD_PROP_RADIO_DIV_SETUP_t divSetup;
#endif
    rfc_CMD_PROP_RADIO_SETUP_t setup;
};

*/
// Enable Activity profiling
// #define EASYLINK_ACTIVITY_PROFILING
//Primary IEEE address location
#define EASYLINK_PRIMARY_IEEE_ADDR_LOCATION   (FCFG1_BASE + FCFG1_O_MAC_15_4_0)
//Secondary IEEE address location
#define EASYLINK_SECONDARY_IEEE_ADDR_LOCATION (CCFG_BASE  + CCFG_O_IEEE_MAC_0)

#define EASYLINK_RF_EVENT_MASK  ( RF_EventLastCmdDone | \
             RF_EventCmdAborted | RF_EventCmdStopped | RF_EventCmdCancelled | \
             RF_EventCmdPreempted )

#define EASYLINK_RF_CMD_HANDLE_INVALID -1

#define EasyLink_CmdHandle_isValid(handle) (handle >= 0)

/* EasyLink Proprietary Header Configuration */
#define EASYLINK_PROP_TRX_SYNC_WORD     0x930B51DE
#define EASYLINK_PROP_HDR_NBITS         8U
#define EASYLINK_PROP_LEN_OFFSET        0U

/* IEEE 802.15.4g Header Configuration
 * _S indicates the shift for a given bit field
 * _M indicates the mask required to isolate a given bit field
 */
#define EASYLINK_IEEE_TRX_SYNC_WORD     0x0055904E
#define EASYLINK_IEEE_HDR_NBITS         16U
#define EASYLINK_IEEE_LEN_OFFSET        0xFC

#define EASYLINK_IEEE_HDR_LEN_S         0U
#define EASYLINK_IEEE_HDR_LEN_M         0x00FFU
#define EASYLINK_IEEE_HDR_CRC_S         12U
#define EASYLINK_IEEE_HDR_CRC_M         0x1000U
#define EASYLINK_IEEE_HDR_WHTNG_S       11U
#define EASYLINK_IEEE_HDR_WHTNG_M       0x0800U
#define EASYLINK_IEEE_HDR_CRC_2BYTE     1U
#define EASYLINK_IEEE_HDR_CRC_4BYTE     0U
#define EASYLINK_IEEE_HDR_WHTNG_EN      1U
#define EASYLINK_IEEE_HDR_WHTNG_DIS     0U

#define EASYLINK_IEEE_HDR_CREATE(crc, whitening, length) {                         \
    ((crc << EASYLINK_IEEE_HDR_CRC_S) | (whitening << EASYLINK_IEEE_HDR_WHTNG_S) | \
    ((length << EASYLINK_IEEE_HDR_LEN_S) & EASYLINK_IEEE_HDR_LEN_M))               \
}

static uint8_t rfParamsnID_counter=0;//to keep track of protocol activated
/* Common Configuration (Prop and IEEE) */
#define EASYLINK_HDR_LEN_NBITS          8U// 8U

/* Return the size of the header rounded up to
 * the nearest integer number of bytes
 */
#define EASYLINK_HDR_SIZE_NBYTES(x) (((x) >> 3U) + (((x) & 0x03) ? 1U : 0U))
/* 2-GFSK 5 Kbps baud rate in kbps */
#define BAUD_RATE_2GFSK_5K     (5U)
/* 2-GFSK 50 Kbps baud rate in kbps */
#define BAUD_RATE_2GFSK_50K     (50U)
/* 2-GFSK 200 Kbps baud rate in kbps */
#define BAUD_RATE_2GFSK_200K    (200U)
/* SLR chip rate in kcps */
#define BAUD_RATE_SLR_5K        (20U)
/* Payload length in terms of chips */
#define PKT_NCHIPS_SLR_5K       (32U)
/* Packet overhead in terms of chips */
#define PKT_OVHD_SLR_5K         (480U)
/* Packet overhead in terms of bits */
#define PKT_OVHD_2GFSK_50K      (10U)
/* Packet overhead in terms of bits */
#define PKT_OVHD_2GFSK_200K     (10U)

/***** Prototypes *****/
static EasyLink_TxDoneCb txCb;
static EasyLink_ReceiveCb rxCb;
static EasyLink_GetRandomNumber getRN;

/***** Variable declarations *****/

//Rx buffer includes data entry structure, hdr (len=1byte), dst addr (max of 8 bytes) and data
//which must be aligned to 4B
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN (rxBuffer, 4);
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 4
#elif defined(__GNUC__)
__attribute__((aligned(4)))
#else
#error This compiler is not supported.
#endif
static uint8_t rxBuffer[sizeof(rfc_dataEntryGeneral_t) + 1 +
                        EASYLINK_MAX_ADDR_SIZE +
                        EASYLINK_MAX_DATA_LENGTH];

static dataQueue_t dataQueue;
static rfc_propRxOutput_t rxStatistics;

//Tx buffer includes hdr (len=1 or 2 bytes), dst addr (1 byte) and data // original: (max of 8 bytes) and data
static uint8_t txBuffer[2U + EASYLINK_MAX_ADDR_SIZE + EASYLINK_MAX_DATA_LENGTH];

// Addr size for Filter and Tx/Rx operations
// Set default to 1 byte addr to work with SmartRF studio default settings
// NOTE that it can take on values in the range [0, EASYLINK_MAX_ADDR_SIZE]
static uint8_t addrSize = EASYLINK_ADDR_SIZE;

// Default address transmitted when EASYLINK_USE_DEFAULT_ADDR = true
static uint8_t defaultAddr = EASYLINK_DEFAULT_ADDR;//defaultAddr[8] = EASYLINK_DEFAULT_ADDR;

// Header Size (in bytes) for Advanced Tx operations. For IEEE 802.15.4g modes
// it is 2 bytes, 1 for the rest
static uint8_t hdrSize = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_PROP_HDR_NBITS);

//Indicating that the API is initialized
 uint8_t configured = 0;
//Indicating that the API suspended
 uint8_t suspended = 0;
//Use an IEEE header for the Tx/Rx command
 bool useIeeeHeader = 0;
//RF Params allowing configuration of the inactivity timeout, which is the time
//it takes for the radio to shut down when there are no commands in the queue
 bool rfParamsConfigured = 0;

//Flag used to indicate the multi client operation is enabled
static bool rfModeMultiClient = EASYLINK_ENABLE_MULTI_CLIENT;

//Async Rx timeout value
static uint32_t asyncRxTimeOut = EASYLINK_ASYNC_RX_TIMEOUT;

// Current Command Priority
static uint32_t cmdPriority = EasyLink_Priority_Normal;

//local commands, contents will be defined by modulation type
/*
static setupCmd_t EasyLink_cmdPropRadioSetup;
static rfc_CMD_FS_t EasyLink_cmdFs;
static RF_Mode EasyLink_RF_prop;
static rfc_CMD_PROP_TX_ADV_t EasyLink_cmdPropTxAdv;
static rfc_CMD_PROP_RX_ADV_t EasyLink_cmdPropRxAdv;
static rfc_CMD_PROP_CS_t EasyLink_cmdPropCs;
static EasyLink_Params EasyLink_params;
// Command schedule parameters, needed by the DMM to schedule commands in amulti-client setup
static RF_ScheduleCmdParams schParams_prop;
static RF_Object rfObject;
static RF_Handle rfHandle;
static RF_Params rfParams;
*/
//----------------------------------------------
//list of pointers commands
 setupCmd_t* pEasyLink_cmdPropRadioSetup;
 rfc_CMD_FS_t* pEasyLink_cmdFs;
 RF_Mode* pEasyLink_RF_prop;
 rfc_CMD_PROP_TX_ADV_t* pEasyLink_cmdPropTxAdv;
 rfc_CMD_PROP_RX_ADV_t* pEasyLink_cmdPropRxAdv;
 rfc_CMD_PROP_CS_t* pEasyLink_cmdPropCs;
 RF_ScheduleCmdParams* pschParams_prop;
 EasyLink_Params* pEasyLink_params;
 RF_Object* prfObject;
 RF_Handle* prfHandle;
 RF_Params* prfParams;
// The table for setting the Rx Address Filters
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN (addrFilterTable, 4);
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 4
#elif defined(__GNUC__)
__attribute__((aligned(4)))
#endif
static uint8_t addrFilterTable[EASYLINK_MAX_ADDR_FILTERS * EASYLINK_MAX_ADDR_SIZE] = EASYLINK_ADDR_FILTER_TABLE;

// Used as a pointer to an entry in the EasyLink_rfSettings array
static EasyLink_RfSetting *rfSetting = 0;

// Default inactivity timeout of 1 ms
static uint32_t inactivityTimeout = EASYLINK_IDLE_TIMEOUT;

//Mutex for locking the RF driver resource
#ifdef TI_RTOS
static Semaphore_Handle busyMutex;
#endif
#ifdef RTOS_AL
static sempaphore_struct busyMutex;
#endif
//Handle for last Async command, which is needed by EasyLink_abort
static RF_CmdHandle asyncCmdHndl = EASYLINK_RF_CMD_HANDLE_INVALID;

/* Set Default parameters structure */
static const EasyLink_Params EasyLink_defaultParams = EASYLINK_PARAM_CONFIG;


// Check the address size (in bytes) in the range [0, EASYLINK_MAX_ADDR_SIZE]
static bool isAddrSizeValid(uint8_t ui8AddrSize)
{
    return((ui8AddrSize == 0) || (ui8AddrSize <= EASYLINK_MAX_ADDR_SIZE));
}

void EasyLink_Params_init(EasyLink_Params *params)
{
    *params = EasyLink_defaultParams;
}

// Generate the activity table value
static uint32_t genActivityTableValue(EasyLink_Activity activity, EasyLink_Priority priority)
{
    return((activity << 16) | priority);
}

//Create an Advanced Tx command from a Tx Command
void createTxAdvFromTx(rfc_CMD_PROP_TX_ADV_t *dst, rfc_CMD_PROP_TX_t *src)
{
    memset(dst, 0 , sizeof(rfc_CMD_PROP_TX_ADV_t));
    dst->commandNo = CMD_PROP_TX_ADV;
    memcpy(&(dst->status), &(src->status), offsetof(rfc_CMD_PROP_TX_ADV_t, pktConf) -
           offsetof(rfc_CMD_PROP_TX_ADV_t, status));
    dst->pktConf.bFsOff = src->pktConf.bFsOff;
    dst->pktConf.bUseCrc = src->pktConf.bUseCrc;
    dst->pktLen = src->pktLen;
    dst->syncWord = src->syncWord;
}

//Callback for Async Tx complete
static void txDoneCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    EasyLink_Status status;

    //Release now so user callback can call EasyLink API's
	#ifdef TI_RTOS
    Semaphore_post(busyMutex);
	#endif
	#ifdef RTOS_AL
    semaphore_release(&busyMutex);
	#endif
    asyncCmdHndl = EASYLINK_RF_CMD_HANDLE_INVALID;

    if (e & RF_EventLastCmdDone)
    {
        status = EasyLink_Status_Success;
    }
    else if ( (e & RF_EventCmdAborted) || (e & RF_EventCmdCancelled) || (e & RF_EventCmdPreempted) )
    {
        status = EasyLink_Status_Aborted;
    }
    else if(e == (RF_EventCmdCancelled | RF_EventCmdPreempted))
    {
        status = EasyLink_Status_Cmd_Rejected;
    }
    else
    {
        status = EasyLink_Status_Tx_Error;
    }

    if (txCb != NULL)
    {
        txCb(status);
    }
}

// Calculate the command time for a given txPacket in ms
static uint32_t calculateCmdTime(EasyLink_TxPacket *txPacket)
{
    uint32_t cmdTime = 0;

    if(pEasyLink_params->ui32ModType == EasyLink_Phy_5kbpsSlLr)
    {
        /* calculate the command time for the packet format:
         *
         * +--------+--------+--------+----------------------------+-----+----+
         * |INV_SYNC|INV_SYNC|SYNCWORD|_____PAYLOAD(LEN + 1)_______|_CRC_|TERM|
         * |        |        |        |__LEN__|ADDR|____DATA_______|     |    |
         * |        |        |        |       | (A)|               |     |    |
         * |   64S  |   64S  |   64S  |  1B   |1-8B|  'LEN-A' B    | 2B  | 7B |
         * +--------+--------+--------+-------+--------------------+-----+----+
         *                             <----------pktlen---------->
         * S - symbols, B -bytes
         * Time-of-Flight,
         * nsymbols = (M+1)*SYNCWORD + LEN + ADDR + DATA + CRC + TERM
         *          = (M+1)*64 + (pktlen + CRC + TERM)*8*2*DSSS
         *          = 3*64 + (pktlen+9)*32
         *          = pktlen*32 + 480
         * M - number of inverted sync words (RF_cmdPropRadioDivSetup_sl_lr.preamConf.nPreamBytes)
         * DSSS - spreading factor of 2, set in the overrides
         *
         * TOF (ms) = (nsymbols/symbol_rate)*1e3
         *          = (pktlen*32 + 480 / 20e3)*1e3
         *          = (pktlen*32 + 480 / 20)
         */
        cmdTime = (pEasyLink_cmdPropTxAdv->pktLen*PKT_NCHIPS_SLR_5K + PKT_OVHD_SLR_5K) / BAUD_RATE_SLR_5K;
    }
    else if(pEasyLink_params->ui32ModType == EasyLink_Phy_200kbps2gfsk)
    {
        /* calculate the command time for the packet format:
         *
         * +----------+----------+----------------------------+-----+
         * |_PREAMBLE_|_SYNCWORD_|_____PAYLOAD(LEN + 1)_______|_CRC_|
         * |          |          |__HDR__|ADDR|____DATA_______|     |
         * |          |          |  LEN  | (A)|               |     |
         * |    4B    |    4B    |  2B   |1-8B|  'LEN-A' B    | 2B  |
         * +----------+----------+-------+--------------------+-----+
         *                        <----------pktlen---------->
         * Time-of-Flight,
         * TOF(ms) = (PREAMBLE + SYNCWORD + LEN + ADDR + DATA + CRC) * 8bits/200kbps *1e3
         *         = (pktlen + 10) * 8/200k
         */
        cmdTime = ((pEasyLink_cmdPropTxAdv->pktLen + PKT_OVHD_2GFSK_200K) * CHAR_BIT) / BAUD_RATE_2GFSK_200K;
    }
    else //assume 50kbps
    {
        /* calculate the command time for the packet format:
         *
         * +----------+----------+----------------------------+-----+
         * |_PREAMBLE_|_SYNCWORD_|_____PAYLOAD(LEN + 1)_______|_CRC_|
         * |          |          |__LEN__|ADDR|____DATA_______|     |
         * |          |          |       | (A)|               |     |
         * |    4B    |    4B    |  1B   |1-8B|  'LEN-A' B    | 2B  |
         * +----------+----------+-------+--------------------+-----+
         *                        <----------pktlen---------->
         * Time-of-Flight,
         * TOF(ms) = (PREAMBLE + SYNCWORD + LEN + ADDR + DATA + CRC) * 8bits/50kbps *1e3
         *         = (pktlen + 10) * 8/50k
         */
        cmdTime = ((pEasyLink_cmdPropTxAdv->pktLen + PKT_OVHD_2GFSK_50K) * CHAR_BIT) / BAUD_RATE_2GFSK_50K;
    }

    return (cmdTime);
}

//Callback for Clear Channel Assessment Done
static void ccaDoneCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    EasyLink_Status status        = EasyLink_Status_Tx_Error;
    RF_Op* pCmd                   = RF_getCmdOp(h, ch);
    bool bCcaRunAgain             = false;
    static uint8_t be             = EASYLINK_MIN_CCA_BACKOFF_WINDOW;
    static uint32_t backOffTime;

    pschParams_prop->startTime    = 0U;
    pschParams_prop->startType    = RF_StartNotSpecified;
    pschParams_prop->allowDelay   = RF_AllowDelayNone;
    pschParams_prop->endTime      = 0U;
    pschParams_prop->endType      = RF_EndNotSpecified;
    pschParams_prop->duration     = 0U;
    pschParams_prop->activityInfo = genActivityTableValue(EasyLink_Activity_Tx, (EasyLink_Priority)cmdPriority);

    asyncCmdHndl = EASYLINK_RF_CMD_HANDLE_INVALID;

    if (e & RF_EventLastCmdDone)
    {
        if(pCmd->status ==  PROP_DONE_IDLE)
        {
            // Carrier Sense operation ended with an idle channel,
            // and the next op (TX) should have already taken place
            // Failure to transmit is reflected in the default status,
            // EasyLink_Status_Tx_Error, being set
            if(pCmd->pNextOp->status == PROP_DONE_OK)
            {
                //Release now so user callback can call EasyLink API's
				#ifdef TI_RTOS
            	Semaphore_post(busyMutex);
				#endif
				#ifdef RTOS_AL
            	semaphore_release(&busyMutex);
				#endif
                status = EasyLink_Status_Success;
                // Reset the number of retries
                be = EASYLINK_MIN_CCA_BACKOFF_WINDOW;
            }
        }
        else if(pCmd->status == PROP_DONE_BUSY)
        {
            if(be > EASYLINK_MAX_CCA_BACKOFF_WINDOW)
            {
                //Release now so user callback can call EasyLink API's
				#ifdef TI_RTOS
            	Semaphore_post(busyMutex);
				#endif
				#ifdef RTOS_AL
            	semaphore_release(&busyMutex);
				#endif
                // Reset the number of retries
                be = EASYLINK_MIN_CCA_BACKOFF_WINDOW;
                // CCA failed max number of retries
                status = EasyLink_Status_Busy_Error;
            }
            else
            {
                // The back-off time is a random number chosen from 0 to 2^be,
                // where 'be' goes from EASYLINK_MIN_CCA_BACKOFF_WINDOW
                // to EASYLINK_MAX_CCA_BACKOFF_WINDOW. This number is then converted
                // into EASYLINK_CCA_BACKOFF_TIMEUNITS units, and subsequently used to
                // schedule the next CCA sequence. The variable 'be' is incremented each
                // time, up to a pre-configured maximum, the back-off algorithm is run.
                backOffTime = (getRN() & ((1 << be++)-1)) *
                        EasyLink_us_To_RadioTime(EASYLINK_CCA_BACKOFF_TIMEUNITS);
                // running CCA again
                bCcaRunAgain = true;
                // The random number generator function returns a value in the range
                // 0 to 2^15 - 1 and we choose the 'be' most significant bits as our
                // back-off time in milliseconds (converted to RAT ticks)
                pCmd->startTime = RF_getCurrentTime() + backOffTime;
                // post the chained CS+TX command again while checking
                // for a clear channel (CCA) before sending a packet
                if(rfModeMultiClient)
                {
                    pschParams_prop->startTime    = pCmd->startTime;
                    pschParams_prop->startType    = RF_StartAbs;
                    asyncCmdHndl = RF_scheduleCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropCs,
                                                  pschParams_prop, ccaDoneCallback, EASYLINK_RF_EVENT_MASK);
                }
                else
                {
                    asyncCmdHndl = RF_postCmd(h, (RF_Op*)pEasyLink_cmdPropCs,
                        RF_PriorityHigh, ccaDoneCallback, EASYLINK_RF_EVENT_MASK);
                }

                if(EasyLink_CmdHandle_isValid(asyncCmdHndl) == false)
                {
                    //Release now so user callback can call EasyLink API's
				#ifdef TI_RTOS
                Semaphore_post(busyMutex);
				#endif
				#ifdef RTOS_AL
                semaphore_release(&busyMutex);
				#endif

                    // Reset the number of retries
                    be = EASYLINK_MIN_CCA_BACKOFF_WINDOW;
                }
            }
        }
        else
        {
            //Release now so user callback can call EasyLink API's
        #ifdef TI_RTOS
        	Semaphore_post(busyMutex);
		#endif
		#ifdef RTOS_AL
        	semaphore_release(&busyMutex);
		#endif
            // Reset the number of retries
            be = EASYLINK_MIN_CCA_BACKOFF_WINDOW;
            // The CS command status should be either IDLE or BUSY,
            // all other status codes can be considered errors
            // Status is set to the default, EasyLink_Status_Tx_Error
        }


    }
    else if ((e & RF_EventCmdAborted)   || (e & RF_EventCmdCancelled) || (e & RF_EventCmdPreempted))
    {
        //Release now so user callback can call EasyLink API's
		#ifdef TI_RTOS
    	Semaphore_post(busyMutex);
		#endif
		#ifdef RTOS_AL
    	semaphore_release(&busyMutex);
		#endif
        // Reset the number of retries
        be = EASYLINK_MIN_CCA_BACKOFF_WINDOW;
        if(e == (RF_EventCmdCancelled | RF_EventCmdPreempted))
        {
            status = EasyLink_Status_Cmd_Rejected;
        }
        else
        {
            status = EasyLink_Status_Aborted;
        }
    }
    else
    {
        //Release now so user callback can call EasyLink API's
        #ifdef TI_RTOS
    	Semaphore_post(busyMutex);
		#endif
		#ifdef RTOS_AL
    	semaphore_release(&busyMutex);
		#endif
        // Reset the number of retries
        be = EASYLINK_MIN_CCA_BACKOFF_WINDOW;
        // Status is set to the default, EasyLink_Status_Tx_Error
    }

    if ((txCb != NULL) && (!bCcaRunAgain))
    {
        txCb(status);
    }
}

//Callback for Async Rx complete
static void rxDoneCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    EasyLink_Status status = EasyLink_Status_Rx_Error;
    //create rxPacket as a static so that the large payload buffer it is not
    //allocated from the stack
    static EasyLink_RxPacket rxPacket;
    rfc_dataEntryGeneral_t *pDataEntry;
    pDataEntry = (rfc_dataEntryGeneral_t*) rxBuffer;

    if (e & RF_EventLastCmdDone)
    {
        //Release now so user callback can call EasyLink API's
		#ifdef TI_RTOS
    		Semaphore_post(busyMutex);
		#endif
		#ifdef RTOS_AL
    	semaphore_release(&busyMutex);
		#endif
        asyncCmdHndl = EASYLINK_RF_CMD_HANDLE_INVALID;

        //Check command status
        if (pEasyLink_cmdPropRxAdv->status == PROP_DONE_OK)
        {
            //Check that data entry status indicates it is finished with
            if (pDataEntry->status != DATA_ENTRY_FINISHED)
            {
                status = EasyLink_Status_Rx_Error;
            }
            else if ( (rxStatistics.nRxOk == 1) ||
                    //or filer disabled and ignore due to addr mistmatch
                    ((pEasyLink_cmdPropRxAdv->pktConf.filterOp == 1) &&
                     (rxStatistics.nRxIgnored == 1)) )
            {
                //copy length from pDataEntry
                rxPacket.len = *(uint8_t*)(&pDataEntry->data) - addrSize;
                if(useIeeeHeader)
                {
                    hdrSize = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_IEEE_HDR_NBITS);
                }
                else
                {
                    hdrSize = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_PROP_HDR_NBITS);
                }
                //copy address from packet payload (as it is not in hdr)
                memcpy(&rxPacket.dstAddr, (&pDataEntry->data + hdrSize), addrSize);
                if(EASYLINK_MAX_DATA_LENGTH >= rxPacket.len)
                {
                    //copy payload
                    memcpy(&rxPacket.payload, (&pDataEntry->data + hdrSize + addrSize), rxPacket.len);
                    rxPacket.rssi = rxStatistics.lastRssi;
                    rxPacket.absTime = rxStatistics.timeStamp;

                    status = EasyLink_Status_Success;
                }
                else
                {
                    // Packet payload too long
                    status = EasyLink_Status_Rx_Buffer_Error;
                }
            }
            else if ( rxStatistics.nRxBufFull == 1)
            {
                status = EasyLink_Status_Rx_Buffer_Error;
            }
            else if ( rxStatistics.nRxStopped == 1)
            {
                status = EasyLink_Status_Aborted;
            }
            else
            {
                status = EasyLink_Status_Rx_Error;
            }
        }
        else if ( pEasyLink_cmdPropRxAdv->status == PROP_DONE_RXTIMEOUT)
        {
            status = EasyLink_Status_Rx_Timeout;
        }
        else
        {
            status = EasyLink_Status_Rx_Error;
        }
    }
    else if (e & (RF_EventCmdCancelled | RF_EventCmdAborted | RF_EventCmdPreempted | RF_EventCmdStopped))
    {
        //Release now so user callback can call EasyLink API's
   	   	#ifdef TI_RTOS
    	Semaphore_post(busyMutex);
		#endif
		#ifdef RTOS_AL
    	semaphore_release(&busyMutex);
		#endif
        asyncCmdHndl = EASYLINK_RF_CMD_HANDLE_INVALID;
        if(e == (RF_EventCmdCancelled | RF_EventCmdPreempted))
        {
            status = EasyLink_Status_Cmd_Rejected;
        }
        else
        {
            status = EasyLink_Status_Aborted;
        }
    }

    if (rxCb != NULL)
    {
        rxCb(&rxPacket, status);
    }
}

//Callback for Async TX Test mode
static void asyncCmdCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
#ifdef TI_RTOS
	Semaphore_post(busyMutex);
#endif
#ifdef RTOS_AL
	semaphore_release(&busyMutex);
#endif
    asyncCmdHndl = EASYLINK_RF_CMD_HANDLE_INVALID;
}

static EasyLink_Status enableTestMode(EasyLink_CtrlOption mode)
{
    EasyLink_Status status = EasyLink_Status_Cmd_Error;
    //This needs to be static as it is used by the RF driver and Modem after
    //this function exits
    static rfc_CMD_TX_TEST_t txTestCmd = {0};
    static rfc_CMD_RX_TEST_t rxTestCmd = {0};

    pschParams_prop->startTime    = 0U;
    pschParams_prop->startType    = RF_StartNotSpecified;
    pschParams_prop->allowDelay   = RF_AllowDelayNone;
    pschParams_prop->endTime      = 0U;
    pschParams_prop->endType      = RF_EndNotSpecified;
    pschParams_prop->duration     = 0U;
    pschParams_prop->activityInfo = genActivityTableValue(EasyLink_Activity_Rx, (EasyLink_Priority)cmdPriority);

    if((!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }
    if((mode != EasyLink_Ctrl_Test_Tone)    &&
       (mode != EasyLink_Ctrl_Test_Signal)  &&
       (mode != EasyLink_Ctrl_Rx_Test_Tone))
    {
        return EasyLink_Status_Param_Error;
    }
    if (EasyLink_CmdHandle_isValid(asyncCmdHndl))
    {
        return EasyLink_Status_Busy_Error;
    }
    //Check and take the busyMutex
	#ifdef TI_RTOS
    if (Semaphore_pend(busyMutex, 0) == false)
	#endif
	#ifdef RTOS_AL
    if (semaphore_trytotake_time(&busyMutex, 0) == false)
	#endif
    {
        return EasyLink_Status_Busy_Error;
    }

    if((mode == EasyLink_Ctrl_Test_Tone) || (mode == EasyLink_Ctrl_Test_Signal))
    {
        txTestCmd.commandNo = CMD_TX_TEST;
        txTestCmd.startTrigger.triggerType = TRIG_NOW;
        txTestCmd.startTrigger.pastTrig = 1;
        txTestCmd.startTime = 0;

        txTestCmd.config.bFsOff = 1;
        txTestCmd.syncWord = pEasyLink_cmdPropTxAdv->syncWord;

        /* WhitenMode
         * 0: No whitening
         * 1: Default whitening
         * 2: PRBS-15
         * 3: PRBS-32
         */
        txTestCmd.config.whitenMode = EASYLINK_WHITENING_MODE;

        //set tone (unmodulated) or signal (modulated)
        if (mode == EasyLink_Ctrl_Test_Tone)
        {
            txTestCmd.txWord = 0xFFFF;
            txTestCmd.config.bUseCw = 1;
        }
        else
        {
            txTestCmd.txWord = 0xAAAA;
            txTestCmd.config.bUseCw = 0;
        }

        //generate continuous test signal
        txTestCmd.endTrigger.triggerType = TRIG_NEVER;

        /* Post command and store Cmd Handle for future abort */
        if(rfModeMultiClient)
        {
            asyncCmdHndl = RF_scheduleCmd(*prfHandle, (RF_Op*)&txTestCmd,
                        pschParams_prop, asyncCmdCallback, EASYLINK_RF_EVENT_MASK);
        }
        else
        {
            asyncCmdHndl = RF_postCmd(*prfHandle, (RF_Op*)&txTestCmd,
                                      RF_PriorityNormal, asyncCmdCallback,
                                      EASYLINK_RF_EVENT_MASK);
        }

        /* Has command completed? */
        uint16_t count = 0;
        while (txTestCmd.status != ACTIVE)
        {
            //The command did not complete as fast as expected, sleep for 10ms
            #ifdef TI_RTOS
            Task_sleep(10000 / Clock_tickPeriod);
  			#endif
  			#ifdef RTOS_AL
            vTaskDelay(SEC_TO(0.01));
  			#endif

            if (count++ > 500)
            {
                //Should not get here, if we did Something went wrong with the
                //the RF Driver, get out of here and return an error.
                //The next command will likely lock up.
                break;
            }
        }

        if (txTestCmd.status == ACTIVE)
        {
            status = EasyLink_Status_Success;
        }
    }
    else // mode is EasyLink_Ctrl_Rx_Test_Tone
    {
        rxTestCmd.commandNo = CMD_RX_TEST;
        rxTestCmd.startTrigger.triggerType = TRIG_NOW;
        rxTestCmd.startTrigger.pastTrig = 1;
        rxTestCmd.startTime = 0;

        rxTestCmd.config.bFsOff = 1;
        // Correlation threshold set to max to prevent sync, as RSSI values
        // are locked after sync
        rxTestCmd.config.bNoSync = 1;
        rxTestCmd.syncWord = pEasyLink_cmdPropRxAdv->syncWord0;

        //detect test signal continuously
        rxTestCmd.endTrigger.triggerType = TRIG_NEVER;

        /* Post command and store Cmd Handle for future abort */
        if(rfModeMultiClient)
        {
            asyncCmdHndl = RF_scheduleCmd(*prfHandle, (RF_Op*)&rxTestCmd,
                pschParams_prop, txDoneCallback, EASYLINK_RF_EVENT_MASK);
        }
        else
        {
            asyncCmdHndl = RF_postCmd(*prfHandle, (RF_Op*)&rxTestCmd,
                                      RF_PriorityNormal, asyncCmdCallback,
                                      EASYLINK_RF_EVENT_MASK);
        }

        if(EasyLink_CmdHandle_isValid(asyncCmdHndl))
        {
            status = EasyLink_Status_Success;
        }
        else
        {
            status = EasyLink_Status_Cmd_Error;
        }
    }

    return status;
}

#if defined(EASYLINK_ACTIVITY_PROFILING)
uint32_t numCmdFlush = 0U;
static void activityPreemptCb(xdc_UArg arg)
{
    if(arg)
    {
        /* Flush all commands from the queue */
        (void)RF_flushCmd((RF_Handle)arg, RF_CMDHANDLE_FLUSH_ALL, RF_ABORT_PREEMPTION);
        numCmdFlush++;
    }
}
#endif // EASYLINK_ACTIVITY_PROFILING

EasyLink_Status EasyLink_close(EasyLink_rf_Params* pEasyLink_RfParams_ext){
if((pEasyLink_RfParams_ext->rfHandle)!=NULL) {
        RF_close(pEasyLink_RfParams_ext->rfHandle);
        pEasyLink_RfParams_ext->rfHandle=NULL;
        prfHandle=&(pEasyLink_RfParams_ext->rfHandle);
}
    return(EasyLink_Status_Success);
/*
	if(*prfHandle!=NULL) {
		RF_close(*prfHandle);
	    *prfHandle=NULL;}
    return(EasyLink_Status_Success);*/
}
/**/
void EasyLink_set_radio_structs_pointers(EasyLink_rf_Params* pEasyLink_RfParams_ext)
{
	/*pass the pointers */
	pEasyLink_RF_prop=&(pEasyLink_RfParams_ext->EasyLink_RF_prop);//set the parameters stores as currents one
	pEasyLink_cmdPropRadioSetup=&(pEasyLink_RfParams_ext->EasyLink_cmdPropRadioSetup);
	pEasyLink_cmdFs=&(pEasyLink_RfParams_ext->EasyLink_cmdFs);
	pEasyLink_cmdPropCs=&(pEasyLink_RfParams_ext->EasyLink_cmdPropCs);
	pEasyLink_cmdPropRxAdv=&(pEasyLink_RfParams_ext->EasyLink_cmdPropRxAdv);
	pEasyLink_cmdPropTxAdv=&(pEasyLink_RfParams_ext->EasyLink_cmdPropTxAdv);
	pschParams_prop=&(pEasyLink_RfParams_ext->schParams_prop);
	pEasyLink_params=&(pEasyLink_RfParams_ext->EasyLink_params);
	prfObject=&(pEasyLink_RfParams_ext->rfObject);
	prfHandle=&(pEasyLink_RfParams_ext->rfHandle);
	prfParams=&(pEasyLink_RfParams_ext->rfParams);
	configured=pEasyLink_RfParams_ext->configured;
	suspended=pEasyLink_RfParams_ext->suspended;//Indicating that the API suspended
	useIeeeHeader=pEasyLink_RfParams_ext->useIeeeHeader;
	rfParamsConfigured=pEasyLink_RfParams_ext->rfParamsConfigured;
}

EasyLink_Status EasyLink_switch_radio( EasyLink_rf_Params* pEasyLink_RfParams_ext)
{   //  if (rfHandle=!NULL) {RF_close(rfHandle);} //close the current handle
     //set tje [pomters
	  EasyLink_set_radio_structs_pointers(pEasyLink_RfParams_ext);
	  if (pEasyLink_RfParams_ext->rfHandle==NULL) {
	      pEasyLink_RfParams_ext->rfHandle = RF_open(prfObject, pEasyLink_RF_prop,(RF_RadioSetup*)&(pEasyLink_cmdPropRadioSetup->setup), prfParams);
	      RF_runScheduleCmd(pEasyLink_RfParams_ext->rfHandle, (RF_Op*)&(pEasyLink_RfParams_ext->EasyLink_cmdFs), &(pEasyLink_RfParams_ext->schParams_prop), 0, //asyncCmdCallback,
	                      EASYLINK_RF_EVENT_MASK);
	      //prfHandle=&(pEasyLink_RfParams_ext->rfHandle);
	  }//store the handle
	  if (pEasyLink_RfParams_ext->rfHandle==NULL) {return(EasyLink_Status_Config_Error);}
	  else{						return(EasyLink_Status_Success);}
}
EasyLink_Status EasyLink_init_phys(EasyLink_RfSetting * EasyLink_Phys, EasyLink_rf_Params* EasyLink_RfParams )
{
	  prfHandle=NULL;
	  EasyLink_Params_init(&(EasyLink_RfParams->EasyLink_params));
	  EasyLink_RfParams->configured=0;
	  EasyLink_RfParams->rfParamsConfigured=false;
	  EasyLink_RfParams->suspended=0;
	  EasyLink_RfParams->useIeeeHeader=false;
    if (&(EasyLink_RfParams->EasyLink_params) == NULL)
    {
    	EasyLink_Params_init(&(EasyLink_RfParams->EasyLink_params));}
    if ((EasyLink_RfParams->configured)==1)
    { //Already configure, check and take the busyMutex
            #ifdef TI_RTOS
        	  if (Semaphore_pend(busyMutex, 0) == false)
    		#endif
    		#ifdef RTOS_AL
              if (semaphore_trytotake_time(&busyMutex, 0) == false)
    		#endif
              { return EasyLink_Status_Busy_Error;}
        RF_close(*prfHandle);
    }

    if (!(EasyLink_RfParams->rfParamsConfigured))
    {
        RF_Params_init(&(EasyLink_RfParams->rfParams));
        //set default InactivityTimeout to 1000us
        EasyLink_RfParams->rfParams.nInactivityTimeout = inactivityTimeout;
        //EasyLink_RfParams->rfParams.nPowerUpDurationMargin = 500;//added
        //configure event callback
        if((EasyLink_RfParams->EasyLink_params).pClientEventCb != NULL && (EasyLink_RfParams->EasyLink_params).nClientEventMask != 0){
        	EasyLink_RfParams->rfParams.pClientEventCb = (EasyLink_RfParams->EasyLink_params).pClientEventCb;
        	EasyLink_RfParams->rfParams.nClientEventMask = (EasyLink_RfParams->EasyLink_params).nClientEventMask;
        }

        EasyLink_RfParams->rfParams.nID = RF_STACK_ID_EASYLINK+rfParamsnID_counter;
        ++rfParamsnID_counter;
        EasyLink_RfParams->rfParamsConfigured = 1;
        // Initialize the schedule parameters
        RF_ScheduleCmdParams_init(&(EasyLink_RfParams->schParams_prop));
    }

    // Assign the random number generator function pointer to the global
    // handle, if it is NULL any function that employs it will return a
    // configuration error
    getRN = (EasyLink_RfParams->EasyLink_params).pGrnFxn;

    // Configure the EasyLink Carrier Sense Command
    memset(&(EasyLink_RfParams->EasyLink_cmdPropCs), 0, sizeof(rfc_CMD_PROP_CS_t));
    EasyLink_RfParams->EasyLink_cmdPropCs.commandNo                = CMD_PROP_CS;
    EasyLink_RfParams->EasyLink_cmdPropCs.rssiThr                  = EASYLINK_CS_RSSI_THRESHOLD_DBM;
    EasyLink_RfParams->EasyLink_cmdPropCs.startTrigger.triggerType = TRIG_NOW;
    EasyLink_RfParams->EasyLink_cmdPropCs.condition.rule           = COND_STOP_ON_TRUE;  // Stop next command if this command returned TRUE,
                                                            // End causes for the CMD_PROP_CS command:
                                                            // Observed channel state Busy with csConf.busyOp = 1:                            PROP_DONE_BUSY        TRUE
                                                            // 0bserved channel state Idle with csConf.idleOp = 1:                            PROP_DONE_IDLE        FALSE
                                                            // Timeout trigger observed with channel state Busy:                              PROP_DONE_BUSY        TRUE
                                                            // Timeout trigger observed with channel state Idle:                              PROP_DONE_IDLE        FALSE
                                                            // Timeout trigger observed with channel state Invalid and csConf.timeoutRes = 0: PROP_DONE_BUSYTIMEOUT TRUE
                                                            // Timeout trigger observed with channel state Invalid and csConf.timeoutRes = 1: PROP_DONE_IDLETIMEOUT FALSE
                                                            // Received CMD_STOP after command started:                                       PROP_DONE_STOPPED     FALSE
    EasyLink_RfParams->EasyLink_cmdPropCs.csConf.bEnaRssi          = 0x1; // Enable RSSI as a criterion
    EasyLink_RfParams->EasyLink_cmdPropCs.csConf.busyOp            = 0x1; // End carrier sense on channel Busy
    EasyLink_RfParams->EasyLink_cmdPropCs.csConf.idleOp            = 0x0; // Continue carrier sense on channel Idle
    EasyLink_RfParams->EasyLink_cmdPropCs.csEndTrigger.triggerType = TRIG_REL_START; // Ends at a time relative to the command started
    EasyLink_RfParams->EasyLink_cmdPropCs.csEndTime                = EasyLink_us_To_RadioTime(EASYLINK_CHANNEL_IDLE_TIME_US);

#if (defined(FEATURE_OAD_ONCHIP))
    EasyLink_params->ui32ModType = EasyLink_Phy_Custom;
#endif

    bool useDivRadioSetup = true;
    bool rfConfigOk = false;
    bool createCmdTxAdvFromTx = false;
    EasyLink_RfParams->useIeeeHeader = false;

    // Check if the PHY setting is compatible with the current device
    switch((EasyLink_RfParams->EasyLink_params).ui32ModType)
    {
        case EasyLink_Phy_Custom:
            if((ChipInfo_ChipFamilyIs_CC26x0()) || (ChipInfo_ChipFamilyIs_CC26x0R2()))
            { useDivRadioSetup= false;
            }
            rfConfigOk = true;
        break;

        case EasyLink_Phy_50kbps2gfsk:
            if(!(ChipInfo_ChipFamilyIs_CC26x0()) && !(ChipInfo_ChipFamilyIs_CC26x0R2()))
            {  useDivRadioSetup= true;
                rfConfigOk = true; }
        break;

        case EasyLink_Phy_625bpsLrm:
            if(!(ChipInfo_ChipFamilyIs_CC26x0()) && !(ChipInfo_ChipFamilyIs_CC26x0R2())
                && !(ChipInfo_ChipFamilyIs_CC13x2_CC26x2()) && !(ChipInfo_ChipFamilyIs_CC13x2x7_CC26x2x7()))
            {useDivRadioSetup= true;
                rfConfigOk = true; }
        break;

        case EasyLink_Phy_2_4_100kbps2gfsk:
            if((ChipInfo_GetChipType() == CHIP_TYPE_CC2640R2))
            {useDivRadioSetup= false;
                rfConfigOk = true;}
            else if((ChipInfo_ChipFamilyIs_CC13x2_CC26x2()) && (ChipInfo_GetChipType() != CHIP_TYPE_CC1312))
            {
                useDivRadioSetup= true;
                rfConfigOk = true;
            }
            else if((ChipInfo_ChipFamilyIs_CC13x2x7_CC26x2x7()) && (ChipInfo_GetChipType() != CHIP_TYPE_CC1312R7))
            {useDivRadioSetup= true;
                rfConfigOk = true; }
        break;

        case EasyLink_Phy_2_4_200kbps2gfsk:
            if((ChipInfo_GetChipType() == CHIP_TYPE_CC2650))
            {useDivRadioSetup= false;
            rfConfigOk = true;}
        break;

        case EasyLink_Phy_2_4_250kbps2gfsk:
            if((ChipInfo_GetChipType() == CHIP_TYPE_CC2640R2))
            { useDivRadioSetup= false;
              rfConfigOk = true;    }
            else if((ChipInfo_ChipFamilyIs_CC13x2_CC26x2()) && (ChipInfo_GetChipType() != CHIP_TYPE_CC1312))
            {useDivRadioSetup= true;
                rfConfigOk = true; }
            else if((ChipInfo_ChipFamilyIs_CC13x2x7_CC26x2x7()) && (ChipInfo_GetChipType() != CHIP_TYPE_CC1312R7))
            {useDivRadioSetup= true;
                rfConfigOk = true; }

        break;

        case EasyLink_Phy_5kbpsSlLr:
            if(!(ChipInfo_ChipFamilyIs_CC26x0()) && !(ChipInfo_ChipFamilyIs_CC26x0R2()))
            { useDivRadioSetup= true;
                rfConfigOk = true;
#if (defined LAUNCHXL_CC1352P_4) || (defined LP_CC1352P7_4)
                // CC1352P-4 SLR operates at 433.92 MHz
                EasyLink_cmdFs.frequency = 0x01B1;
                EasyLink_cmdFs.fractFreq = 0xEB9A;
#endif
            }
        break;

        case EasyLink_Phy_200kbps2gfsk:
            if((ChipInfo_GetChipType() == CHIP_TYPE_CC1312) || (ChipInfo_GetChipType() == CHIP_TYPE_CC1352) ||
               (ChipInfo_GetChipType() == CHIP_TYPE_CC1352P) || (ChipInfo_GetChipType() == CHIP_TYPE_CC1312R7))
            {
#if !defined(LAUNCHXL_CC1352P_4) && !defined(LP_CC1352P7_4)
                // This mode is not supported in the 433 MHz band
                useDivRadioSetup= true;
                rfConfigOk = true;
#endif
            }
            break;

        default:  // Invalid PHY setting
            rfConfigOk = false;
        break;
    }

    // Return an error if the PHY setting is incompatible with the current device
    if(!rfConfigOk)
    {
        #ifdef TI_RTOS
        if (busyMutex != NULL){
        	Semaphore_post(busyMutex);
		#endif
		#ifdef RTOS_AL
        if (busyMutex.driver.handle != NULL){
        	semaphore_release(&busyMutex);
		#endif
        }
        return EasyLink_Status_Param_Error;
    }

    // check if the easylink is usefull
      if(EasyLink_Phys->EasyLink_phyType == (EasyLink_RfParams->EasyLink_params).ui32ModType)
        {
            rfSetting = EasyLink_Phys;
            if(EasyLink_Phys->RF_pCmdPropTxAdv == NULL)
            {
                // Advanced Tx command was not generated, create one from the
                // base Tx command. IEEE header is not used by default
                createCmdTxAdvFromTx = true;
            }
            else
            {
                // Advanced Tx command was generated, an IEEE header is
                // required for this PHY
                useIeeeHeader = true;
            }
        }

    // Return an error if the PHY isn't in the supported settings list
    if(rfSetting == 0)
    {

        #ifdef TI_RTOS
            if (busyMutex != NULL)
            {Semaphore_post(busyMutex);
		#endif
		#ifdef RTOS_AL
            if (busyMutex.driver.handle != NULL)
            {semaphore_release(&busyMutex);
		#endif
        }
        return EasyLink_Status_Param_Error;
    }

    // Copy the PHY settings to the EasyLink PHY variable
    if(useDivRadioSetup)
    {
#if ((defined LAUNCHXL_CC1352P1) || (defined LAUNCHXL_CC1352P_2) || \
     (defined LAUNCHXL_CC1352P_4)|| (defined LP_CC1352P7_1)      || \
	 (defined LP_CC1352P7_4)     || (defined CONFIG_LP_CC2652PSIP) || \
     (defined CONFIG_CC2652P1FSIP))
        memcpy(&((EasyLink_RfParams->EasyLink_cmdPropRadioSetup.divSetup), (rfSetting->RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup), sizeof(rfc_CMD_PROP_RADIO_DIV_SETUP_PA_t));
#else
        memcpy(&(EasyLink_RfParams->EasyLink_cmdPropRadioSetup.divSetup), (rfSetting->RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup), sizeof(rfc_CMD_PROP_RADIO_DIV_SETUP_t));
#endif
    }
    else
    {
        memcpy(&(EasyLink_RfParams->EasyLink_cmdPropRadioSetup.setup), (rfSetting->RF_uCmdPropRadio.RF_pCmdPropRadioSetup), sizeof(rfc_CMD_PROP_RADIO_SETUP_t));
    }
    memcpy(&(EasyLink_RfParams->EasyLink_cmdFs), (rfSetting->RF_pCmdFs), sizeof(rfc_CMD_FS_t));
    memcpy(&(EasyLink_RfParams->EasyLink_RF_prop), (rfSetting->RF_pProp), sizeof(RF_Mode));
    memcpy(&(EasyLink_RfParams->EasyLink_cmdPropRxAdv),(rfSetting->RF_pCmdPropRxAdv), sizeof(rfc_CMD_PROP_RX_ADV_t));
    if(createCmdTxAdvFromTx)
    {createTxAdvFromTx(&(EasyLink_RfParams->EasyLink_cmdPropTxAdv), (rfSetting->RF_pCmdPropTx));
    }
    else
    {memcpy(&(EasyLink_RfParams->EasyLink_cmdPropTxAdv), (rfSetting->RF_pCmdPropTxAdv), sizeof(rfc_CMD_PROP_TX_ADV_t));
    }

#if !(defined(DeviceFamily_CC26X0R2))
    if (rfModeMultiClient)
    {
    	EasyLink_RfParams->EasyLink_RF_prop.rfMode = RF_MODE_MULTIPLE;
    }
#endif //defined(DeviceFamily_CC26X0R2)

    /* Request access to the radio */
    EasyLink_RfParams->rfHandle = RF_open(&(EasyLink_RfParams->rfObject), &(EasyLink_RfParams->EasyLink_RF_prop),
            (RF_RadioSetup*)&(EasyLink_RfParams->EasyLink_cmdPropRadioSetup.setup), &(EasyLink_RfParams->rfParams));
    prfHandle=&(EasyLink_RfParams->rfHandle);
    // Setup the Proprietary Rx Advanced Command
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.status = 0x0000;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pNextOp = 0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.startTime = 0x00000000;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.startTrigger.triggerType = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.startTrigger.bEnaCmd = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.startTrigger.triggerNo = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.startTrigger.pastTrig = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.condition.rule = 0x1;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.condition.nSkip = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pktConf.bFsOff = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pktConf.bRepeatOk = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pktConf.bRepeatNok = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pktConf.bUseCrc = 0x1;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pktConf.bCrcIncSw = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pktConf.endType = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pktConf.filterOp = !(EASYLINK_ENABLE_ADDR_FILTERING);
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.rxConf.bAutoFlushIgnored = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.rxConf.bAutoFlushCrcErr = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.rxConf.bIncludeHdr = 0x1;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.rxConf.bIncludeCrc = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.rxConf.bAppendRssi = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.rxConf.bAppendTimestamp = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.rxConf.bAppendStatus = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.syncWord1 = 0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.maxPktLen = EASYLINK_MAX_DATA_LENGTH +
            EASYLINK_MAX_ADDR_SIZE;
    if(EasyLink_RfParams->useIeeeHeader)
    {
    	EasyLink_RfParams->EasyLink_cmdPropRxAdv.syncWord0 = EASYLINK_IEEE_TRX_SYNC_WORD;
        EasyLink_RfParams->EasyLink_cmdPropRxAdv.hdrConf.numHdrBits = EASYLINK_IEEE_HDR_NBITS;
        EasyLink_RfParams->EasyLink_cmdPropRxAdv.lenOffset = EASYLINK_IEEE_LEN_OFFSET;
        // Exclude the header from the CRC calculation
        EasyLink_RfParams->EasyLink_cmdPropRxAdv.pktConf.bCrcIncHdr = 0U;
    }
    else
    {
    	EasyLink_RfParams->EasyLink_cmdPropRxAdv.syncWord0 = EASYLINK_PROP_TRX_SYNC_WORD;
    	EasyLink_RfParams-> EasyLink_cmdPropRxAdv.hdrConf.numHdrBits = EASYLINK_PROP_HDR_NBITS;
    	EasyLink_RfParams->EasyLink_cmdPropRxAdv.lenOffset = EASYLINK_PROP_LEN_OFFSET;
        // Include the header in the CRC calculation - The header (length
        // byte) is considered to be the first byte of the payload
    	EasyLink_RfParams->EasyLink_cmdPropRxAdv.pktConf.bCrcIncHdr = 1U;
    }
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.hdrConf.numLenBits = EASYLINK_HDR_LEN_NBITS;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.hdrConf.lenPos = 0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.addrConf.addrType = 0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.addrConf.addrSize = addrSize;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.addrConf.addrPos = 0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.addrConf.numAddr = EASYLINK_NUM_ADDR_FILTER;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.endTrigger.triggerType = 0x1;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.endTrigger.bEnaCmd = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.endTrigger.triggerNo = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.endTrigger.pastTrig = 0x0;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.endTime = 0x00000000;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pAddr = addrFilterTable;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pQueue = &dataQueue;
    EasyLink_RfParams->EasyLink_cmdPropRxAdv.pOutput = (uint8_t*)&rxStatistics;

    if (rfModeMultiClient)
    {
        /* Set params */
    	EasyLink_RfParams->schParams_prop.startTime    = 0U;
    	EasyLink_RfParams->schParams_prop.startType    = RF_StartNotSpecified;
    	EasyLink_RfParams->schParams_prop.allowDelay   = RF_AllowDelayNone;
    	EasyLink_RfParams->schParams_prop.endTime      = 0U;
    	EasyLink_RfParams->schParams_prop.endType      = RF_EndNotSpecified;
    	EasyLink_RfParams->schParams_prop.duration     = 0U;
    	EasyLink_RfParams->schParams_prop.activityInfo = genActivityTableValue(EasyLink_Activity_Tx, (EasyLink_Priority)cmdPriority);

        //Set the frequency
        (void)RF_runScheduleCmd(EasyLink_RfParams->rfHandle, (RF_Op*)&(EasyLink_RfParams->EasyLink_cmdFs), &(EasyLink_RfParams->schParams_prop), 0, //asyncCmdCallback,
                EASYLINK_RF_EVENT_MASK);
    }
    else
    {
        //Set the frequency
        (void)RF_runCmd(EasyLink_RfParams->rfHandle, (RF_Op*)&(EasyLink_RfParams->EasyLink_cmdFs), RF_PriorityNormal, 0, //asyncCmdCallback,
                EASYLINK_RF_EVENT_MASK);
    }

    #ifdef TI_RTOS
    //Create a semaphore for blocking commands
    Semaphore_Params semParams;
    Error_Block eb;
    // init params
    Semaphore_Params_init(&semParams);
    Error_init(&eb);
    // create semaphore instance if not already created
    if (busyMutex == NULL){ busyMutex = Semaphore_create(0, &semParams, &eb);
        if (busyMutex == NULL)
        {return EasyLink_Status_Mem_Error;  }
        Semaphore_post(busyMutex);}
    else {Semaphore_post(busyMutex);//already configured and taken busyMutex, so release it
        }
	#endif
	#ifdef RTOS_AL
    if (busyMutex.driver.handle == NULL)
       {     semaphore_init(&busyMutex,0);
    if (busyMutex.driver.handle == NULL){
    	return EasyLink_Status_Mem_Error;  }
    	semaphore_release(&busyMutex);}
    else {semaphore_release(&busyMutex);//already configured and taken busyMutex, so release it
    	}
	#endif

    EasyLink_RfParams->configured = 1;

#if defined(EASYLINK_ACTIVITY_PROFILING)
/* 10 ms interrupt period based on 10us clock tick */
#define EASYLINK_ACTIVITY_PROFILING_PREEMPT_TIMEOUT  (1000)

    static Clock_Handle EasyLink_activityClock = NULL;

    /* Create a clock instance to periodically flush all RF commands */
    Clock_Params clkparams;
    Clock_Params_init(&clkparams);
    clkparams.startFlag = FALSE;
    clkparams.period    = EASYLINK_ACTIVITY_PROFILING_PREEMPT_TIMEOUT;
    clkparams.arg       = (xdc_UArg)EasyLink_RfParams->rfHandle;
    EasyLink_activityClock = Clock_create((Clock_FuncPtr)activityPreemptCb,
                                          EASYLINK_ACTIVITY_PROFILING_PREEMPT_TIMEOUT, &clkparams, NULL);
    Clock_start(EasyLink_activityClock);
#endif // EASYLINK_ACTIVITY_PROFILING

    return EasyLink_Status_Success;
}

EasyLink_Status EasyLink_setFrequency(uint32_t ui32Frequency_raw)
{
    EasyLink_Status status = EasyLink_Status_Cmd_Error;
    uint16_t centerFreq, fractFreq;
    uint32_t ui32Frequency=RF_SUB1GHZ_FREQ_COMP_SET(ui32Frequency_raw);
    if ( (!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }
    //Check and take the busyMutex
#ifdef TI_RTOS
  if (Semaphore_pend(busyMutex, 0) == false)
#endif
#ifdef RTOS_AL
  if (semaphore_trytotake_time(&busyMutex, 0) == false)
#endif
    {
        return EasyLink_Status_Busy_Error;
    }

    /* Set the frequency */
    centerFreq = (uint16_t)(ui32Frequency / 1000000);//do not compesate because is 9700 is lower than 1000000
    fractFreq  = (uint16_t) (( ( (uint64_t) ui32Frequency ) -
                             (   (uint64_t)centerFreq * 1000000)) *
                             65536 / 1000000);
    pEasyLink_cmdFs->frequency = centerFreq;
    pEasyLink_cmdFs->fractFreq = fractFreq;

    /* Set params */
    pschParams_prop->startTime    = 0U;
    pschParams_prop->startType    = RF_StartNotSpecified;
    pschParams_prop->allowDelay   = RF_AllowDelayNone;
    pschParams_prop->endTime      = 0U;
    pschParams_prop->endType      = RF_EndNotSpecified;
    pschParams_prop->duration     = 0U;
    pschParams_prop->activityInfo = genActivityTableValue(EasyLink_Activity_Tx, (EasyLink_Priority)cmdPriority);

#if (!defined(DeviceFamily_CC26X0R2) && !defined(DeviceFamily_CC26X0))
    /* If the command type is CMD_PROP_RADIO_DIV_SETUP then set the center frequency
     * to the same band as the FS command
     */
    if((pEasyLink_cmdPropRadioSetup->divSetup.commandNo == CMD_PROP_RADIO_DIV_SETUP) &&
       (pEasyLink_cmdPropRadioSetup->divSetup.centerFreq != centerFreq))
    {
        pEasyLink_cmdPropRadioSetup->divSetup.centerFreq = centerFreq;

        if (rfModeMultiClient)
        {
            /* Run the Setup Command */
            (void)RF_runScheduleCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropRadioSetup,
                      pschParams_prop, 0, EASYLINK_RF_EVENT_MASK);
        }
        else
        {
            /* Run the Setup Command */
            (void)RF_runCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropRadioSetup,
                      RF_PriorityNormal, 0, EASYLINK_RF_EVENT_MASK);
        }
    }
#endif

    RF_EventMask result;
    if (rfModeMultiClient)
    {
        /* Run command */
        result = RF_runScheduleCmd(*prfHandle, (RF_Op*)pEasyLink_cmdFs,
                pschParams_prop, 0, EASYLINK_RF_EVENT_MASK);
    }
    else
    {
        /* Run command */
        result = RF_runCmd(*prfHandle, (RF_Op*)pEasyLink_cmdFs,
                RF_PriorityNormal, 0, EASYLINK_RF_EVENT_MASK);
    }

    if((result & RF_EventLastCmdDone) && (pEasyLink_cmdFs->status == DONE_OK))
    {
        status = EasyLink_Status_Success;
    }

#ifdef TI_RTOS
	Semaphore_post(busyMutex);
#endif
#ifdef RTOS_AL
	semaphore_release(&busyMutex);
#endif

    return status;
}

uint32_t EasyLink_getFrequency(void)
{
    uint32_t freq_khz;

    if ( (!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }

    freq_khz = pEasyLink_cmdFs->frequency * 1000000;
    freq_khz += ((((uint64_t)pEasyLink_cmdFs->fractFreq * 1000000)) / 65536);

    return (RF_SUB1GHZ_FREQ_COMP_GET(freq_khz));
}

EasyLink_Status EasyLink_setRfPower(int8_t i8TxPowerDbm)
{
    EasyLink_Status status = EasyLink_Status_Cmd_Error;

    if ( (!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }
    //Check and take the busyMutex

	#ifdef TI_RTOS
        if (Semaphore_pend(busyMutex, 0) == false)
	#endif
	#ifdef RTOS_AL
    	if (semaphore_trytotake_time(&busyMutex, 0) == false)
	#endif
    {
        return EasyLink_Status_Busy_Error;
    }

#if (defined CONFIG_CC1352R1F3RGZ)     || (defined CONFIG_CC1312R1F3RGZ)     || \
    (defined CONFIG_CC2652R1FRGZ)      || (defined CONFIG_CC2652R1FSIP)      || \
    (defined CONFIG_CC2652P1FSIP)      || (defined CONFIG_CC2652R7RGZ)       || \
	(defined CONFIG_CC1312R7RGZ)       || (defined CONFIG_CC1352P7RGZ)       || \
	(defined CONFIG_CC1312R1_LAUNCHXL) || (defined CONFIG_CC1352R1_LAUNCHXL) || \
    (defined LAUNCHXL_CC1352P1)        || (defined CONFIG_CC26X2R1_LAUNCHXL) || \
    (defined LAUNCHXL_CC1352P_4)       || (defined LAUNCHXL_CC1352P_2)       || \
    (defined CONFIG_LP_CC2652PSIP)     || (defined CONFIG_LP_CC2652RSIP)     || \
    (defined CONFIG_LP_CC1312R7)       || (defined LP_CC1352P7_4)            || \
	(defined LP_CC1352P7_1)            || (defined CONFIG_LP_CC2652R7)
	

    RF_TxPowerTable_Entry *rfPowerTable = NULL;
    RF_TxPowerTable_Value newValue;
    uint8_t rfPowerTableSize = 0;

    newValue = RF_TxPowerTable_findValue((RF_TxPowerTable_Entry *)rfSetting->RF_pTxPowerTable, i8TxPowerDbm);
    if(newValue.rawValue != RF_TxPowerTable_INVALID_VALUE)
    {
        // Found a valid entry
        rfPowerTable = (RF_TxPowerTable_Entry *)rfSetting->RF_pTxPowerTable;
        rfPowerTableSize = rfSetting->RF_txPowerTableSize;
    }
    else
    {
        //Release the busyMutex
		#ifdef TI_RTOS
        Semaphore_post(busyMutex);
		#endif
		#ifdef RTOS_AL
        semaphore_release(&busyMutex);
		#endif
        // Desired power is too low to be supported on this device
        return EasyLink_Status_Config_Error;
    }

    //if max power is requested then the CCFG_FORCE_VDDR_HH must be set in
    //the ccfg; this does not apply to 2.4GHz proprietary modes
#if (CCFG_FORCE_VDDR_HH != 0x1)
    if((newValue.paType == RF_TxPowerTable_DefaultPA) &&
       (i8TxPowerDbm == rfPowerTable[rfPowerTableSize-2].power) &&
       (pEasyLink_params->ui32ModType != EasyLink_Phy_2_4_100kbps2gfsk) &&
       (pEasyLink_params->ui32ModType != EasyLink_Phy_2_4_250kbps2gfsk))
    {
        //Release the busyMutex
#ifdef TI_RTOS
    	Semaphore_post(busyMutex);
	#endif
	#ifdef RTOS_AL
    	semaphore_release(&busyMutex);
	#endif
        // The desired power level is set to the maximum supported under the
        // default PA settings, but the boost mode (CCFG_FORCE_VDDR_HH) is not
        // turned on
        return EasyLink_Status_Config_Error;
    }
#else
    // dummy read to avoid build warnings
    if(rfPowerTable[rfPowerTableSize-2].power){}
#endif


    RF_Stat rfStatus = RF_setTxPower(*prfHandle, newValue);
    if(rfStatus == RF_StatSuccess)
    {
        status = EasyLink_Status_Success;
    }
    else
    {
        status = EasyLink_Status_Config_Error;
    }
#else

    rfc_CMD_SCH_IMM_t immOpCmd = {0};
    rfc_CMD_SET_TX_POWER_t cmdSetPower = {0};
    uint8_t txPowerIdx;

    immOpCmd.commandNo = CMD_SCH_IMM;
    immOpCmd.startTrigger.triggerType = TRIG_NOW;
    immOpCmd.startTrigger.pastTrig = 1;
    immOpCmd.startTime = 0;

    cmdSetPower.commandNo = CMD_SET_TX_POWER;

    if (i8TxPowerDbm < rfPowerTable[0].dbm)
    {
        i8TxPowerDbm = rfPowerTable[0].dbm;
    }
    else if (i8TxPowerDbm > rfPowerTable[rfPowerTableSize-1].dbm )
    {
        i8TxPowerDbm = rfPowerTable[rfPowerTableSize-1].dbm;
    }

    //if max power is requested then the CCFG_FORCE_VDDR_HH must be set in
    //the ccfg
#if (CCFG_FORCE_VDDR_HH != 0x1)
    if (i8TxPowerDbm == rfPowerTable[rfPowerTableSize-1].dbm)
    {
        //Release the busyMutex
    #ifdef TI_RTOS
    	Semaphore_post(busyMutex);
	#endif
	#ifdef RTOS_AL
    	semaphore_release(&busyMutex);
	#endif
        return EasyLink_Status_Config_Error;
    }
#endif

    for (txPowerIdx = 0; txPowerIdx < rfPowerTableSize; txPowerIdx++)
    {
        if (i8TxPowerDbm >= rfPowerTable[txPowerIdx].dbm)
        {
            cmdSetPower.txPower = rfPowerTable[txPowerIdx].txPower;
            EasyLink_cmdPropRadioSetup.setup.txPower = rfPowerTable[txPowerIdx].txPower;
        }
    }

    //point the Operational Command to the immediate set power command
    immOpCmd.cmdrVal = (uint32_t) &cmdSetPower;

    // Send command
    RF_CmdHandle cmd = RF_postCmd(*prfHandle, (RF_Op*)&immOpCmd,
                                  RF_PriorityNormal, 0, EASYLINK_RF_EVENT_MASK);

    RF_EventMask result = RF_pendCmd(*prfHandle, cmd, RF_EventLastCmdDone);

    if (result & RF_EventLastCmdDone)
    {
        status = EasyLink_Status_Success;
    }
#endif

    //Release the busyMutex
#ifdef TI_RTOS
	Semaphore_post(busyMutex);
#endif
#ifdef RTOS_AL
	semaphore_release(&busyMutex);
#endif

    return status;
}

EasyLink_Status EasyLink_getRfPower(int8_t *pi8TxPowerDbm)
{
    int8_t txPowerDbm = 0xff;

    if ( (!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }

#if (defined CONFIG_CC1352R1F3RGZ)     || (defined CONFIG_CC1312R1F3RGZ)     || \
    (defined CONFIG_CC2652R1FRGZ)      || (defined CONFIG_CC2652R1FSIP)      || \
    (defined CONFIG_CC2652P1FSIP)      || (defined CONFIG_CC1312R7RGZ)       || \
	(defined CONFIG_CC1352P7RGZ)       || (defined CONFIG_CC2652R7RGZ)       || \
	(defined CONFIG_CC1312R1_LAUNCHXL) || (defined CONFIG_CC1352R1_LAUNCHXL) || \
    (defined LAUNCHXL_CC1352P1)        || (defined CONFIG_CC26X2R1_LAUNCHXL) || \
    (defined LAUNCHXL_CC1352P_4)       || (defined LAUNCHXL_CC1352P_2)       || \
    (defined CONFIG_LP_CC2652PSIP)     || (defined CONFIG_LP_CC2652RSIP)     || \
    (defined CONFIG_LP_CC1312R7)       || (defined LP_CC1352P7_4)            || \
	(defined LP_CC1352P7_1)            || (defined CONFIG_LP_CC2652R7)      
	
	

    uint8_t rfPowerTableSize = 0;
    RF_TxPowerTable_Entry *rfPowerTable = NULL;
    RF_TxPowerTable_Value currValue = RF_getTxPower(*prfHandle);

    if(currValue.rawValue == RF_TxPowerTable_INVALID_VALUE)
    {
        // Value not valid
        return EasyLink_Status_Config_Error;
    }
    else
    {
        rfPowerTable = (RF_TxPowerTable_Entry *)rfSetting->RF_pTxPowerTable;
        rfPowerTableSize = rfSetting->RF_txPowerTableSize;
        txPowerDbm = RF_TxPowerTable_findPowerLevel(rfPowerTable, currValue);

        //if CCFG_FORCE_VDDR_HH is not set max power cannot be achieved; this
        //does not apply to 2.4GHz proprietary modes
#if (CCFG_FORCE_VDDR_HH != 0x1)
        if((currValue.paType == RF_TxPowerTable_DefaultPA) &&
           (txPowerDbm == rfPowerTable[rfPowerTableSize-2].power) &&
           (pEasyLink_params->ui32ModType != EasyLink_Phy_2_4_100kbps2gfsk) &&
           (pEasyLink_params->ui32ModType != EasyLink_Phy_2_4_250kbps2gfsk))
        {
            txPowerDbm = rfPowerTable[rfPowerTableSize-3].power;
        }
#else
        // dummy read to avoid build warnings
        if(rfPowerTable[rfPowerTableSize-2].power){}
#endif
    }
#else
    uint8_t txPowerIdx;
    for (txPowerIdx = 0; txPowerIdx < rfPowerTableSize; txPowerIdx++)
    {
        if (rfPowerTable[txPowerIdx].txPower == EasyLink_cmdPropRadioSetup.setup.txPower)
        {
            txPowerDbm = rfPowerTable[txPowerIdx].dbm;
            continue;
        }
    }

    //if CCFG_FORCE_VDDR_HH is not set max power cannot be achieved
#if (CCFG_FORCE_VDDR_HH != 0x1)
    if (txPowerDbm == rfPowerTable[rfPowerTableSize-1].dbm)
    {
        txPowerDbm = rfPowerTable[rfPowerTableSize-2].dbm;
    }
#endif
#endif

    *pi8TxPowerDbm = txPowerDbm;

    return EasyLink_Status_Success;
}

EasyLink_Status EasyLink_getRssi(int8_t *pi8Rssi)
{
    if((!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }

    *pi8Rssi = RF_getRssi(*prfHandle);

    return EasyLink_Status_Success;
}

EasyLink_Status EasyLink_getAbsTime(uint32_t *pui32AbsTime)
{
    if((!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }

    *pui32AbsTime = RF_getCurrentTime();

    return EasyLink_Status_Success;
}

EasyLink_Status EasyLink_transmit(EasyLink_TxPacket *txPacket)
{
    EasyLink_Status status = EasyLink_Status_Tx_Error;
    RF_CmdHandle cmdHdl;
    uint32_t cmdTime;

    pschParams_prop->startTime    = 0U;
    pschParams_prop->startType    = RF_StartNotSpecified;
    pschParams_prop->allowDelay   = RF_AllowDelayNone;
    pschParams_prop->endTime      = 0U;
    pschParams_prop->endType      = RF_EndNotSpecified;
    pschParams_prop->duration     = 0U;
    pschParams_prop->activityInfo = genActivityTableValue(EasyLink_Activity_Tx, (EasyLink_Priority)cmdPriority);

    if ( (!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }
    if (txPacket->len > EASYLINK_MAX_DATA_LENGTH)
    {
        return EasyLink_Status_Param_Error;
    }
    //Check and take the busyMutex
    	#ifdef TI_RTOS
            if (Semaphore_pend(busyMutex, 0) == false)
    	#endif
    	#ifdef RTOS_AL
        	if (semaphore_trytotake_time(&busyMutex, 0) == false)
    	#endif
    {
        return EasyLink_Status_Busy_Error;
    }

    if(useIeeeHeader)
    {
        uint16_t ieeeHdr = EASYLINK_IEEE_HDR_CREATE(EASYLINK_IEEE_HDR_CRC_2BYTE, EASYLINK_IEEE_HDR_WHTNG_EN, (txPacket->len + addrSize + sizeof(ieeeHdr)));
        hdrSize     = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_IEEE_HDR_NBITS);
        txBuffer[0] = (uint8_t)(ieeeHdr & 0x00FF);
        txBuffer[1] = (uint8_t)((ieeeHdr & 0xFF00) >> 8);
    }
    else
    {
        hdrSize     = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_PROP_HDR_NBITS);
        txBuffer[0] = txPacket->len + addrSize;
    }

    if(EASYLINK_USE_DEFAULT_ADDR)
    {
        // Use the default address defined in easylink_config.h
        memcpy(&txBuffer[hdrSize], &defaultAddr, addrSize);
    }
    else
    {
        // Use the address passed in from the application
        memcpy(&txBuffer[hdrSize], &(txPacket->dstAddr), addrSize);
    }

    memcpy(&txBuffer[addrSize + hdrSize], txPacket->payload, txPacket->len);

    //packet length to Tx includes address and length field
    pEasyLink_cmdPropTxAdv->pktLen = txPacket->len + addrSize + hdrSize;
    pEasyLink_cmdPropTxAdv->pPkt = txBuffer;

    cmdTime = calculateCmdTime(txPacket);

    if (txPacket->absTime != 0)
    {
        pEasyLink_cmdPropTxAdv->startTrigger.triggerType = TRIG_ABSTIME;
        pEasyLink_cmdPropTxAdv->startTrigger.pastTrig = 1;
        pEasyLink_cmdPropTxAdv->startTime = txPacket->absTime;
        pschParams_prop->startTime = txPacket->absTime;
        pschParams_prop->startType = RF_StartAbs;
        pschParams_prop->duration  = EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endTime   = pEasyLink_cmdPropTxAdv->startTime + EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endType   = RF_EndAbs;
    }
    else
    {
        pEasyLink_cmdPropTxAdv->startTrigger.triggerType = TRIG_NOW;
        pEasyLink_cmdPropTxAdv->startTrigger.pastTrig = 1;
        pEasyLink_cmdPropTxAdv->startTime = 0U;
        pschParams_prop->startTime = 0U;
        pschParams_prop->startType = RF_StartNotSpecified;
        pschParams_prop->duration  = EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endTime   = RF_getCurrentTime() + EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endType   = RF_EndAbs;
    }

    // Send packet
    if(rfModeMultiClient)
    {
        cmdHdl = RF_scheduleCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropTxAdv,
                    pschParams_prop, 0, EASYLINK_RF_EVENT_MASK);
    }
    else
    {
        cmdHdl = RF_postCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropTxAdv,
            RF_PriorityHigh, 0, EASYLINK_RF_EVENT_MASK);
    }

    // Wait for Command to complete
    RF_EventMask result = RF_pendCmd(*prfHandle, cmdHdl, EASYLINK_RF_EVENT_MASK);

    if (result == (RF_EventCmdCancelled | RF_EventCmdPreempted))
    {
        status = EasyLink_Status_Cmd_Rejected;
    }
    else if (result & RF_EventLastCmdDone)
    {
        status = EasyLink_Status_Success;
    }

    //Release the busyMutex
 #ifdef TI_RTOS
	Semaphore_post(busyMutex);
#endif
#ifdef RTOS_AL
	semaphore_release(&busyMutex);
#endif

    return status;
}

EasyLink_Status EasyLink_transmitAsync(EasyLink_TxPacket *txPacket, EasyLink_TxDoneCb cb)
{
    EasyLink_Status status = EasyLink_Status_Tx_Error;
    uint32_t cmdTime;

    pschParams_prop->startTime    = 0U;
    pschParams_prop->startType    = RF_StartNotSpecified;
    pschParams_prop->allowDelay   = RF_AllowDelayNone;
    pschParams_prop->endTime      = 0U;
    pschParams_prop->endType      = RF_EndNotSpecified;
    pschParams_prop->duration     = 0U;
    pschParams_prop->activityInfo = genActivityTableValue(EasyLink_Activity_Tx, (EasyLink_Priority)cmdPriority);

    //Check if not configure or already an Async command being performed
    if ( (!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }
    if (EasyLink_CmdHandle_isValid(asyncCmdHndl))
    {
        return EasyLink_Status_Busy_Error;
    }
    if (txPacket->len > EASYLINK_MAX_DATA_LENGTH)
    {
        return EasyLink_Status_Param_Error;
    }
    //Check and take the busyMutex
        #ifdef TI_RTOS
            if (Semaphore_pend(busyMutex, 0) == false){
    	#endif
    	#ifdef RTOS_AL
        	if (semaphore_trytotake_time(&busyMutex, 0) == false){
    	#endif

        return EasyLink_Status_Busy_Error;
       }

    //store application callback
    txCb = cb;

    if(useIeeeHeader)
    {
        uint16_t ieeeHdr = EASYLINK_IEEE_HDR_CREATE(EASYLINK_IEEE_HDR_CRC_2BYTE, EASYLINK_IEEE_HDR_WHTNG_EN, (txPacket->len + addrSize + sizeof(ieeeHdr)));
        hdrSize     = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_IEEE_HDR_NBITS);
        txBuffer[0] = (uint8_t)(ieeeHdr & 0x00FF);
        txBuffer[1] = (uint8_t)((ieeeHdr & 0xFF00) >> 8);
    }
    else
    {
        hdrSize     = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_PROP_HDR_NBITS);
        txBuffer[0] = txPacket->len + addrSize;
    }

    if(EASYLINK_USE_DEFAULT_ADDR)
    {
        // Use the default address defined in easylink_config.h
        memcpy(&txBuffer[hdrSize], &defaultAddr, addrSize);
    }
    else
    {
        // Use the address passed in from the application
        memcpy(&txBuffer[hdrSize], &(txPacket->dstAddr), addrSize);
    }
    memcpy(&txBuffer[addrSize + hdrSize], txPacket->payload, txPacket->len);
    //packet length to Tx includes address and length field
    pEasyLink_cmdPropTxAdv->pktLen = txPacket->len + addrSize + hdrSize;
    pEasyLink_cmdPropTxAdv->pPkt = txBuffer;

    cmdTime = calculateCmdTime(txPacket);

    if (txPacket->absTime != 0)
    {
        pEasyLink_cmdPropTxAdv->startTrigger.triggerType = TRIG_ABSTIME;
        pEasyLink_cmdPropTxAdv->startTrigger.pastTrig = 1;
        pEasyLink_cmdPropTxAdv->startTime = txPacket->absTime;
        pschParams_prop->startTime = txPacket->absTime;
        pschParams_prop->startType = RF_StartAbs;
        pschParams_prop->duration  = EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endTime   = pEasyLink_cmdPropTxAdv->startTime + EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endType   = RF_EndAbs;
    }
    else
    {
        pEasyLink_cmdPropTxAdv->startTrigger.triggerType = TRIG_NOW;
        pEasyLink_cmdPropTxAdv->startTrigger.pastTrig = 1;
        pEasyLink_cmdPropTxAdv->startTime = 0;
        pschParams_prop->startTime = 0U;
        pschParams_prop->startType = RF_StartNotSpecified;
        pschParams_prop->duration  = EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endTime   = RF_getCurrentTime() + EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endType   = RF_EndAbs;
    }

    // Send packet
    if(rfModeMultiClient)
    {
        asyncCmdHndl = RF_scheduleCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropTxAdv,
            pschParams_prop, txDoneCallback, EASYLINK_RF_EVENT_MASK);
    }
    else
    {
        asyncCmdHndl = RF_postCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropTxAdv,
            RF_PriorityHigh, txDoneCallback, EASYLINK_RF_EVENT_MASK);
    }

    if (EasyLink_CmdHandle_isValid(asyncCmdHndl))
    {
        status = EasyLink_Status_Success;
    }
    else
    {
        // Error occurred. Callback will not be called, release busyMutex.
    #ifdef TI_RTOS
    	Semaphore_post(busyMutex);
	#endif
	#ifdef RTOS_AL
    	semaphore_release(&busyMutex);
	#endif
    }
    //busyMutex will be released by the callback
    return status;
}

EasyLink_Status EasyLink_transmitCcaAsync(EasyLink_TxPacket *txPacket, EasyLink_TxDoneCb cb)
{
    EasyLink_Status status = EasyLink_Status_Tx_Error;
    uint32_t cmdTime;

    pschParams_prop->startTime    = 0U;
    pschParams_prop->startType    = RF_StartNotSpecified;
    pschParams_prop->allowDelay   = RF_AllowDelayNone;
    pschParams_prop->endTime      = 0U;
    pschParams_prop->endType      = RF_EndNotSpecified;
    pschParams_prop->duration     = 0U;
    pschParams_prop->activityInfo = genActivityTableValue(EasyLink_Activity_Tx, (EasyLink_Priority)cmdPriority);

    //Check if not configure or already an Async command being performed, or
    //if a random number generator function is not provided
    if ( (!configured) || suspended || (!getRN))
    {
        return EasyLink_Status_Config_Error;
    }
    if (EasyLink_CmdHandle_isValid(asyncCmdHndl))
    {
        return EasyLink_Status_Busy_Error;
    }
    if (txPacket->len > EASYLINK_MAX_DATA_LENGTH)
    {
        return EasyLink_Status_Param_Error;
    }
    //Check and take the busyMutex
       #ifdef TI_RTOS
            if (Semaphore_pend(busyMutex, 0) == false)
    	#endif
    	#ifdef RTOS_AL
        	if (semaphore_trytotake_time(&busyMutex, 0) == false)
    	#endif
    {
        return EasyLink_Status_Busy_Error;
    }

    //store application callback
    txCb = cb;

    if(useIeeeHeader)
    {
        uint16_t ieeeHdr = EASYLINK_IEEE_HDR_CREATE(EASYLINK_IEEE_HDR_CRC_2BYTE, EASYLINK_IEEE_HDR_WHTNG_EN, (txPacket->len + addrSize + sizeof(ieeeHdr)));
        hdrSize     = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_IEEE_HDR_NBITS);
        txBuffer[0] = (uint8_t)(ieeeHdr & 0x00FF);
        txBuffer[1] = (uint8_t)((ieeeHdr & 0xFF00) >> 8);
    }
    else
    {
        hdrSize     = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_PROP_HDR_NBITS);
        txBuffer[0] = txPacket->len + addrSize;
    }

    if(EASYLINK_USE_DEFAULT_ADDR)
    {
        // Use the default address defined in easylink_config.h
        memcpy(&txBuffer[hdrSize], &defaultAddr, addrSize);
    }
    else
    {
        // Use the address passed in from the application
        memcpy(&txBuffer[hdrSize], &(txPacket->dstAddr), addrSize);
    }

    memcpy(&txBuffer[addrSize + hdrSize], txPacket->payload, txPacket->len);

    // Set the Carrier Sense command attributes
    // Chain the TX command to run after the CS command
    pEasyLink_cmdPropCs->pNextOp        = (rfc_radioOp_t *)pEasyLink_cmdPropTxAdv;

    //packet length to Tx includes address and length field
    pEasyLink_cmdPropTxAdv->pktLen = txPacket->len + addrSize + hdrSize;
    pEasyLink_cmdPropTxAdv->pPkt = txBuffer;

    cmdTime = calculateCmdTime(txPacket);

    if (txPacket->absTime != 0)
    {
        pEasyLink_cmdPropCs->startTrigger.triggerType = TRIG_ABSTIME;
        pEasyLink_cmdPropCs->startTrigger.pastTrig = 1;
        pEasyLink_cmdPropCs->startTime = txPacket->absTime;
        pschParams_prop->startTime  = txPacket->absTime;
        pschParams_prop->startType  = RF_StartAbs;
        pschParams_prop->duration   = EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endTime    = pEasyLink_cmdPropCs->startTime + EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endType    = RF_EndAbs;
    }
    else
    {
        pEasyLink_cmdPropCs->startTrigger.triggerType = TRIG_NOW;
        pEasyLink_cmdPropCs->startTrigger.pastTrig = 1;
        pEasyLink_cmdPropCs->startTime = 0;
        pschParams_prop->startTime  = 0;
        pschParams_prop->startType  = RF_StartNotSpecified;
        pschParams_prop->duration   = EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endTime    = RF_getCurrentTime() + EasyLink_ms_To_RadioTime(cmdTime);
        pschParams_prop->endType    = RF_EndAbs;
    }

    // Check for a clear channel (CCA) before sending a packet
    if(rfModeMultiClient)
    {
        asyncCmdHndl = RF_scheduleCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropCs,
            pschParams_prop, ccaDoneCallback, EASYLINK_RF_EVENT_MASK);
    }
    else
    {
        asyncCmdHndl = RF_postCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropCs,
            RF_PriorityHigh, ccaDoneCallback, EASYLINK_RF_EVENT_MASK);
    }

    if (EasyLink_CmdHandle_isValid(asyncCmdHndl))
    {
        status = EasyLink_Status_Success;
    }
    else
    {
        // Error occurred. Callback will not be called, release busyMutex.
    #ifdef TI_RTOS
    	Semaphore_post(busyMutex);
	#endif
	#ifdef RTOS_AL
    	semaphore_release(&busyMutex);
	#endif
    }

    //busyMutex will be released by the callback

    return status;
}

EasyLink_Status EasyLink_receive(EasyLink_RxPacket *rxPacket)
{
    EasyLink_Status status = EasyLink_Status_Rx_Error;
    RF_EventMask result;
    rfc_dataEntryGeneral_t *pDataEntry;
    RF_CmdHandle rx_cmd;

    pschParams_prop->startTime    = 0U;
    pschParams_prop->startType    = RF_StartNotSpecified;
    pschParams_prop->allowDelay   = RF_AllowDelayNone;
    pschParams_prop->endTime      = 0U;
    pschParams_prop->endType      = RF_EndNotSpecified;
    pschParams_prop->duration     = 0U;
    pschParams_prop->activityInfo = genActivityTableValue(EasyLink_Activity_Rx, (EasyLink_Priority)cmdPriority);

    if ( (!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }
    //Check and take the busyMutex
       #ifdef TI_RTOS
            if (Semaphore_pend(busyMutex, 0) == false)
    	#endif
    	#ifdef RTOS_AL
        	if (semaphore_trytotake_time(&busyMutex, 0) == false)
    	#endif
    {
        return EasyLink_Status_Busy_Error;
    }

    pDataEntry = (rfc_dataEntryGeneral_t*) rxBuffer;
    //data entry rx buffer includes hdr (len-1Byte), addr (max 8Bytes) and data
    pDataEntry->length = 1 + EASYLINK_MAX_ADDR_SIZE + EASYLINK_MAX_DATA_LENGTH;
    pDataEntry->status = 0;
    dataQueue.pCurrEntry = (uint8_t*)pDataEntry;
    dataQueue.pLastEntry = NULL;
    pEasyLink_cmdPropRxAdv->pQueue = &dataQueue;               /* Set the Data Entity queue for received data */
    pEasyLink_cmdPropRxAdv->pOutput = (uint8_t*)&rxStatistics;

    if (rxPacket->absTime != 0)
    {
        pEasyLink_cmdPropRxAdv->startTrigger.triggerType = TRIG_ABSTIME;
        pEasyLink_cmdPropRxAdv->startTrigger.pastTrig = 1;
        pEasyLink_cmdPropRxAdv->startTime = rxPacket->absTime;
        pschParams_prop->startTime  = rxPacket->absTime;
        pschParams_prop->startType  = RF_StartAbs;
    }
    else
    {
        pEasyLink_cmdPropRxAdv->startTrigger.triggerType = TRIG_NOW;
        pEasyLink_cmdPropRxAdv->startTrigger.pastTrig = 1;
        pEasyLink_cmdPropRxAdv->startTime = 0;
        pschParams_prop->startTime  = 0U;
        pschParams_prop->startType  = RF_StartNotSpecified;
    }

    if (rxPacket->rxTimeout != 0)
    {
        pEasyLink_cmdPropRxAdv->endTrigger.triggerType = TRIG_ABSTIME;
        pEasyLink_cmdPropRxAdv->endTrigger.pastTrig = 1;
        pEasyLink_cmdPropRxAdv->endTime = RF_getCurrentTime() + rxPacket->rxTimeout;
        pschParams_prop->endTime    = pEasyLink_cmdPropRxAdv->endTime;
        pschParams_prop->endType    = RF_EndAbs;
    }
    else
    {
        pEasyLink_cmdPropRxAdv->endTrigger.triggerType = TRIG_NEVER;
        pEasyLink_cmdPropRxAdv->endTrigger.pastTrig = 1;
        pEasyLink_cmdPropRxAdv->endTime = 0;
        pschParams_prop->endTime    = 0U;
        pschParams_prop->endType    = RF_EndNotSpecified;
    }

    //Clear the Rx statistics structure
    memset(&rxStatistics, 0, sizeof(rfc_propRxOutput_t));

    if(rfModeMultiClient)
    {
        rx_cmd = RF_scheduleCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropRxAdv,
                    pschParams_prop, 0, EASYLINK_RF_EVENT_MASK);
    }
    else
    {
        rx_cmd = RF_postCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropRxAdv,
            RF_PriorityHigh, 0, EASYLINK_RF_EVENT_MASK);
    }

    /* Wait for Command to complete */
    result = RF_pendCmd(*prfHandle, rx_cmd, RF_EventLastCmdDone);

    if (result & RF_EventLastCmdDone)
    {
        //Check command status
        if (pEasyLink_cmdPropRxAdv->status == PROP_DONE_OK)
        {
            //Check that data entry status indicates it is finished with
            if (pDataEntry->status != DATA_ENTRY_FINISHED)
            {
                status = EasyLink_Status_Rx_Error;
            }
            //check Rx Statistics
            else if ( (rxStatistics.nRxOk == 1) ||
                     //or  filer disabled and ignore due to addr mistmatch
                     ((pEasyLink_cmdPropRxAdv->pktConf.filterOp == 1) &&
                      (rxStatistics.nRxIgnored == 1)) )
            {
                //copy length from pDataEntry (- addrSize)
                rxPacket->len = *(uint8_t*)(&pDataEntry->data) - addrSize;
                if(useIeeeHeader)
                {
                    hdrSize = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_IEEE_HDR_NBITS);
                }
                else
                {
                    hdrSize = EASYLINK_HDR_SIZE_NBYTES(EASYLINK_PROP_HDR_NBITS);
                }
                //copy address
                memcpy(&(rxPacket->dstAddr), (&pDataEntry->data + hdrSize), addrSize);
                //copy payload
                memcpy(&rxPacket->payload, (&pDataEntry->data + hdrSize + addrSize), (rxPacket->len));
                rxPacket->rssi = rxStatistics.lastRssi;

                status = EasyLink_Status_Success;
                rxPacket->absTime = rxStatistics.timeStamp;
            }
            else if ( rxStatistics.nRxBufFull == 1)
            {
                status = EasyLink_Status_Rx_Buffer_Error;
            }
            else if ( rxStatistics.nRxStopped == 1)
            {
                status = EasyLink_Status_Aborted;
            }
            else
            {
                status = EasyLink_Status_Rx_Error;
            }
        }
        else if ( pEasyLink_cmdPropRxAdv->status == PROP_DONE_RXTIMEOUT)
        {
            status = EasyLink_Status_Rx_Timeout;
        }
        else
        {
            status = EasyLink_Status_Rx_Error;
        }
    }
    else if (result == (RF_EventCmdCancelled | RF_EventCmdPreempted))
    {
        status = EasyLink_Status_Cmd_Rejected;
    }

    //Release the busyMutex
#ifdef TI_RTOS
	Semaphore_post(busyMutex);
#endif
#ifdef RTOS_AL
	semaphore_release(&busyMutex);
#endif

    return status;
}

EasyLink_Status EasyLink_receiveAsync(EasyLink_ReceiveCb cb, uint32_t absTime)
{
    EasyLink_Status status = EasyLink_Status_Rx_Error;
    rfc_dataEntryGeneral_t *pDataEntry;

    pschParams_prop->startTime    = 0U;
    pschParams_prop->startType    = RF_StartNotSpecified;
    pschParams_prop->allowDelay   = RF_AllowDelayNone;
    pschParams_prop->endTime      = 0U;
    pschParams_prop->endType      = RF_EndNotSpecified;
    pschParams_prop->duration     = 0U;
    pschParams_prop->activityInfo = genActivityTableValue(EasyLink_Activity_Rx, (EasyLink_Priority)cmdPriority);

    //Check if not configure of already an Async command being performed
    if ( (!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }
    if (EasyLink_CmdHandle_isValid(asyncCmdHndl))
    {
        return EasyLink_Status_Busy_Error;
    }
    //Check and take the busyMutex
        #ifdef TI_RTOS
            if (Semaphore_pend(busyMutex, 0) == false)
    	#endif
    	#ifdef RTOS_AL
        	if (semaphore_trytotake_time(&busyMutex, 0) == false)
    	#endif
    {
        return EasyLink_Status_Busy_Error;
    }

    rxCb = cb;

    pDataEntry = (rfc_dataEntryGeneral_t*) rxBuffer;
    //data entry rx buffer includes hdr (len-1Byte), addr (max 8Bytes) and data
    pDataEntry->length = 1 + EASYLINK_MAX_ADDR_SIZE + EASYLINK_MAX_DATA_LENGTH;
    pDataEntry->status = 0;
    dataQueue.pCurrEntry = (uint8_t*)pDataEntry;
    dataQueue.pLastEntry = NULL;
    pEasyLink_cmdPropRxAdv->pQueue = &dataQueue;               /* Set the Data Entity queue for received data */
    pEasyLink_cmdPropRxAdv->pOutput = (uint8_t*)&rxStatistics;

    if (absTime != 0)
    {
        pEasyLink_cmdPropRxAdv->startTrigger.triggerType = TRIG_ABSTIME;
        pEasyLink_cmdPropRxAdv->startTrigger.pastTrig = 1;
        pEasyLink_cmdPropRxAdv->startTime = absTime;
        pschParams_prop->startTime  = absTime;
        pschParams_prop->startType  = RF_StartAbs;
    }
    else
    {
        pEasyLink_cmdPropRxAdv->startTrigger.triggerType = TRIG_NOW;
        pEasyLink_cmdPropRxAdv->startTrigger.pastTrig = 1;
        pEasyLink_cmdPropRxAdv->startTime = 0;
        pschParams_prop->startTime  = 0U;
        pschParams_prop->startType  = RF_StartNotSpecified;
    }

    if (asyncRxTimeOut != 0)
    {
        pEasyLink_cmdPropRxAdv->endTrigger.triggerType = TRIG_ABSTIME;
        pEasyLink_cmdPropRxAdv->endTrigger.pastTrig = 1;
        pEasyLink_cmdPropRxAdv->endTime = RF_getCurrentTime() + asyncRxTimeOut;
        pschParams_prop->endTime    = pEasyLink_cmdPropRxAdv->endTime;
        pschParams_prop->endType    = RF_EndAbs;
    }
    else
    {
        pEasyLink_cmdPropRxAdv->endTrigger.triggerType = TRIG_NEVER;
        pEasyLink_cmdPropRxAdv->endTrigger.pastTrig = 1;
        pEasyLink_cmdPropRxAdv->endTime = 0;
        pschParams_prop->endTime    = 0U;
        pschParams_prop->endType    = RF_EndNotSpecified;
    }

    //Clear the Rx statistics structure
    memset(&rxStatistics, 0, sizeof(rfc_propRxOutput_t));

    if(rfModeMultiClient)
    {
        asyncCmdHndl = RF_scheduleCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropRxAdv,
                    pschParams_prop, rxDoneCallback, EASYLINK_RF_EVENT_MASK);
    }
    else
    {
        asyncCmdHndl = RF_postCmd(*prfHandle, (RF_Op*)pEasyLink_cmdPropRxAdv,
            RF_PriorityHigh, rxDoneCallback, EASYLINK_RF_EVENT_MASK);
    }

    if (EasyLink_CmdHandle_isValid(asyncCmdHndl))
    {
        status = EasyLink_Status_Success;
    }
    else
    {
        //Callback will not be called, release the busyMutex
    #ifdef TI_RTOS
    	Semaphore_post(busyMutex);
	#endif
	#ifdef RTOS_AL
    	semaphore_release(&busyMutex);
	#endif

    }

    //busyMutex will be released in callback

    return status;
}

EasyLink_Status EasyLink_abort(void)
{
    EasyLink_Status status = EasyLink_Status_Cmd_Error;

    if ( (!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }
    //check an Async command is running, if not return success
    if (!EasyLink_CmdHandle_isValid(asyncCmdHndl))
    {
        return EasyLink_Status_Aborted;
    }

    //force abort (graceful param set to 0)
    if (RF_cancelCmd(*prfHandle, asyncCmdHndl, 0) == RF_StatSuccess)
    {
        /* If command is cancelled immediately, callback may have set the cmd handle to invalid.
         * In that case, no need to pend.
         */
        if (EasyLink_CmdHandle_isValid(asyncCmdHndl))
        {
            /* Wait for Command to complete */
            RF_EventMask result = RF_pendCmd(*prfHandle, asyncCmdHndl, (RF_EventLastCmdDone |
                    RF_EventCmdAborted | RF_EventCmdCancelled | RF_EventCmdStopped));
            if (result & RF_EventLastCmdDone)
            {
                status = EasyLink_Status_Success;
            }
        }
        else
        {
            /* Command already cancelled */
            status = EasyLink_Status_Success;
        }
    }
    else
    {
        status = EasyLink_Status_Cmd_Error;
    }

    return status;
}

EasyLink_Status EasyLink_enableRxAddrFilter(uint8_t* pui8AddrFilterTable, uint8_t ui8AddrSize, uint8_t ui8NumAddrs)
{
    EasyLink_Status status = EasyLink_Status_Param_Error;

    if ( (!configured) || suspended)
    {
        return EasyLink_Status_Config_Error;
    }
    if (isAddrSizeValid(ui8AddrSize) == false)
    {
       return EasyLink_Status_Param_Error;
    }
        #ifdef TI_RTOS
            if (Semaphore_pend(busyMutex, 0) == false)
    	#endif
    	#ifdef RTOS_AL
        	if (semaphore_trytotake_time(&busyMutex, 0) == false)
    	#endif
    {
        return EasyLink_Status_Busy_Error;
    }

    if ( (pui8AddrFilterTable != NULL) &&
         (ui8AddrSize != 0) && (ui8NumAddrs != 0) &&
         (ui8AddrSize == addrSize) &&
         (ui8NumAddrs <= EASYLINK_MAX_ADDR_FILTERS) )
    {
        memcpy(addrFilterTable, pui8AddrFilterTable, ui8AddrSize * ui8NumAddrs);
        pEasyLink_cmdPropRxAdv->addrConf.addrSize = ui8AddrSize;
        pEasyLink_cmdPropRxAdv->addrConf.numAddr = ui8NumAddrs;
        pEasyLink_cmdPropRxAdv->pktConf.filterOp = 0;

        status = EasyLink_Status_Success;
    }
    else if (pui8AddrFilterTable == NULL)
    {
        //disable filter
        pEasyLink_cmdPropRxAdv->pktConf.filterOp = 1;

        status = EasyLink_Status_Success;
    }

    //Release the busyMutex
 #ifdef TI_RTOS
	Semaphore_post(busyMutex);
#endif
#ifdef RTOS_AL
	semaphore_release(&busyMutex);
#endif

    return status;
}

EasyLink_Status EasyLink_setCtrl(EasyLink_CtrlOption Ctrl, uint32_t ui32Value)
{
    EasyLink_Status status = EasyLink_Status_Param_Error;
    switch(Ctrl)
    {
        case EasyLink_Ctrl_AddSize:
            if (isAddrSizeValid(ui32Value))
            {
                addrSize = (uint8_t) ui32Value;
                pEasyLink_cmdPropRxAdv->addrConf.addrSize = addrSize;
                status = EasyLink_Status_Success;
            }
            break;
        case EasyLink_Ctrl_Idle_TimeOut:
            if(configured)
            {
                // Client has already been configured, need to modify
                // the client configuration directly
                RF_control(*prfHandle, RF_CTRL_SET_INACTIVITY_TIMEOUT, &ui32Value);
            }
            else
            {
                // RF_open() in EasyLink_init() has not been called yet so it
                // is safe to alter the rfParams
            	inactivityTimeout = ui32Value;
            }

            status = EasyLink_Status_Success;
            break;
        case EasyLink_Ctrl_MultiClient_Mode:
            rfModeMultiClient = (bool) ui32Value;
            status = EasyLink_Status_Success;
            break;
        case EasyLink_Ctrl_AsyncRx_TimeOut:
            asyncRxTimeOut = ui32Value;
            status = EasyLink_Status_Success;
            break;
        case EasyLink_Ctrl_Cmd_Priority:
            if(ui32Value < EasyLink_Priority_NEntries)
            {
                cmdPriority = (EasyLink_Priority)ui32Value;
                status = EasyLink_Status_Success;
            }
            break;
        case EasyLink_Ctrl_Test_Tone:
            status = enableTestMode(EasyLink_Ctrl_Test_Tone);
            break;
        case EasyLink_Ctrl_Test_Signal:
            status = enableTestMode(EasyLink_Ctrl_Test_Signal);
            break;
        case EasyLink_Ctrl_Rx_Test_Tone:
            status = enableTestMode(EasyLink_Ctrl_Rx_Test_Tone);
            break;
    }

    return status;
}

EasyLink_Status EasyLink_getCtrl(EasyLink_CtrlOption Ctrl, uint32_t* pui32Value)
{
    EasyLink_Status status = EasyLink_Status_Cmd_Error;

    switch(Ctrl)
    {
        case EasyLink_Ctrl_AddSize:
            *pui32Value = addrSize;
            status = EasyLink_Status_Success;
            break;
        case EasyLink_Ctrl_Idle_TimeOut:
            *pui32Value = prfParams->nInactivityTimeout;
            status = EasyLink_Status_Success;
            break;
        case EasyLink_Ctrl_MultiClient_Mode:
            *pui32Value = (uint32_t) rfModeMultiClient;
            status = EasyLink_Status_Success;
            break;
        case EasyLink_Ctrl_AsyncRx_TimeOut:
            *pui32Value = asyncRxTimeOut;
            status = EasyLink_Status_Success;
            break;
        case EasyLink_Ctrl_Cmd_Priority:
            *pui32Value = cmdPriority;
            status = EasyLink_Status_Success;
            break;
        case EasyLink_Ctrl_Test_Tone:
        case EasyLink_Ctrl_Test_Signal:
        case EasyLink_Ctrl_Rx_Test_Tone:
            *pui32Value = 0;
            status = EasyLink_Status_Success;
            break;
    }

    return status;
}

EasyLink_Status EasyLink_getIeeeAddr(uint8_t *ieeeAddr)
{
    EasyLink_Status status = EasyLink_Status_Param_Error;

    if (ieeeAddr != NULL)
    {
        int i;

        //Reading from primary IEEE location...
        uint8_t *location = (uint8_t *)EASYLINK_PRIMARY_IEEE_ADDR_LOCATION;

        /*
         * ...unless we can find a byte != 0xFF in secondary
         *
         * Intentionally checking all 8 bytes here instead of len, because we
         * are checking validity of the entire IEEE address irrespective of the
         * actual number of bytes the caller wants to copy over.
         */
        for (i = 0; i < 8; i++) {
            if (((uint8_t *)EASYLINK_SECONDARY_IEEE_ADDR_LOCATION)[i] != 0xFF) {
                //A byte in the secondary location is not 0xFF. Use the
                //secondary
                location = (uint8_t *)EASYLINK_SECONDARY_IEEE_ADDR_LOCATION;
                break;
            }
        }

        //inverting byte order
        for (i = 0; i < 8; i++) {
            ieeeAddr[i] = location[8 - 1 - i];
        }

        status = EasyLink_Status_Success;
    }

    return status;
}
