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
#ifdef RTOS_TEST
#include "rtos_custom.h"
#ifdef RTOS_PRINT_DEBUG
#include <driver_custom/driverdebug.h>
#endif

//******************************************************************
//Declaration of tasks functions
//******************************************************************
static void task1_imu (void* pvParameters );
static void task2_gps (void* pvParameters );
static void task3_sd (void* pvParameters );
static void task_sysmonitor (void* pvParameters );
static void task_rf (void* pvParameters );
static void task_ap1 (void* pvParameters );
static void task_periodic (void* pvParameters );
static void task_before_run(TaskData_periodic* task_data);
static void task_after_run(TaskData_periodic* task_data);
//******************************************************************
//Declaration of tasks structures and set up
//******************************************************************
//here you set your parameters regarding your system
//System tick time=1ms. So .t=1000 implies 1000 ms of task period
//y_c has to be the same value as y
TaskData_periodic task1_data= {.c=990,.t=3000,.d=3000,.r=1000,
		              .p=RTOSMAX_PRIORITIES-2,.pxTaskCode=task1_imu,.name="Task imu",//MAX 10 CHAR LONG
                                                              //.name='0123456789'MAX 10 CHAR LONG
	                 	.y=2000,.y_c=2000,.y_b=RTOS_False,.y_p_f=RTOS_FALSE,.y_p=5,
		                .ki=2000,.ki_c=0,.s=0,.s_p=1};
TaskData_periodic task2_data= {.c=990,.t=4000,.d=4000,.r=2000,
		              .p=RTOSMAX_PRIORITIES-3,.pxTaskCode=task2_gps,.name="Task gps",//MAX 10 CHAR LONG
					          };                                  //.name='0123456789'MAX 10 CHAR LONG
TaskData_periodic task3_data= {.c=990,.t=6000,.d=6000,.r=3000,
		              .p=RTOSMAX_PRIORITIES-4,.pxTaskCode=task3_sd, .name="Task sd",//MAX 10 CHAR LONG
					          };                                  //.name='0123456789'MAX 10 CHAR LONG
TaskData_aperiodic task4_data= {.c=1000,.t=6000,
		              .p= RTOSMAX_PRIORITIES - 1 ,.pxTaskCode=task_ap1,.name="Task ap1",
					  .s=0,.s_p=1};
TaskData_aperiodic task5_data= {.c=2000,.t=6000,
		              .p= RTOSMAX_PRIORITIES - 1 ,.pxTaskCode=task_ap1,.name="Task ap2",
		              .s=0,.s_p=1};
//for access of each structure;
static TaskData_periodic* task_data_p[]= {&task1_data , &task2_data , &task3_data};
static TaskData_aperiodic* task_data_ap[]= {&task4_data , &task5_data};
static uint8_t task_data_p_n =(sizeof(task_data_p))/sizeof(TaskData_periodic*) ;//amounts of tasks
static uint8_t task_data_ap_n =(sizeof(task_data_ap))/sizeof(TaskData_aperiodic*) ;//amounts of tasks
//for simulation purpose:
//semaphores
//system implementation of the call
#ifdef RTOS_SEMAPHORE_SHARED
  static SemaphoreHandle_t semaphore_shared;
#endif
#ifdef RTOS_MUTEX_SHARED
  static SemaphoreHandle_t mutex_shared = NULL;
#endif
#ifdef RTOS_DP
  uint8_t i_dp;//dual priority counter
  RTOSBoolType_t flag_dp=RTOS_FALSE;//dualpriorityflag
  static SemaphoreHandle_t semaphore_dp;
  static void task_dp (void* pvParameters);
#endif
#ifdef RTOS_SS
  static SemaphoreHandle_t xMutex = NULL;//mutex is used to allow only one task uses the serial
#endif

#ifdef RTOS_TI
// This a created version of vTaskDelayUntil for TI-RTOS
  //Notes: Due to how the TI-RTOS work is not necesary to SUSPEND ALL Tasks
  // like freertos vTaskDelayUntil does in order to check the clock frequency
  //THis is beacuse a task is running when enters the loop and
  // by default time slicing is disable, so the RTOS will interrupt stop
  //the task execution unless you change this
  //this allows you to safely compare time agaINST RTOS CLOCK
  //then at last you call for system sleep to move the task from running
  // to blocked. When task end their sleep will become READY
  //and the RTOS will choose if executing the task based on priority set up
