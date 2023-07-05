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
#include "RTOS_TASKS_PARAMS.h"
#include "rtos_tasks.h"

#ifdef  RTOS_PRINTU
  #include "task_uart_printu.h"
#else
#define  printuf_nortos(...)               ;
#endif
#include "task_spi0_microsd.h"
//******************************************************************
//Declaration of tasks functions. must also be declared extern in the c/cpp file
//******************************************************************
void task_pwm0_1_2_leds (void* ptask_data);
void task_temperature (void* ptask_data);
void task_i2c0_imu_accgyro (void* ptask_data);
void task_i2c0_imu_mag (void* ptask_data);
void task_uart1_gps (void* ptask_data);
void task_spi0_microsd (void* ptask_data);
void task_animal_activity (void* ptask_data);
void task_adc0_sysmonitor (void* ptask_data);
void task_rfsub1ghz_ka (void* ptask_data);
void task_rfsub1ghz_tx_data (void* ptask_data);
void task_animal_activity( void* ptask_data);
void task_rfsub1ghz_TD_rx_data( void* ptask_data);
void task_rfsub1ghz_DCS_rx_data (void* ptask_data);
#ifdef RTOS_PRINTU
void task_uart0_printu(void* ptask_data);
#endif
//*********************************************************************************************************
//Declaration of tasks structures and set up
//**********************************************************************************************************
 TaskData taskdata_pwmleds       ={.c_s=TASKP_LEDS_TIME,.t_s=TASKP_LEDS_PERIOD,.d_s=TASKP_LEDS_DEADLINE,
                                   .p=TASKP_LEDS_PRIORITY, .name=TASKP_LEDS_NAME,.type=TASKP_LEDS_TYPE,
                                   .pxTaskCode=task_pwm0_1_2_leds,             .stack_size=TASKP_LEDS_STACKSIZE,
                                   };
 TaskData taskdata_temperature       ={  .c_s=TASKP_TEMP_TIME,.t_s=TASKP_TEMP_PERIOD,.d_s=TASKP_TEMP_DEADLINE,
        							.p=TASKP_TEMP_PRIORITY, .name=TASKP_TEMP_NAME,.type=TASKP_TEMP_TYPE,
									.pxTaskCode=task_temperature,             .stack_size=TASKP_TEMP_STACKSIZE,
        							};
 TaskData taskdata_adc0_sysmonitor  ={  .c_s=TASKP_SYSMONITOR_TIME,.t_s=TASKP_SYSMONITOR_PERIOD,.d_s=TASKP_SYSMONITOR_DEADLINE,
        							.p=TASKP_SYSMONITOR_PRIORITY, .name=TASKP_SYSMONITOR_NAME,.type=TASKP_SYSMONITOR_TYPE,     //
									.pxTaskCode=task_adc0_sysmonitor,  .stack_size=TASKP_SYSMONITOR_STACKSIZE,
        							};
 TaskData taskdata_i2c0_imu_accgyro={.c_s=TASKP_IMUAG_TIME,.t_s=TASKP_IMUAG_PERIOD,.d_s=TASKP_IMUAG_DEADLINE,
		                            .p=TASKP_IMUAG_PRIORITY, .name=TASKP_IMUAG_NAME,.type=TASKP_IMUAG_TYPE,     //
					                .pxTaskCode=task_i2c0_imu_accgyro, .stack_size=TASKP_IMUAG_STACKSIZE,
		                            };
 TaskData taskdata_i2c0_imu_mag= {   .c_s=TASKP_IMUM_TIME,.t_s=TASKP_IMUM_PERIOD,.d_s=TASKP_IMUM_DEADLINE,
		                            .p=TASKP_IMUM_PRIORITY, .name=TASKP_IMUM_NAME,.type=TASKP_IMUM_TYPE,     //
					                .pxTaskCode=task_i2c0_imu_mag,      .stack_size=TASKP_IMUM_STACKSIZE,
		                            };
 TaskData taskdata_rfsub_keep_alive= {.c_s=TASKP_RFKA_TIME,.t_s=TASKP_RFKA_PERIOD,.d_s=TASKP_RFKA_DEADLINE,
		                            .p=TASKP_RFKA_PRIORITY, .name=TASKP_RFKA_NAME,.type=TASKP_RFKA_TYPE,//
					                .pxTaskCode=task_rfsub1ghz_ka,      .stack_size=TASKP_RFKA_STACKSIZE,
		                            };
 TaskData taskdata_rfsub_tx_data=    {.c_s=TASKP_RFTX_TIME,.t_s=TASKP_RFTX_PERIOD,.d_s=TASKP_RFTX_DEADLINE,
		                            .p=TASKP_RFTX_PRIORITY, .name=TASKP_RFTX_NAME,.type=TASKP_RFTX_TYPE,//
					                .pxTaskCode=task_rfsub1ghz_tx_data,  .stack_size=TASKP_RFTX_STACKSIZE,
		                            };
 TaskData taskdata_uart1_gps=        {.c_s=TASKP_RFKA_TIME,.t_s=TASKP_GPS_PERIOD,.d_s=TASKP_GPS_DEADLINE,
		                            .p=TASKP_GPS_PRIORITY, .name=TASKP_GPS_NAME,.type=TASKP_GPS_TYPE,//
					                .pxTaskCode=task_uart1_gps,          .stack_size=TASKP_GPS_STACKSIZE,
		                            };
