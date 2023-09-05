#include <device_system_setup.h>
//system general  libraries
#include "rtos_tasks.h"          //allow the use of an operative system function
#ifdef RTOS_PRINTU
#include "task_uart_printu.h"    //for printing services
#endif
//-------------proyect libraires
#include "task_animal_activity.h"    //for IMU structures

/*convert animal activity numer to string, must be defined even if libary is not called*/
uint8_t get_animal_activity_str(char* c,uint8_t num){
    uint8_t c_amount;
 if(num==0){ c_amount=sprintf(c,"Unknown");}
 if(num==1){c_amount=sprintf(c,"Still");}
 if(num==2){c_amount=sprintf(c,"Moving");}
return(c_amount);
}
//
#ifdef IMU_ANIMAL_ACTIVITY_ENABLED
static animal_activity_struct animal_activity;
static animal_activity_enum   animal_activity_newstate;
static uint32_t last_activity_state_clock;

//-------------------------parameters
static sempaphore_struct sem_animal_activity;
static sempaphore_struct sem_animal_activity_wait;
static sempaphore_struct sem_animal_activity_wait_for_processing;   //for internal use
#define SEM_ANIMAL_ACTIVITY_TIMEOUT			1//11 ms is max time for conversion
static TaskData* ptaskdata_animal_activity;//pointer to microsd task struct

//

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h> //for memmove

typedef int16_t var_type;

#define BUFFER_SIZE 64
var_type buffer_ax [BUFFER_SIZE];
var_type buffer_ay [BUFFER_SIZE];
var_type buffer_az [BUFFER_SIZE];
/*Computes the average of <amount_average> samples, then check if the  */
bool check_dataint16_average_above_treshold(var_type* data, uint16_t amount,var_type* buffer_av, uint16_t amount_average,var_type treshold){
uint16_t i, j;
int32_t average;
bool avobe_treshold=false;//return false as default
for (i=0;i<amount; ++i) {
    //get average of signal buffer
    average=0;
    for(j=0;j<amount_average;++j){average=average+ buffer_av[j]; }
    average=average/amount_average;
    //compare if signal-minus-average surpass the treshold
    if ( ((data[i]-average)>(treshold))|((average-data[i])>(treshold))){
        avobe_treshold=true;//set flag as true
    }
    //shift average buffer and adds new element for next calculation
    memmove(buffer_av, &(buffer_av[1]), (amount_average-1)*sizeof(var_type));
    buffer_av[amount_average-1]=data[i];
}
return (avobe_treshold);
}
//-------------------------parameters//-------------------------parameters
static var_type accel_data_ax[32];
static var_type accel_data_ay[32];
static var_type accel_data_az[32];
static uint16_t accel_data_amount;
/*
var_type new_data[]={10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,
                     10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10900,10000};
int main()
{
    printf("\r\n Hello World, size of var is %i",sizeof(var_type));
    bool result=false;
    result=process_new_data(new_data,sizeof(new_data)/sizeof(var_type),BUFFER_SIZE, 400); //sizeof(new_data)
    printf("\r\n second time");
    result=process_new_data(new_data,sizeof(new_data)/sizeof(var_type),BUFFER_SIZE, 800); //sizeof(new_data)
    if (result){ printf("\n peak found");}
    return 0;
}
*/