void vTaskDelayUntil( RTOSTickType_t * const pxPreviousWakeTime, const RTOSTickType_t xTimeIncrement);
void vTaskDelayUntil( RTOSTickType_t * const pxPreviousWakeTime, const RTOSTickType_t xTimeIncrement){
RTOSTickType_t xTimeToWake,xTimeTodelay;
xTimeToWake=rtos_get_tick();//ge the current clock tick
xTimeTodelay=0;//initial set up
if(xTimeToWake>(*pxPreviousWakeTime) ){//check if the deadline of task has passes
	xTimeTodelay=xTimeToWake - *pxPreviousWakeTime;}//then sets the time delay
if (xTimeIncrement>xTimeTodelay){//checks if it is need to delay
	xTimeTodelay=xTimeIncrement-xTimeTodelay;//updated the delay time
    *pxPreviousWakeTime=xTimeToWake + xTimeTodelay;//update the counter
	Task_sleep(xTimeTodelay);//task changes from running to bloqued
   }
else{*pxPreviousWakeTime=rtos_get_tick();}//safeguard to avoid lost-count
}
#endif
//here goes your functions:
#ifdef EAT_CPU
static void eatCpu (int ticks);
static void eatCpu (int ticks) {
	RTOSTickType_t tick;
	RTOSTickType_t ticks_clock;
	ticks_clock=   ticks;
	int countTick = 0;
	while (countTick < ticks_clock) {
		tick =rtos_get_tick();
		while ( rtos_get_tick() == tick) {
			;
		}
		countTick += 1;

	}
}
#endif

/*
 * Array where the current tick, the id of the running task, the
 * available slack and each periodic task slack are written.
 */