#ifdef SD_STORAGE_ON
 TaskData taskdata_spi0_microsd=     {.c_s=TASKA_SD_TIME,.t_s=TASKA_SD_PERIOD,.d_s=TASKA_SD_PERIOD,
                                    .p=TASKA_SD_PRIORITY,    .name=TASKA_SD_NAME,   .type=TASKA_SD_TYPE,//
                                    .pxTaskCode=task_spi0_microsd,      .stack_size=TASKP_SD_STACKSIZE,
                                    };
#endif
#ifdef IMU_ANIMAL_ACTIVITY_ENABLED
 TaskData taskdata_animal_activity= {.c_s=TASKA_ANIMALACT_TIME,.t_s=TASKA_ANIMALACT_PERIOD,.d_s=TASKA_ANIMALACT_PERIOD,
		                            .p=TASKA_ANIMALACT_PRIORITY,    .name=TASKA_ANIMALACT_NAME,   .type=TASKA_ANIMALACT_TYPE,//
					                .pxTaskCode=task_animal_activity,      .stack_size=TASKP_ANIMALACT_STACKSIZE,
		                            };
#endif
//------------------------------------------------------------------------------------------------
#ifdef RTOS_PRINTU
 TaskData taskdata_printu           = {.c_s=TASKA_PRINTU_TIME,.t_s=TASKA_PRINTU_PERIOD,.d_s=TASKA_PRINTU_PERIOD,
                                    .p=TASKA_PRINTU_PRIORITY,    .name=TASKA_PRINTU_NAME,   .type=TASKA_PRINTU_TYPE,//
                                    .pxTaskCode=task_uart0_printu,      .stack_size=TASKP_PRINTU_STACKSIZE,
                                    };
#endif
 //------------------------------------------------------------------------------------------------
 #if  (DEVICE_TYPE_KIND==DEVICE_TYPE_TD)
  TaskData taskdata_rfsub_td_rx=     {.c_s=TASKA_RF_TD_RX_TIME,.t_s=TASKA_RF_TD_RX_PERIOD,.d_s=TASKA_RF_TD_RX_PERIOD,
                                     .p=TASKA_RF_TD_RX_PRIORITY,    .name=TASKA_RF_TD_RX_NAME,   .type=TASKA_RF_TD_RX_TYPE,//
                                     .pxTaskCode=task_rfsub1ghz_TD_rx_data,      .stack_size=TASKA_RF_TD_RX_STACKSIZE,
                                     };
 #endif
//------------------------------------------------------------------------------------------------
#if  (DEVICE_TYPE_KIND==DEVICE_TYPE_DCS)
 TaskData taskdata_rfsub_dcs_rx=     {.c_s=TASKA_RF_DSC_RX_TIME,.t_s=TASKA_RF_DSC_RX_PERIOD,.d_s=TASKA_RF_DSC_RX_PERIOD,
                                    .p=TASKA_RF_DSC_RX_PRIORITY,    .name=TASKA_RF_DSC_RX_NAME,   .type=TASKA_RF_DSC_RX_TYPE,//
                                    .pxTaskCode=task_rfsub1ghz_DCS_rx_data,      .stack_size=TASKA_RF_DSC_RX_STACKSIZE,
                                    };
