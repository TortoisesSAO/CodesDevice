/*
 * freertos_custom.c
 *
 *  Created on: 11 nov. 2021
 *      Author: Andres Oliva Trevisan
 */
/*
 * Creates all the task, sempharore and anything needed
*/
//posix librarys
#include "rtos_AL.h"

#ifdef RTOS_PRINT_DEBUG
#include <driver_custom/driverdebug.h>
#endif

/*
 * This a custom version of vTaskDelayUntil for TI-RTOS/Posix
 * Notes:TI-RTOS has time slicing DISABLED for default
 * once you call  "(vTaskResumeAll() TI RTOS equivalent)"
 * this allows you to safely compare time against RTOS CLOCK
 * then at last you call for system sleep to move the task from running
 * to blocked. When task end their sleep time, it will become READY
 * and the RTOS will choose if executing the task based on your priority set up
 * Or schedulling scheme used
 */
//general use
//----------------------------------
int thread_create(TaskData* task_data){
int  retc;
#if defined(RTOS_POSIX_AND_OTHER)|defined(RTOS_POSIX)
	pthread_t           thread;
	pthread_attr_t      attrs;
	struct sched_param  priParam;
	pthread_attr_init(&attrs);
	retc = pthread_attr_setschedparam(&attrs, &priParam);
	retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    //if (retc != 0){while (1) {}}/* failed to set attributes */
    retc = pthread_create(&thread, &attrs, task_data->pxTaskCode, NULL);
    //if (retc != 0){while (1) {}}/* pthread_create() failed */
    task_data->handle=thread;
#endif
return(retc);
}
//----------------------------------
int thread_destroy(TaskData * task_data){
int retc;

#if defined(RTOS_POSIX_AND_OTHER)|defined(RTOS_POSIX)
retc=pthread_detach(task_data->handle);
#endif

return(retc);
}

//Task delay until
#ifndef RTOS_FREERTOS //If no free rtos is defined
void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement){
TickType_t xTimeToWake,xTimeTodelay;
vTaskSuspendAll();//suspend all tasks execution
xTimeToWake=xTaskGetTickCount();//get the current clock tick
xTimeTodelay=0;//initial set up
if(xTimeToWake>(*pxPreviousWakeTime) ){//check if the deadline of task has passes
	xTimeTodelay=xTimeToWake - *pxPreviousWakeTime;
	}//then sets the time-to-delay value
if (xTimeIncrement>xTimeTodelay){//checks if it is need to delay
	xTimeTodelay=xTimeIncrement-xTimeTodelay;//updates delay time
    *pxPreviousWakeTime=xTimeToWake + xTimeTodelay;//update the counter of previous wake up time
	vTaskResumeAll();//never put this after task sleep
	vTaskDelay(xTimeTodelay);//task changes from running to bloqued
   }
else{*pxPreviousWakeTime=xTaskGetTickCount();
vTaskResumeAll();
}//safeguard to avoid lost-count
}
//----------------------------------
#endif
//here goes your functions:
#ifdef RTOS_EAT_CPU
 void eatCpu (uint32_t ticks) {
	TickType_t tick;
	TickType_t ticks_clock;
	ticks_clock=   ticks;
	uint32_t countTick = 0;
	while (countTick < ticks_clock) {
		tick =xTaskGetTickCount();
		while ( xTaskGetTickCount() == tick) {
			;
		}
		countTick += 1;
	}
}
#endif