void posixrtos_task_creation(void)
{
	  //IMPORTANTE, SI EL VALOR return de task_creation ES -1,
	  // CAMBIAR EL PARAMETRO DE HEAPSIZE EN FreeRTOSConfig.H configTOTAL_HEAP_SIZE
	  // al mayor valor posible o remover funciones de las tareas
	  // ya que ocupan tanto espacio de memoria ram que el SO no puede alocarlar
	  // y por lo tanto falla al crearlas. Si esto no alcanza una vez aumentado el heap size
	  //Otra solución es quitar funciones que requieran muchos recursos de la tarea.
	  //task creations

RTOSTickType_t task_creation;
uint8_t i_tk; //task index
pthread_t           thread;
pthread_attr_t      attrs;
struct sched_param  priParam;
int                 retc;
//creates periodic tasks
for (i_tk=0; i_tk < task_data_p_n; i_tk++) { //task_data_p_n
  #ifdef RTOS_DP
  (task_data_p[i]->p)=taskdualprioritys.lowband;//set default priority of periodic tasks to low band
  #endif
 //sprintf(cMessage, "PTask%d", i+1); // Task Name
  	  //POXIS LOOP FOR THREAD CREATION
      task_data_p[i_tk]->i_tk=i_tk;//Store the index of the structure in the structure to allow acces from thread
      pthread_attr_init(&attrs);      /* Initialize the attributes structure with default values */
      priParam.sched_priority = task_data_p[i_tk]->p;
      retc = pthread_attr_setschedparam(&attrs, &priParam);
      retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
      retc |= pthread_attr_setstacksize(&attrs, RTOSSTACKDEPTH);
      if (retc != 0){while (1) {}}/* failed to set attributes */
      retc = pthread_create(&thread, &attrs, task_data_p[i_tk]->pxTaskCode, &(task_data_p[i_tk]->i_tk));
      if (retc != 0){while (1) {}}/* pthread_create() failed */
 // task_creation=xTaskCreate(task_data_p[i]->pxTaskCode,task_data_p[i]->name,RTOSSTACKDEPTH, NULL,task_data_p[i]->p,&(task_data_p[i]->handle));

  #ifdef RTOS_SS
   vSlackSetTaskParams((task_data_p[i_tk]->handle),PERIODIC_TASK, task_data_p[i_tk]->t,
		  task_data_p[i_tk]->d,task_data_p[i_tk]->c, task_data_p[i_tk]->s_p  );
  #endif
 #ifdef RTOS_PRINT_DEBUG
 printdebug("Task "),printdebug(task_data_p[i_tk]->name);
 printdebug(" creation result: %d \n\r",task_creation);
 #endif
}
//creates aperiodic tasks
for (i_tk=0; i_tk < task_data_ap_n; i_tk++) {
#ifdef RTOS_DP
(task_data_ap[i_tk]->p)=taskdualprioritys.highband;//set default priority to high band
#endif

//task_creation=xTaskCreate(task_data_ap[i]->pxTaskCode,task_data_ap[i]->name,  RTOSSTACKDEPTH, NULL,task_data_ap[i]->p,&(task_data_ap[i]->handle));
 #ifdef RTOS_SS
  vSlackSetTaskParams(task_data_ap[i_tk]->handle, APERIODIC_TASK, 0, 0, 0, task_data_ap[i_tk]->s_p);
 #endif
 #ifdef RTOS_PRINT_DEBUG
 printdebug("Task "),printdebug(task_data_p[i_tk]->name);
 printdebug(" creation result: %d \n\r",task_creation);
 #endif
}
// creates semaphores and mutex
#ifdef RTOS_SEMAPHORE_SHARED
  semaphore_shared = xSemaphoreCreateBinary();//sempahore is uses for syncro
#endif
#ifdef RTOS_MUTEX_SHARED
  mutex_shared = xSemaphoreCreateMutex(); //mutex implementa mecanismo de herencia de prioridades
#endif
#ifdef RTOS_DP
 xTaskCreate( task_dp, "DP", configMINIMAL_STACK_SIZE, NULL,
		 RTOSMAX_PRIORITIES -1, NULL);//creates task that handles dual priority
semaphore_dp = xSemaphoreCreateBinary();//sempahore is uses for syncro
#endif
#ifdef RTOS_SS_DEBUG
xMutex = xSemaphoreCreateMutex(); //mutex implementa mecanismo de herencia de prioridades
#endif

//IMPORTANT:
/*if your task must communitate with periphericals with i2c, spi, uart, ect
DISABLE ROUND ROBBIN (SO will run in FIFO mode for task handle)
Add: #define  configUSE_TIME_SLICING 0  to "FreeRTOSConfig.h" inside "Inc" folder
configUSE_TIME_SLICING is set to 0 then the RTOS scheduler
will still run the highest priority task that is in the Ready state,
but will not switch between tasks of equal priority just because a
tick interrupt has occurred.*/

}
#ifdef RTOS_DP
void task_dp( void* pvParameters )
{
	TaskData_periodic* task_data;
	uint8_t i = 0;
    for(;;) {
        /* Se bloquea en el semáforo binario. El semáforo binario inicialmente
         * esta en cero. El "up" del semáforo se realiza desde el tick de reloj
         * mediante la función vApplicationTickHook(), sólo si existen tareas
         * cuyo contador de tiempo de promoción sea cero. */
        xSemaphoreTake(semaphore_dp, portMAX_DELAY );//tries to take sempaphore until release
        /* Actualiza las prioridades de las tareas cuyo contador de promoción
         * sea cero. */
        for (i=0; i < task_data_p_n; i++) {
        	task_data=(task_data_p[i]);
        	if((task_data->y_c==0)&&((task_data->y_p_f)==RTOS_FALSE)){//check if promotion counter is equal to 0
            vTaskPrioritySet(task_data->handle,task_data->y_p);//Elevates priority to its asigned highband
            task_data->y_p_f=RTOS_TRUE;
        	}
        }
    }
}

#endif
static void task_periodic (void* pvParameters )
{
RTOSTickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.
uint8_t task_index=*((uint8_t*)pvParameters); //index of led
TaskData_periodic* task_data=(task_data_p[task_index]);//handle of task1;
//xTask *task = (xTask*) pvParameters;//for slask setaling
RTOSTickType_t xFrequency = task_data->t;
for (;;)
	{
	 //to do before task run
	task_before_run(task_data);//To run before executing tasks work
//********************************************************
	//TASK FUNCTION TO EXECUTE, CHANGE ACCORDINGLY
    eatCpu(task_data->c);//execute the task
//************************************************
    task_after_run(task_data);//To run before executing tasks work
    //NOW THAT task is finished, will start their delay execution until next time
    vTaskDelayUntil(&xLastWakeTime, xFrequency);}//task changes from running to bloqued
	}
