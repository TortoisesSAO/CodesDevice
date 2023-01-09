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
#include <RTOS_AL/rtos_AL.h>
#include <RTOS_AL/semaphore_AL.h> //allow the use of an operative system
#include <RTOS_TASKS_PARAMS.h>

#include <ti/sysbios/BIOS.h>
#ifdef RTOS_PRINT_DEBUG
#include <driver_custom/driverdebug.h>
#endif

//******************************************************************
//Declaration of tasks functions
//******************************************************************
void task_temp (uint8_t task_index);
void task_i2c0_imu_accgyro (uint8_t task_index);
void task_i2c0_imu_mag (uint8_t task_index);
void task_uart0_gps (uint8_t task_index);//CHECKED: LIBRARY WORKS PERFECT!!
void task_spi0_microsd (uint8_t task_index  );
// void task_sysmonitor (void* pvParameters );
void task_rfsub1ghz_ka (uint8_t task_index);
// void task_ap1 (void* pvParameters );
//******************************************************************
//Declaration of tasks structures and set up
//******************************************************************
//here you set your parameters regarding your system
//System tick time=1ms. So .t=1000 implies 1000 ms of task period
//y_c has to be the same value as y
TaskData taskdata_temp          ={  .c_s=TASKP_TEMP_TIME,.t_s=TASKP_TEMP_PERIOD,.d_s=TASKP_TEMP_DEADLINE,
        							.p=TASKP_TEMP_PRIORITY, .name=TASKP_TEMP_NAME,.type=TASKP_TEMP_TYPE,     //
									.pxTaskCode=task_temp,
        							};
TaskData taskdata_i2c0_imu_accgyro={.c_s=TASKP_IMU_TIME,.t_s=TASKP_IMU_PERIOD,.d_s=TASKP_IMU_DEADLINE,
		                            .p=TASKP_IMU_PRIORITY, .name=TASKP_IMU_NAME,.type=TASKP_IMU_TYPE,     //
					                .pxTaskCode=task_i2c0_imu_accgyro,
		                            };
TaskData taskdata_i2c0_imu_mag= {   .c_s=TASKP_IMUM_TIME,.t_s=TASKP_IMUM_PERIOD,.d_s=TASKP_IMUM_DEADLINE,
		                            .p=TASKP_IMUM_PRIORITY, .name=TASKP_IMUM_NAME,.type=TASKP_IMUM_TYPE,     //
					                .pxTaskCode=task_i2c0_imu_mag,//task_i2c0_imu_mag,
		                            };
TaskData taskdata_spi0_microsd=     {.c_s=TASKA_SD_TIME,.t_s=TASKA_SD_PERIOD,.d_s=TASKA_SD_PERIOD,
		                            .p=TASKA_SD_PRIORITY, .name=TASKA_SD_NAME,.type=TASKA_SD_TYPE,//
					                .pxTaskCode=task_spi0_microsd,
		                            };
TaskData taskdata_rfsub_keep_alive= {.c_s=TASKP_RFKA_TIME,.t_s=TASKP_RFKA_PERIOD,.d_s=TASKP_RFKA_DEADLINE,
		                            .p=TASKP_RFKA_PRIORITY, .name=TASKP_RFKA_NAME,.type=TASKP_RFKA_TYPE,//
					                .pxTaskCode=task_rfsub1ghz_ka,
		                            };
TaskData taskdata_uart0_gps=        {.c_s=TASKP_RFKA_TIME,.t_s=TASKP_GPS_PERIOD,.d_s=TASKP_GPS_DEADLINE,
		                            .p=TASKP_GPS_PRIORITY, .name=TASKP_GPS_NAME,.type=TASKP_GPS_TYPE,//
					                .pxTaskCode=task_uart0_gps,
		                            };
//for access of each structure;        //&taskdata_spi0_microsd
//static TaskData* task_data_p[]= { &task2_data, &task3_data,&task4_data};//&task1_data ,  &taskdata_i2c0_imu_accgyro,
extern TaskData* task_data_p[]= {&taskdata_spi0_microsd,&taskdata_rfsub_keep_alive,&taskdata_i2c0_imu_accgyro,&taskdata_i2c0_imu_mag,
		                         &taskdata_uart0_gps, &taskdata_temp
                              };//,&taskdata_rfsub_keep_alive};//,&task_rfsub_keepalive};//&task1_data , &task2_data
