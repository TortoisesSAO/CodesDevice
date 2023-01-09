/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
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

/*
 *  ======== rfEasyLinkEchoRx.c ========
 */
/* Standard C Libraries */
#include <stdlib.h>
/* BIOS Header files */
#include <RTOS_AL/rtos_AL.h> //allow the use of an operative system
/* TI-RTOS Header files */
#include <ti_radio_config.h>
#include "ti_drivers_config.h"
/* Application Header files */
#include "easylink/EasyLink.h"


/* Undefine to not use async mode */

#define RFEASYLINKECHO_PAYLOAD_LENGTH     5 //fixed at 5, send 5 bytes ofdata values

static uint16_t time_to_recieve;
EasyLink_TxPacket txPacket = {{0}, 0, 0, {0}};

extern TaskData* task_data_p[2];

//extern "C"	{
extern void task_rfsub1ghz_ka(uint8_t task_index);
//---------------------------------------------
 void task_rfsub1ghz_ka(uint8_t task_index)
{   //settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t clocktick_start,clocktick_end;
	//configuring devices to use
	int8_t samples_to_add;uint32_t absTime;
    EasyLink_RxPacket rxPacket = {{0}, 0, 0, 0, 0, {0}};
    // Initialize the EasyLink parameters to their default values
    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);
    /*
     * Initialize EasyLink with the settings found in ti_easylink_config.h
     * Modify EASYLINK_PARAM_CONFIG in ti_easylink_config.h to change the default
     * PHY
     */
    if(EasyLink_init(&easyLink_params) != EasyLink_Status_Success) {while (1);}
     EasyLink_setFrequency(150000000);//to change the frequenncy
     EasyLink_setRfPower(9);
    /*If you wish to use a frequency other than the default, use
     * the following API: EasyLink_setFrequency(868000000); */
    // Packet Originator
     uint8_t i;
	 for (;;){
        /* Create packet with incrementing sequence number and random payload */
        for (i = 0; i < RFEASYLINKECHO_PAYLOAD_LENGTH; i++)
        {   txPacket.payload[i] = 255;}//write all ones in binary
        txPacket.len = RFEASYLINKECHO_PAYLOAD_LENGTH;
        /* Set Tx absolute time to current time + 1000ms */
        if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
        {while(1);}// Problem getting absolute time, only for debugg pourpose
        txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(14);
        EasyLink_Status result = EasyLink_transmit(&txPacket);
        //-------------task kP
        vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
         }
}


//power policy libraries (non used, just to be saved if you wish to create your own power policy)
//#include <ti/drivers/Power.h> //power policy
//#include <ti/drivers/power/PowerCC26XX.h> //power policy defin es
//#include <PowerCC26x2_tirtos.h>
//PowerCC26XX_standbyPolicy
//extern void PowerCC26XX_standbyPolicy2(void);

//PowerCC26XX_sleepPolicy