void task_before_run(TaskData_periodic* task_data)
{
	#ifdef RTOS_DP
  	vTaskPrioritySet(task_data->handle,taskdualprioritys.lowband);//set priority to lowband
  	task_data->y_c=task_data->y;
	task_data->y_b=RTOS_FALSE;//task starts, then is NON bloqued
    #endif
    #ifdef RTOS_PRINT_DEBUG
	taskDISABLE_INTERRUPTS();//this to disable O.S. interrupt when system is printing uart
    printdebug("Tick starting ");printdebug(task_data->name);
    printdebug(": %d \n\r", xTaskGetTickCount());
    taskENABLE_INTERRUPTS();//Enables disable O.S. interrupt after system printed
    #endif
}
void task_after_run(TaskData_periodic* task_data)
{
#ifdef RTOS_SS_DEBUG
SsTCB_t *pxTaskSsTCB = getTaskSsTCB( NULL );
if ( xSemaphoreTake( xMutex, MUTEX_TIMEOUT ) )
     {
         vTasksGetSlacks( slackArray );
         vPrintSlacks( cMessage, 'E', slackArray, pxTaskSsTCB->xCur );
         xSemaphoreGive( xMutex );
     }
     else
     {
         prvPrintString("!");
     }
#endif
#ifdef RTOS_DP
task_data->y_b=RTOS_TRUE;//task finish, then is bloqued
#endif
}