#endif
//for access of each structure;
       TaskData* task_data_p[]= {
                                #ifdef RTOS_PRINTU
                                 &taskdata_printu,
                                #endif
                               #ifdef SD_STORAGE_ON
                                 &taskdata_spi0_microsd,
                                #endif
                                 &taskdata_pwmleds,
	                             &taskdata_adc0_sysmonitor,
                                #if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD)
		                         &taskdata_rfsub_keep_alive,
							     &taskdata_rfsub_tx_data,
		                         &taskdata_i2c0_imu_accgyro,
								 &taskdata_i2c0_imu_mag,
	                             &taskdata_temperature,
                                #ifdef IMU_ANIMAL_ACTIVITY_ENABLED
								 &taskdata_animal_activity,
                                #endif
                               #endif
                                #if ((DEVICE_TYPE_KIND==DEVICE_TYPE_MD)|(DEVICE_TYPE_KIND==DEVICE_TYPE_DCS))
                                    #if !defined(STATE_MACHINE_GPS_DISABLED)
	                                &taskdata_uart1_gps,
                                    #endif
                                #endif
                               #if  (DEVICE_TYPE_KIND==DEVICE_TYPE_TD)
								 &taskdata_rfsub_td_rx,//task_rfsub1ghz_tx_data_TD
                               #endif
                               #if (DEVICE_TYPE_KIND==DEVICE_TYPE_DCS)
								 &taskdata_rfsub_dcs_rx,
                              #endif
                                 };
#define task_data_p_n ((sizeof(task_data_p))/sizeof(TaskData*))//amounts of tasks
//#define MAX_RTOS_ALLTASKS_STACK_SIZE 19000 //commented: controled in "device_system_config.h"
uint8_t  rtos_tasks_stack[MAX_RTOS_ALLTASKS_STACK_SIZE];
static uint32_t rtos_tasks_stack_position=0;
//--------------------------
//----------------------------------------------------------------
//--------------------SEMAPHORES DECLARATION----------------------------------------------------------------------
sempaphore_struct sem_state_machine;
sempaphore_struct sem_i2c0;
sempaphore_struct sem_i2s0;
sempaphore_struct sem_spi0;//semaphore for sharing the spi bus acces
sempaphore_struct sem_uart0;
sempaphore_struct sem_uart1;       //semaphore for sharing the uart 1 bus acces
sempaphore_struct sem_adc0;        //semaphore for sharing the uart 1 bus acces
sempaphore_struct sem_rfsub1GHz;
sempaphore_struct sem_rfsub1GHz_packetTx;
//------------------------------------------------------------------------------------------
//-------------------------TASK SET UP, CREATION AND SCHEDULER START-----------------------------------------------------------------
//------------------------------------------------------------------------------------------
int task_set_parameters(TaskData* ptask_data){
    if (ptask_data->stack_size==0){
    ptask_data->stack_size=configMINIMAL_STACK_SIZE;
    ptask_data->stack_p=NULL;}//dynamic mem allocation
if  ((rtos_tasks_stack_position+ptask_data->stack_size)<=sizeof(rtos_tasks_stack)){
    //check if there is enough stack available to perform allocation
    ptask_data->stack_p=&(rtos_tasks_stack[rtos_tasks_stack_position]);
    rtos_tasks_stack_position=rtos_tasks_stack_position+ptask_data->stack_size;}
else{
    printuf_nortos("\r\nFATAL ERROR: NOT ENOUGH STACK TO START THE RTOS:\r\nStack required: %i, stack available: %i bytes, please increase your stack by: %i...",
                   rtos_tasks_stack_position+ptask_data->stack_size, sizeof(rtos_tasks_stack),rtos_tasks_stack_position+1-sizeof(rtos_tasks_stack));
    return(1);}//stack is not enough to allocate, so returns 1==erorr
ptask_data->c= SEC_TO(ptask_data->c_s);
ptask_data->t= SEC_TO(ptask_data->t_s);
ptask_data->d= SEC_TO(ptask_data->d_s);
return(0);
}
/*IMPORTANT: Be very aware of task stack overflow. This can cause your RTOS to crash even if creation was succesfull.
 * You can solve this by either increasing the asigned task stack or either removing functions like printf which increase
 * stack in consirable amounts*/