extern "C"	{
void task_animal_activity_processing(void* ptask_data);
void task_animal_activity( void* ptask_data);
}
/*Computes animal activity and returns a value*/
void task_animal_activity(void* ptask_data)
{
	ptaskdata_animal_activity=(TaskData*) ptask_data;
	//init variabled
	animal_activity.state=ANIMAL_MOVING;
	animal_activity.state_elapsed_time=0;
	last_activity_state_clock=xTaskGetTickCount();
    //init semaphores
	semaphore_init(&sem_animal_activity,SEC_TO(SEM_ANIMAL_ACTIVITY_TIMEOUT));
	semaphore_init(&sem_animal_activity_wait_for_processing,SEC_TO(SEM_ANIMAL_ACTIVITY_TIMEOUT));
	semaphore_trytotake(&sem_animal_activity_wait_for_processing);//take int in order for use the task as a non blocking process
	if(semaphore_trytotake(&sem_animal_activity)==true){
		semaphore_release(&sem_animal_activity);//release the resource
	}
	semaphore_init(&sem_animal_activity_wait,SEM_WAIT_FOREVER);
	semaphore_trytotake(&sem_animal_activity_wait);//release the resource
while(1){
         semaphore_trytotake(&sem_animal_activity_wait);//awaits a resource release to write
         task_animal_activity_processing((TaskData*) ptask_data);
}
}
void task_animal_activity_processing(void* ptask_data){
	//animal_activity_newstate=ANIMAL_EATING;//generates a new state
    //get the current new state
    bool tresh_ax_above,tresh_ay_above,tresh_az_above;
    tresh_ax_above=check_dataint16_average_above_treshold(accel_data_ax,accel_data_amount,buffer_ax,IMU0_ACCEL_AVERAGE_SAMPLES_TORTOISE,IMU0_ACCEL_TRESHOLD_TORTOISE);
    tresh_ay_above=check_dataint16_average_above_treshold(accel_data_ay,accel_data_amount,buffer_ay,IMU0_ACCEL_AVERAGE_SAMPLES_TORTOISE,IMU0_ACCEL_TRESHOLD_TORTOISE);
    tresh_az_above=check_dataint16_average_above_treshold(accel_data_az,accel_data_amount,buffer_az,IMU0_ACCEL_AVERAGE_SAMPLES_TORTOISE,IMU0_ACCEL_TRESHOLD_TORTOISE);
	if ((tresh_ax_above)or(tresh_ay_above)or(tresh_az_above)){
	     animal_activity_newstate=ANIMAL_MOVING; }
	else{animal_activity_newstate=ANIMAL_STILL;}
	//update systems variables
	if (animal_activity_newstate!=animal_activity.state){

		 animal_activity.state=animal_activity_newstate;
		 animal_activity.state_elapsed_time=0;
		 last_activity_state_clock=xTaskGetTickCount();
		}
    semaphore_release(&sem_animal_activity_wait_for_processing);
	semaphore_release(&sem_animal_activity);//release the resource
}

void estimate_animal_activity_from_accel(FIFO_buffer_struct* imuLSM9DS1_data, uint8_t gyro_enable, uint16_t amount_of_data ){
	uint16_t i;
    if (semaphore_trytotake(&sem_animal_activity)==true){
	//now we have the size of the buffer, so we proceed to perform a complete copy after writing
        accel_data_amount=amount_of_data;
        if(gyro_enable){for (i=0;i<accel_data_amount; ++i) {
                        accel_data_ax[i]=imuLSM9DS1_data->gyro_accel[i].ax;
                        accel_data_ay[i]=imuLSM9DS1_data->gyro_accel[i].ay;
                        accel_data_az[i]=imuLSM9DS1_data->gyro_accel[i].az;
                         }}
                   else{for (i=0;i<accel_data_amount; ++i) {
                        accel_data_ax[i]=imuLSM9DS1_data->accel[i].ax;
                        accel_data_ay[i]=imuLSM9DS1_data->accel[i].ay;
                        accel_data_az[i]=imuLSM9DS1_data->accel[i].az;
                        }}
        semaphore_release(&sem_animal_activity_wait);//release to indicate to #get_animal_activity# that processing has been done
        semaphore_trytotake(&sem_animal_activity_wait_for_processing);
	}
}
/*Thread-safely functions returns to the current animal state and the time as clock ticks  in this state . */
animal_activity_struct get_animal_activity(void){
	animal_activity_struct animal_activity_ret={ANIMAL_,0};
    if (semaphore_trytotake(&sem_animal_activity)==true){
    	   animal_activity.state_elapsed_time=xTaskGetTickCount()-last_activity_state_clock;
    	   animal_activity_ret.state=animal_activity.state;
    	   animal_activity_ret.state_elapsed_time=animal_activity.state_elapsed_time;
	  	   semaphore_release(&sem_animal_activity);
	}
    return (animal_activity_ret);
}

#endif