static void task1_imu (void* pvParameters )
{
	RTOSTickType_t xLastWakeTime= 0; // Initialise the xLastWakeTime variable with the current time.
	//TaskData_periodic *task_data= (TaskData_periodic*) pvParameters;//handle of task elements, like execution time and frequency;//
	//TaskData_periodic *task_data= (TaskData_periodic*) pvParameters;
	uint8_t task_index=*((uint8_t*)pvParameters); //index of led
	TaskData_periodic* task_data=(task_data_p[task_index]);//handle of task1;
	RTOSTickType_t xFrequency = task_data->t;
	RTOSTickType_t clocktick_start,clocktick_end;
	for (;;)
	{
  //To do BEFORE doing task
      #ifdef RTOS_DP
	  vTaskPrioritySet(task_data->handle,taskdualprioritys.lowband);//set priority to lowband
	  task_data->y_c=task_data->y;//reset dual piority counter
      task_data->y_b=RTOS_FALSE;//task starts, then is NON bloqued
      task_data->y_p_f=RTOS_FALSE;//task starts, then is NON bloqued
      #endif
      #ifdef RTOS_PRINT_DEBUG
      taskDISABLE_INTERRUPTS();//this to disable O.S. interrupt when system is printing uart
      printdebug("Tick starting ");printdebug(task_data->name);
      printdebug(": %d \n\r", xTaskGetTickCount());
      taskENABLE_INTERRUPTS();//Enables disable O.S. interrupt after system printed
      #endif
//******************************************************
//TASK FUNCTION TO EXECUTE, CHANGE ACCORDINGLY !!!
//******************************************************
  #ifdef RTOS_SEMAPHORE_SHARED
   if (xSemaphoreTake(semaphore_shared, 50)){eatCpu(task_data->c);xSemaphoreGive(semaphore_shared);}
    else{printdebug(task_data->name);printdebug(" could not take semaphore \n\r");}
  #endif
  #ifdef RTOS_MUTEX_SHARED
      if (xSemaphoreTake(mutex_shared, 50)){eatCpu(task_data->c);xSemaphoreGive(mutex_shared);}
        else{printdebug(task_data->name);printdebug(" could not take mutex \n\r");}
  #endif
  #ifndef RTOS_SEMAPHORES_SHARED
      clocktick_start=rtos_get_tick();
      Task_yield();//Enables the cpu to do another work
      eatCpu(task_data->c);//execute the task
      clocktick_end=rtos_get_tick();//(1000 / Clock_tickPeriod);
  #endif
//******************************************************
   //To do BEFORE doing task
      #ifdef RTOS_SS_DEBUG
      SsTCB_t *pxTaskSsTCB = getTaskSsTCB( NULL );
      if ( xSemaphoreTake( xMutex, MUTEX_TIMEOUT )){
         vTasksGetSlacks( slackArray );
         vPrintSlacks( cMessage, 'E', slackArray, pxTaskSsTCB->xCur );
         xSemaphoreGive( xMutex );}
       else
       {printdebug("!");}
       #endif
       #ifdef RTOS_DP
       task_data->y_b=RTOS_TRUE;//task finish, then is bloqued
       #endif
//NOW THAT task is finished, will start their delay execution until next time
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
  	}//task changes from running to bloqued}
}
static void task2_gps (void* pvParameters )
{
	RTOSTickType_t xLastWakeTime= 0; // Initialise the xLastWakeTime variable with the current time.
	uint8_t task_index=*((uint8_t*)pvParameters); //index of led
	TaskData_periodic* task_data=(task_data_p[task_index]);//handle of task1;
	RTOSTickType_t xFrequency = task_data->t;
	RTOSTickType_t clocktick_start,clocktick_end;
	for (;;)
	{
  //To do BEFORE doing task
      #ifdef RTOS_DP
	  vTaskPrioritySet(task_data->handle,taskdualprioritys.lowband);//set priority to lowband
	  task_data->y_c=task_data->y;//reset dual piority counter
      task_data->y_b=RTOS_FALSE;//task starts, then is NON bloqued
      task_data->y_p_f=RTOS_FALSE;//task starts, then is NON bloqued
      #endif
      #ifdef RTOS_PRINT_DEBUG
      taskDISABLE_INTERRUPTS();//this to disable O.S. interrupt when system is printing uart
      printdebug("Tick starting ");printdebug(task_data->name);
      printdebug(": %d \n\r", xTaskGetTickCount());
      taskENABLE_INTERRUPTS();//Enables disable O.S. interrupt after system printed
      #endif
//******************************************************
//TASK FUNCTION TO EXECUTE, CHANGE ACCORDINGLY !!!
//******************************************************
#ifdef RTOS_SEMAPHORE_SHARED
 if (xSemaphoreTake(semaphore_shared, 50)){eatCpu(task_data->c);xSemaphoreGive(semaphore_shared);}
  else{printdebug(task_data->name);printdebug(" could not take semaphore \n\r");}
#endif
#ifdef RTOS_MUTEX_SHARED
    if (xSemaphoreTake(mutex_shared, 50)){eatCpu(task_data->c);xSemaphoreGive(mutex_shared);}
      else{printdebug(task_data->name);printdebug(" could not take mutex \n\r");}
#endif
#ifndef RTOS_SEMAPHORES_SHARED
    clocktick_start=rtos_get_tick();
    eatCpu(task_data->c);//execute the task
    clocktick_end=rtos_get_tick();//(1000 / Clock_tickPeriod);
#endif
//******************************************************
    //To do AFTER doing task
      #ifdef RTOS_SS_DEBUG
     SsTCB_t *pxTaskSsTCB = getTaskSsTCB( NULL );
      if ( xSemaphoreTake( xMutex, MUTEX_TIMEOUT ) )
       {
         vTasksGetSlacks( slackArray );
         vPrintSlacks( cMessage, 'E', slackArray, pxTaskSsTCB->xCur );
         xSemaphoreGive( xMutex );
       }
       else
       {
    	   printdebug("!");
       }
       #endif
       #ifdef RTOS_DP
       task_data->y_b=RTOS_TRUE;//task finish, then is bloqued
       #endif
//NOW THAT task is finished, will start their delay execution until next time
       vTaskDelayUntil(&xLastWakeTime, xFrequency);
  	}//task changes from running to bloqued
}
static void task3_sd (void* pvParameters )
{
	RTOSTickType_t xLastWakeTime= 0; // Initialise the xLastWakeTime variable with the current time.
	uint8_t task_index=*((uint8_t*)pvParameters); //index of led
	TaskData_periodic* task_data=(task_data_p[task_index]);//handle of task1;
	RTOSTickType_t xFrequency = task_data->t;
	RTOSTickType_t clocktick_start,clocktick_end;

	for (;;)
	{
  //To do BEFORE doing task
      #ifdef RTOS_DP
	  vTaskPrioritySet(task_data->handle,taskdualprioritys.lowband);//set priority to lowband
	  task_data->y_c=task_data->y;//reset dual piority counter
      task_data->y_b=RTOS_FALSE;//task starts, then is NON bloqued
      task_data->y_p_f=RTOS_FALSE;//task starts, then is NON bloqued
      #endif
      #ifdef RTOS_PRINT_DEBUG
      taskDISABLE_INTERRUPTS();//this to disable O.S. interrupt when system is printing uart
      printdebug("Tick starting ");printdebug(task_data->name);
      printdebug(": %d \n\r", xTaskGetTickCount());
      taskENABLE_INTERRUPTS();//Enables disable O.S. interrupt after system printed
      #endif
//******************************************************
//TASK FUNCTION TO EXECUTE, CHANGE ACCORDINGLY !!!
//******************************************************
       clocktick_start=rtos_get_tick();
       eatCpu(task_data->c);//execute the task
       clocktick_end=rtos_get_tick();//(1000 / Clock_tickPeriod);
//******************************************************
       //To do AFTER doing task
         #ifdef RTOS_SS_DEBUG
        SsTCB_t *pxTaskSsTCB = getTaskSsTCB( NULL );
         if ( xSemaphoreTake( xMutex, MUTEX_TIMEOUT ) )
          {
            vTasksGetSlacks( slackArray );
            vPrintSlacks( cMessage, 'E', slackArray, pxTaskSsTCB->xCur );
            xSemaphoreGive( xMutex );
          }
          else
          {
       	   printdebug("!");
          }
          #endif
          #ifdef RTOS_DP
          task_data->y_b=RTOS_TRUE;//task finish, then is bloqued
          #endif
//NOW THAT task is finished, will start their delay execution until next time
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}//task changes from running to bloqued
}