extern const uint8_t task_data_p_n =(sizeof(task_data_p))/sizeof(TaskData*) ;//amounts of tasks
// struct for fixex stack size.
typedef struct{
  uint8_t stack[configMINIMAL_STACK_SIZE];   //300*12=3600 bytes
}rtos_stacks;// Has to be LEsser than SD_BLOCK_UNIT_SIZE
static rtos_stacks task_stack[task_data_p_n];
//semaphore for all process that share the use of the state machine
extern sempaphore_struct sem_state_machine;
#define SEM_STATE_TIMEOUT			1	 //seconds
//semaphore for sharing the i2c0 bus acces
extern sempaphore_struct sem_i2c0;
#define SEM_I2C0_TIMEOUT			0.1	 //seconds
//semaphore for sharing the spi0 bus acces
extern sempaphore_struct sem_spi0_microsd;//semaphore for sharing the spi bus acces
#define SEM_SPI0_MICROSD			1	 //this a very high timout
//semaphore for sharing the uart 0 bus acces
extern sempaphore_struct sem_uart0;
#define SEM_UART0_TIMEOUT			0.1	 //seconds
//semaphore for sharing the uart 0 bus acces
extern sempaphore_struct sem_uart1;  //semaphore for sharing the uart 1 bus acces
#define SEM_UART1_TIMEOUT			0.1	 //seconds
//semaphore for sharing the adc0  drive access
extern sempaphore_struct sem_adc0;   //semaphore for sharing the uart 1 bus acces
#define SEM_ADC0_TIMEOUT			0.015//11 ms is max time for conversion
//semaphore for sharing teh rf sub1GHZ radio
extern sempaphore_struct sem_rfsub1GHZ;
#define SEM_RFSUB1GHZ_TIMEOUT		0.2  //this timeout is for packabes lower than 100 bytes at 5 KBPS
/*
TickType_t sec_to_tick(float seconds){
///(TickType_t)
	return( ( ( (seconds) * CLOCK_TICK_UNIT) / Clock_tickPeriod ));
}*/
void rtos_task_init_parameter(TaskData* t)
{
t->stack_size=configMINIMAL_STACK_SIZE;
t->c= SEC_TO(t->c_s);
t->t= SEC_TO(t->t_s);
t->d= SEC_TO(t->d_s);
}
extern void rtos_task_creation(void);
void rtos_task_creation(void)
{
	  //IMPORTANTE, SI EL VALOR return de task_creation ES -1,
	  // CAMBIAR EL PARAMETRO DE HEAPSIZE EN FreeRTOSConfig.H configTOTAL_HEAP_SIZE
	  // al mayor valor posible o remover funciones de las tareas
	  // ya que ocupan tanto espacio de memoria ram que el SO no puede alocarlar
	  // y por lo tanto falla al crearlas. Si esto no alcanza una vez aumentado el heap size
	  //Otra solución es quitar funciones que requieran muchos recursos de la tarea.
	  //task creations

TickType_t task_creation;
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
  	  //POXIS LOOP FOR THREAD CREATION
      rtos_task_init_parameter(task_data_p[i_tk]);//finifh to set up the task parameters
      task_data_p[i_tk]->stack_p=&(task_stack[i_tk].stack);
      task_data_p[i_tk]->i_tk=i_tk;//Store the index of the structure in the structure to allow acces from thread
      pthread_attr_init(&attrs);      /* Initialize the attributes structure with default values */
      priParam.sched_priority = (int) (task_data_p[i_tk]->p);
      //the set priority is genmerating error
      retc = pthread_attr_setschedparam(&attrs, &priParam);
      retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
      //retc |= pthread_attr_setstacksize(&attrs, RTOSSTACKDEPTH);//*commented: to use dynamic stack for rtos stack
      retc |= pthread_attr_setstack(&attrs,task_data_p[i_tk]->stack_p,configMINIMAL_STACK_SIZE);// we use fixed stack instead
      if (retc != 0){while (1) {}}/* failed to set attributes */
      retc = pthread_create(&(task_data_p[i_tk]->handle), &attrs, task_data_p[i_tk]->pxTaskCode, task_data_p[i_tk]->i_tk);
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

//task_creation=xTaskCreate(task_data_ap[i]->pxTaskCode,task_data_ap[i]->name,  RTOSSTACKDEPTH, NULL,task_data_ap[i]->p,&(task_data_ap[i]->handle));
 #ifdef RTOS_SS
  vSlackSetTaskParams(task_data_ap[i_tk]->handle, APERIODIC_TASK, 0, 0, 0, task_data_ap[i_tk]->s_p);
 #endif
 #ifdef RTOS_PRINT_DEBUG
 printdebug("Task "),printdebug(task_data_p[i_tk]->name);
 printdebug(" creation result: %d \n\r",task_creation);
 #endif
}
//Sempahore Definitions
semaphore_init(&sem_i2c0,SEC_TO(SEM_I2C0_TIMEOUT));
semaphore_init(&sem_spi0_microsd,SEC_TO(SEM_SPI0_MICROSD));
//BIOS_start();
}

//IMPORTANT:
/*if your task must communitate with periphericals with i2c, spi, uart, ect
DISABLE ROUND ROBBIN (SO will run in FIFO mode for task handle)
Add: #define  configUSE_TIME_SLICING 0  to "FreeRTOSConfig.h" inside "Inc" folder
configUSE_TIME_SLICING is set to 0 then the RTOS scheduler
will still run the highest priority task that is in the Ready state,
but will not switch between tasks of equal priority just because a
tick interrupt has occurred.*/
