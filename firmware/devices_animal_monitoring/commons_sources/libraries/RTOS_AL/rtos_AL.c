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
 #include <ti/posix/tirtos/_pthread.h> //TI RTOSto acces task sleep fro
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

int task_create_AL(TaskData* ptask_data){
int  retc;
#if defined(RTOS_POSIX_AND_OTHER)|defined(RTOS_POSIX)
	pthread_attr_t      attrs;
	struct sched_param  priParam;
	pthread_attr_init(&attrs);
    priParam.sched_priority = (int) (ptask_data->p);
	retc = pthread_attr_setschedparam(&attrs, &priParam);
	retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
	if (ptask_data->stack_p==NULL){ retc |= pthread_attr_setstacksize(&attrs, ptask_data->stack_size);} //dynamic stack allocation
	else { retc |= pthread_attr_setstack(&attrs,(ptask_data->stack_p),ptask_data->stack_size);}       // we use fixed stack instead
    retc = pthread_create(&(ptask_data->handle), &attrs,(void*)( ptask_data->pxTaskCode),  ptask_data);
    if ((retc==0)&(ptask_data->stack_p==NULL)){ptask_data->stack_p=attrs.stack; }//stores the pointer if creation succesfull
#else
#ifdef RTOS_TIRTOS
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.priority = ptask_data->p;
    taskParams.name = ptask_data->name;
    taskParams.arg0 = ((uintptr_t)(ptask_data));// (UArg)
    taskParams.env = ptask_data;
    taskParams.stackHeap=NULL;
    if (ptask_data->stack_p==NULL){retc=2;}
    else{
        taskParams.stackSize = ptask_data->stack_size;
        taskParams.stack = ptask_data->stack_p;
        ptask_data->handle=Task_construct(&(ptask_data->object), (Task_FuncPtr)(ptask_data->pxTaskCode), &taskParams, NULL);
        retc=0;
      }
 #endif
#ifdef RTOS_FREERTOS
    if (ptask_data->stack_p==NULL){xTaskCreate(ptask_data->pxTaskCode,ptask_data->name,RTOSSTACKDEPTH, NULL,ptask_data->p,&(ptask_data->handle));}
    else                          {xTaskCreateStatic(ptask_data->pxTaskCode,ptask_data->name,RTOSSTACKDEPTH, NULL,ptask_data->p,&(ptask_data->handle));}
    // else{}
 #endif
#endif
return(retc);
}

/*
pthread_attr_init(&attrs);      // Initialize the attributes structure with default values
priParam.sched_priority = (int) (task_data_p[i_tk]->p);
//the set priority is genmerating error
retc = pthread_attr_setschedparam(&attrs, &priParam);
retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
//choose between dynamic allocation and static allocation
if (task_data_p[i_tk]->stack_p==NULL){ retc |= pthread_attr_setstacksize(&attrs, task_data_p[i_tk]->stack_size);}                          //dynamic stack allocation
else { retc |= pthread_attr_setstack(&attrs,(task_data_p[i_tk]->stack_p),task_data_p[i_tk]->stack_size);}  // we use fixed stack instead

if (retc != 0){while (1) {}}/// failed to set attributes
retc = pthread_create(&(task_data_p[i_tk]->handle), &attrs, (void*) (task_data_p[i_tk]->pxTaskCode), (task_data_p[i_tk]));
if ((retc==0)&(task_data_p[i_tk]->stack_p==NULL)){
   //pthread_attr_getstack(&attrs, (void *)&(task_data_p[i_tk]->stack_p),(size_t*)&(task_data_p[i_tk]->stack_size));
   task_data_p[i_tk]->stack_p=attrs.stack; }
if (retc != 0){while (1) {}}// pthread_create() failed
*/
// task_creation=xTaskCreate(task_data_p[i]->pxTaskCode,task_data_p[i]->name,RTOSSTACKDEPTH, NULL,task_data_p[i]->p,&(task_data_p[i]->handle));
//----------------------------------
int task_delete_AL(TaskData * ptask_data){
int retc;
retc=1;
#if defined(RTOS_POSIX_AND_OTHER)|defined(RTOS_POSIX)
if ((ptask_data->handle)!=NULL){
	//retc=_pthread_destroy(ptask_data->handle);
	pthread_Obj  *thread = (pthread_Obj *)((ptask_data->handle));
	//Task_delete(&(thread->task));
	//retc=pthread_detach(ptask_data->handle);//pthread_detach//release the resources of the thread when the thread end if dinamyc memory alloc was used
	retc=pthread_cancel(ptask_data->handle);//  pthread_cancel//stops the thread in the selected moment and then release the resources. the thread nevers end to excutes all its tasks
	ptask_data->handle=NULL;//set as null before destruction
}
//retc=pthread_cancel((ptask_data->handle));
#else
#ifdef RTOS_TIRTOS
       if ((ptask_data->handle)!=NULL){
          if ((ptask_data->stack_p)!=NULL){
               Task_destruct(&(ptask_data->object));
               ptask_data->handle=NULL;}
          else{
               Task_delete((Task_Handle *)(ptask_data->handle));
               ptask_data->handle=NULL;}
          }
  #endif
#endif
//Free rtos
  ///  if (ptask_data->stack_p==NULL){xTaskCreate(ptask_data->pxTaskCode,ptask_data->name,configMINIMAL_STACK_SIZE, NULL,ptask_data->p,&(ptask_data->handle));}

//Task_exit();
/*
if (retc!=0){	while(1);}
*/
return(retc);
}








//Task delay until
#ifndef RTOS_FREERTOS //If no free rtos is defined
void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement){
TickType_t xTimeToWake,xTimeTodelay;
//vTaskSuspendAll();//suspend all tasks execution
xTimeToWake=xTaskGetTickCount();//get the current clock tick
xTimeTodelay=0;//initial set up
if(xTimeToWake>(*pxPreviousWakeTime) ){//check if the deadline of task has passes
	xTimeTodelay=xTimeToWake - *pxPreviousWakeTime;
	}//then sets the time-to-delay value
if (xTimeIncrement>xTimeTodelay){//checks if it is need to delay
	xTimeTodelay=xTimeIncrement-xTimeTodelay;//updates delay time
    *pxPreviousWakeTime=xTimeToWake + xTimeTodelay;//update the counter of previous wake up time
	//vTaskResumeAll();//never put this after task sleep
	vTaskDelay(xTimeTodelay);//task changes from running to bloqued
   }
else{*pxPreviousWakeTime=xTaskGetTickCount();
//vTaskResumeAll();
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

