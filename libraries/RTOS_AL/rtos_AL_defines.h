/*
 * freertos_custom_defines.h
 *
 *  Created on: 7 dic. 2021
 *      Author: Andres Oliva Trevisan
 */

#ifndef RTOS_AL_DEFINES_H_

#define RTOS_AL_DEFINES_H_
#include "rtos_AL_config.h"
#include <stdint.h>//for integer declaration
//**********/*GENERAL OS DEFINES**********
#ifndef RTOS_FREERTOS
 typedef void (*TaskFunction_t)( void * ); //for using this
 #define TaskFunction_t TaskFunction_t     //TaskFunction_t
 #define TickType_t uint32_t               //TickType_t //for freertos
 #define UBaseType_t unsigned long         //UBaseType_tfor freertos
 #define BaseType_t long                  // BaseType_t //for freertos
 #define BoolType_t uint8_t               // u_int8_t forfreertos
 #define	pdFALSE	0                        //pfFALSE for freertos
 #define	pdTRUE	1                        //pfTRUE for freertos
//functions definitions

 #if defined(RTOS_POSIX_AND_OTHER)|defined(RTOS_POSIX)
  #include <pthread.h> //POSIX thread handle
  #define TaskHandle_t pthread_t//TaskHandle_t is the handle of the thread
 #endif

 #ifndef RTOS_POSIX_AND_OTHER
  #ifdef RTOS_TIRTOS
  #include <ti/sysbios/knl/Task.h>
  #define TaskHandle_t Task_Handle//TaskHandle_t is the handle of the thread
  #endif
 #endif

 #ifdef RTOS_TIRTOS //TimestampProvider_get64
  #include "ti_sysbios_config.h"
  #define configMAX_PRIORITIES       Task_numPriorities_D
  #define configMINIMAL_STACK_SIZE   Task_defaultStackSize_D
  #define CLOCK_TICK_UNIT            1000000  //clock tick is expressed in micro seconds in this app)
  #define config_tick_rate_HZ        (TickType_t)(CLOCK_TICK_UNIT/Clock_tickPeriod)
  #define vTaskDelay                 Task_sleep
  #define xTaskGetTickCount()        Clock_getTicks()    //get clock tick counts
  #define vTaskResumeAll()           Task_enable()       //enable scheduller
  #define vTaskSuspendAll()          Task_disable()      //disable scheduller
  #define taskENABLE_INTERRUPTS()    HwiP_disable()      //enable  all hardware & software interrupts
  #define taskDISABLE_INTERRUPTS()   HwiP_enable()       //disable all hardware & software interrupts
 #endif

#endif
//Task_restore()+
//extern  uintptr_t hwi_key;         //NOTE, THE IN THIS SYSTEM WORKS THIS
                                   //SHOULD ONLY BE CALLED 1 TIME
//macros
//this macros are defined to work in

#if defined(RTOS_TIRTOS)|defined(RTOS_FREERTOS)                   //USING TI_RTOS
//units are expressed in microseconds
/* Convert seconds to tick periods*/
 #define SEC_TO(s)              (uint32_t)(((s )*(1000000))/Clock_tickPeriod)
 #define MILSEC_TO(ms)          (uint32_t)(((ms )*(1000)/(Clock_tickPeriod))
 //as ticks are the normal number, do not perform any convertion
 #define TICK_TO(tick)                tick

 #define TO_SEC(tick)          ((float)(tick*Clock_tickPeriod/CLOCK_TICK_UNIT))
#endif


//struct xLIST_ITEM{
//	listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE			//< Set to a known value if configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1.
//	configLIST_VOLATILE RTOSTickType_t xItemValue;		//< The value being listed.  In most cases this is used to sort the list in descending order.
//	struct xLIST_ITEM * configLIST_VOLATILE pxNext;		//< Pointer to the next ListItem_t in the list.
//	struct xLIST_ITEM * configLIST_VOLATILE pxPrevious;	//< Pointer to the previous ListItem_t in the list.
//	void * pvOwner;										//< Pointer to the object (normally a TCB) that contains the list item.  There is therefore a two way link between the object containing the list item and the list item itself.
//	void * configLIST_VOLATILE pvContainer;				//< Pointer to the list in which this list item is placed (if any).
//	listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE			//< Set to a known value if configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1.
//};
//typedef struct xLIST_ITEM ListItem_t;					// For some reason lint wants this as two separate definitions.

#endif /* RTOS_AL_DEFINES_H_*/
