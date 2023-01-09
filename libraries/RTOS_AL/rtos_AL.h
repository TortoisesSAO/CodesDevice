/*
 * freertos_custom.h
 *
 *  Created on: 11 nov. 2021
 *      Author: Andres Oliva Trevisan
 *      Institution: Instituto Balseiro
 *      Location: San Carlos de Bariloche Bariloche, Argentina
 *      LinkedIn:https://www.linkedin.com/in/andr%C3%A9s-oliva-trevisan-833561165/
 */

#ifndef RTOS_AL_H_
#define RTOS_AL_H_

//**************************************************************************************
//**************************************************************************************
#include "rtos_AL_struct_def.h"

#include <stdio.h>// to perform multiplication
//******STRUCTURES FOR SYSTEM AND SCHEDULER HANDLING************************************
#ifdef RTOS_TIRTOS
 #include <ti/drivers/dpl/HwiP.h> //TI RTOS INTERRUPTIONS
 #include <ti/sysbios/knl/Clock.h> //TI RTOS TO ACCES CLOCK
 #include <ti/sysbios/knl/Task.h> //TI RTOSto acces task sleep fro
#endif
#ifdef RTOS_SSS
 #include <RTOS_AL/freertos_slackstealing/slack.h>
#endif
/*
 */
#ifdef __cplusplus
  extern "C" {
#endif
/*Functions*/
#ifndef RTOS_FREERTOS
 void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement);
#endif

int thread_create ( TaskData * task_data);
int thread_destroy(TaskData * task_data);

#ifdef RTOS_EAT_CPU
 extern void eatCpu (uint32_t ticks);
#endif

//******SPECIFIC SCHEDULERS STRCUTURES************************************
#ifdef RTOS_DP
 typedef struct{
	uint8_t lowband;
	uint8_t highband;
 }TaskDualPriority;
 static TaskDualPriority taskdualprioritys ={.lowband=1,.highband=2};
#endif

#define mainMAX_MSG_LEN                     25
static char cMessage[ mainMAX_MSG_LEN ];//used for debug and task creation

#ifdef RTOS_SS_DEBUG
 #define MUTEX_TIMEOUT                       10
 static int32_t slackArray[ 7 ];
 static void vPrintSlacks( char *buf, char s, int32_t * slackArray, TickType_t xCur );
#endif

#ifdef RTOS_DP
 void vApplicationTickHook (void);
#endif


#ifdef __cplusplus
   }
 #endif

#endif