static void task_ap1 (void* pvParameters )
{
	RTOSTickType_t xLastWakeTime= 0; // Initialise the xLastWakeTime variable with the current time.
	TaskData_aperiodic* task_data=(task_data_ap[0]);//handle of task1;
	RTOSTickType_t xFrequency = task_data->t;
	//srand(1);
	for (;;)
	{
		//xFrequency=xFrequency+(TickType_t)(rand() % 5000);//set random time to be called
        vTaskDelayUntil(&xLastWakeTime,xFrequency);//goes BEFORE TASK to avoid execution at startup
        #ifdef RTOS_PRINT_DEBUG
         taskDISABLE_INTERRUPTS();//this to disable O.S. interrupt when system is printing uart
 		 printdebug("Tiempo Tarea ap1: %d \n\r", xTaskGetTickCount());
         taskENABLE_INTERRUPTS();//Enables disable O.S. interrupt after system printed
        #endif
		eatCpu(task_data->c);//execute the task
	}
}


#ifdef RTOS_DP
//IMPORTANT: ENSURE "configUSE_TICK_HOOk" value is 1 (inside FreeRTOSConfig.h) or this function will not be called!!
//INSIDE vApplicationTickHook YOU CANNOT CALL "eTaskGetState" or "vTaskPrioritySet" inside, becausen system will crash
//WARNING: THIS ONLY WORKS IF THE TASKS ARE INDEPENDENT OR NON BLOCKING
//IF you are going to work with task that can enter suspend or other
//modes, you will need to change the algoritm (maybe add a flag)
//if a task is blocked by a semaphore, you need to keep decreasing the counter
//This script cannot detect if a sempahore bloqued the task or if the task is running
//this only semaphore is used to do the priority changes
//****THIS WAS TESTES AND WORKS, HOOK IS CALLED*****
void vApplicationTickHook (void)
{
	TaskData_periodic* task_data;
	//decrements the counters
	for (i_dp = 0; i_dp < task_data_p_n; i_dp++)
	{
	   task_data=(task_data_p[i_dp]);//get the pointer of the acces
	   //now we check the state of the tassk
	   if (task_data->y_b==RTOS_FALSE)//if task is NOT bloqued
	    { if (task_data->y_c>0){--(task_data->y_c);} }//decrement DP counter
       if((task_data->y_c==0)&&((task_data->y_p_f)==RTOS_FALSE)){//counter of DP=0, then calls the sempahore
    	   flag_dp=RTOS_TRUE;}
     }
    if (flag_dp==RTOS_TRUE){
        xSemaphoreGiveFromISR(semaphore_dp, NULL );
	    flag_dp=RTOS_FALSE;
      }
}
#endif

#ifdef RTOS_SS_DEBUG
//This is a print fuction defined to use with slack stealing
static void vPrintSlacks( char *buf, char s, int32_t * slackArray, TickType_t xCur )
{
    sprintf(buf, "%s\t%c\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n\r",
            pcTaskGetTaskName(NULL), s,
            (int) slackArray[0], (int) slackArray[2], (int) slackArray[3],
            (int) slackArray[4], (int) slackArray[5], (int) slackArray[6],
            (int) xCur);
    printdebug( buf );
}
#endif
#ifdef RTOS_SSS
// Function added to  make slack stealing work
void vApplicationDeadlineMissedHook( char *pcTaskName, const SsTCB_t *xSsTCB,
        TickType_t xTickCount )
{
	  taskDISABLE_INTERRUPTS();
	  sprintf(cMessage, "\n\r%s missed its deadline at %d\n\r", pcTaskName, (int) xTickCount);
	  //prvPrintString( cMessage );
	  for (;; )
	  {}
	return;
}
#endif
#endif