extern int rtos_task_creation(void);
int rtos_task_creation(void){
uint8_t i_tk; //task index
int                 retc;
//creates periodic tasks
printuf_nortos((const char*)"\r\nReal Time Operative System setup started");
printuf_nortos((const char*)"\r\nStarting to create tasks...");
for (i_tk=0; i_tk < task_data_p_n; i_tk++) { //task_data_p_n
      #ifdef RTOS_DP
      (task_data_p[i_tk]->p)=taskdualprioritys.lowband;//set default priority of periodic tasks to low band
      #endif
  	  //------------------first we init the system paramters and stop the process if some invalid parameters is entered
      if(task_set_parameters(task_data_p[i_tk])>0){return(1);}//if there is an issue setting pareters, exit the function
      //---------------------------------------------------------------------------
      task_data_p[i_tk]->i_tk=i_tk;//Store the index of the structure in the structure to allow acces from thread
      //----------------- start thread creation
      retc=task_create_AL(task_data_p[i_tk]);// start thread creation
      if (task_data_p[i_tk]->type==PERIODIC_TASK){printuf_nortos("\r\nCreating task \"%s\" with period %.2f sec., result: %i (0=SUCCESS, otherwise, FAIL)",task_data_p[i_tk]->name,task_data_p[i_tk]->t_s,retc);}
      else                                       {printuf_nortos("\r\nCreating task \"%s\" (APERIODIC), result: %i (0=SUCCESS, otherwise, FAIL)",task_data_p[i_tk]->name);}
      #ifdef RTOS_SS//-----------
      if (task_data_p[i_tk]->type==PERIODIC_TASK){
      vSlackSetTaskParams((task_data_p[i_tk]->handle),PERIODIC_TASK, task_data_p[i_tk]->t,
		  task_data_p[i_tk]->d,task_data_p[i_tk]->c, task_data_p[i_tk]->s_p  );}
      else{vSlackSetTaskParams(task_data_ap[i_tk]->handle, APERIODIC_TASK, 0, 0, 0, task_data_ap[i_tk]->s_p);}
      #endif
}
printuf_nortos("\r\nTASK creation done, total stack: %i of %i bytes available, proceeding to initialize other other RTOS elements...",rtos_tasks_stack_position, sizeof(rtos_tasks_stack));
//-----------------------------------------------------------
//---------------------Sempahores initialization
//-------------shared drivers-------------------------------
semaphore_init(&sem_i2c0,               SEC_TO(SEM_I2C0_TIMEOUT));
semaphore_init(&sem_i2s0,               SEC_TO(SEM_I2C0_TIMEOUT));
semaphore_init(&sem_spi0,               SEC_TO(SEM_SPI0_TIMEOUT));
semaphore_init(&sem_rfsub1GHz,          SEC_TO(SEM_RFSUB1GHZ_TIMEOUT));
semaphore_init(&sem_uart0,              SEC_TO(SEM_UART0_TIMEOUT));
semaphore_init(&sem_uart1,              SEC_TO(SEM_UART0_TIMEOUT));
semaphore_init(&sem_adc0,               SEC_TO(SEM_ADC0_TIMEOUT));
//-------------shared services--------------------------------------
semaphore_init(&sem_state_machine,      SEC_TO(SEM_STATE_TIMEOUT));
semaphore_init(&sem_rfsub1GHz_packetTx, SEC_TO(SEM_RFSUB1GHZ_PACKETTX_TIMEOUT));

/*pass the provided system-shared semaphore of the uart (used to print uart) interface*/
#ifdef RTOS_PRINTU
printu_give_shared_uart_semaphore(&sem_uart0);//must be called before "vTaskStart_Scheduler"
#endif
/*pass the provided  system-shared semaphore of the spi (used for memory storage) interface*/
#ifdef SD_STORAGE_ON
memory_store_give_shared_spi_semaphore(&sem_spi0);//must be called before "vTaskStart_Scheduler"
#endif
//---------------------START OPERATIVE SYSTEM IF STACK SIZE IS OK--------------------------------
if (rtos_tasks_stack_position<sizeof(rtos_tasks_stack)){
    /*Starting RTOS as requested*/
    printuf_nortos((const char*)"\r\nRTOS setup done, preparing for booting up");
    printuf_nortos((const char*)"\r\n***Starting Real Time Operative System***");
    vTaskStart_Scheduler();//starting scheduller, the print message below SHOULD NEVER be reached.
    printuf_nortos((const char*)"\r\n***FATAL ERROR: Real Time Operative System CRASH PLEASE SEE YOUR CODE***");
    return(0);
        }
 else{
     printuf_nortos((const char*)"\r\n FATAL ERROR: NOT ENOUGH STACK TO START THE RTOS");
     printuf_nortos("\r\nStack required: %i, stack available: %i bytes, please increase your stack by: %i...",
                    rtos_tasks_stack_position+1, sizeof(rtos_tasks_stack),rtos_tasks_stack_position+1-sizeof(rtos_tasks_stack));
    return(1);
 }
return(22);
}
