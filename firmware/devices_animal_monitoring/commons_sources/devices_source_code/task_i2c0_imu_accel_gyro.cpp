#include "device_system_information.h"
#if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD)
#include <device_system_setup.h>
#include "rtos_tasks.h"          //allow the use of an operative system function
#include "state_machine.h"       //allow the use of an state machine
#ifdef RTOS_PRINTU
#include "task_uart_printu.h"   //for printing services
#endif
#include <memory_structures/memory_struct_imu0_accgyro.h>
#ifdef SD_STORAGE_ON
#include "task_spi0_microsd.h"         //for storage service
#endif
//for using the IMU LSM9DS1 driver
#include <lsm9ds1/LSM9DS1.h> //imu main library
// other libraries
#include <stdio.h>//to allow float to conversion
#include <string.h>
//for communicating with rf system
#include "task_rfsub1ghz_tx_data.h" //.h to interact with send "RF data" Task
//------------------------------------------------------
LSM9DS1 imu;//declares the variable
//-----------------------------------------------
//--------------------------------------------------
//For testing the IMU
//#define PRINT_IMU
//-----------------------------------------------------------------------------------
//--------functions for sd carg storing, all must be declared static to avoid issues
// ------------------------
typedef mem_imu0_a_g_union mem_struct;
//-----------------------------------------------------------------------------------

static mem_struct buffer_data_ag={
      {//.m
        {MEM_STR_ID_HEAD,IMU0AG_STR_ID,IMU0AG_BLOCK_UNIT_SIZE,0,0,0}, //head
        {0},                                                              //conf
        {{{0}}}
      }
};                                                               //data
//-------------------------------
static uint16_t data_counter=0;/*counter of adcquired data*/
static uint16_t data_to_add=0;//this is added only for this application
//-----------------------------------------------------
static void set_store_struct(mem_struct* pmem,LSM9DS1* imu){
    pmem->m.conf.gyro_on=imu->settings.gyro.enabled;
if (pmem->m.conf.gyro_on==1){pmem->m.conf.sample_rate=imu->settings.gyro.sampleRate;}//gyro ON: Determines sample rate
else{                                pmem->m.conf.sample_rate=imu->settings.accel.sampleRate;}//Gyro off: ACCel determines sample rate
pmem->m.conf.scale_gyro=imu->settings.gyro.scale;
pmem->m.conf.scale_accel=imu->settings.accel.scale;
pmem->m.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
//MESSAGE content: ",SR=,GY=0"
pmem->m.conf.message_to_user[0]=0;//to init
char c;
strncat(pmem->m.conf.message_to_user,",",1);//do not change to work with TICLANG compiler
strncat(pmem->m.conf.message_to_user,",SR=",4);//do not change to work with TICLANG compiler
c=pmem->m.conf.sample_rate +'0';
strncat(pmem->m.conf.message_to_user,&c,1);//do not change to work with TICLANG compiler
strncat(pmem->m.conf.message_to_user,",GY=",4);//do not change to work with TICLANG compiler
c=pmem->m.conf.gyro_on +'0';
strncat(pmem->m.conf.message_to_user,&c,1);//do not change to work with TICLANG compiler
}
static void reset_store_struct(void* pmem){
((mem_struct*) pmem)->m.head.data_written=0;//init the amount of written data arrays (this varies dependding on which is enabled)
}


void store_data(mem_struct* pmem){
      //copy data to SD buffer and perform opperation
    pmem->m.head.acq_time_end=xTaskGetTickCount();    //time the task ended
    // Display_printf(display, 0, 0, "time to finish task imu %3f\n",TO_SEC( pbuffer_data->mem.head.acq_time_end));
#ifdef SD_STORAGE_ON
    memory_store(pmem);
#endif
	reset_store_struct(pmem);
	pmem->m.head.acq_time_start=xTaskGetTickCount();    //time the task started
}

#ifdef IMU_ANIMAL_ACTIVITY_ENABLED
#include "task_animal_activity.h"    //for IMU structures
 static animal_activity_enum current_animal_state=ANIMAL_;
 static animal_activity_struct new_animal_state;
 static char animal_activity_str[90];
#ifdef IMU_ANIMAL_ACTIVITY_CHANGE_SENSORS_ENABLED_IMU
 static uint32_t state_still_elapsed_time=SEC_TO(IMU_STILL_TIMEOUT);
#endif
 //------------------------------------------------------
void animal_activity_accgyro_set(mem_struct* pbuffer_data,LSM9DS1* pimu){
	estimate_animal_activity_from_accel(&(pimu->buffer_ag),pimu->settings.gyro.enabled,pimu->FIFO_samples_available);
	new_animal_state=get_animal_activity();
	if (((new_animal_state.state!=ANIMAL_)and(current_animal_state!=new_animal_state.state))){
	    current_animal_state=new_animal_state.state;//store new state
		animal_activity_str[0]=0;                               //(uint32_t)(TO_SEC(
	    sprintf(animal_activity_str, "\r\n!NEW ANIMAL ACTIVITY DETECTED, CURRENT STATE: %i, at time: %.3f [sec]",(uint8_t)current_animal_state,TO_SEC(xTaskGetTickCount()));//store timeout as parameters
        #ifdef SD_STORAGE_ON
	    memory_store_str((void*)animal_activity_str,sizeof(animal_activity_str));
        #endif
        //------------------------------------------------------------------------------
		#ifdef IMU_ANIMAL_ACTIVITY_CHANGE_SENSORS_ENABLED_IMU
		//--At this point, system will use the animal state information to enable/disable GYRO due to the power consumption
			 //check if animal is moving and gyro is disabled
        #ifdef IMU_ANIMAL_ACTIVITY_CHANGE_SENSORS_TURN_OFFIMU

			if (new_animal_state.state==ANIMAL_MOVING){//TURN ON ALL
		      if (pimu->settings.gyro.enabled==false){ //must check if gyro is already enable
		       // sprintf(animal_activity_str, "\r\nturning ON gyro");//store timeout as parameters
		        //memory_store_str((void*)animal_activity_str,15);
              //-------------------------------------------------
			    store_data(pbuffer_data);//dump data before changing sensor
				if (semaphore_trytotake(&sem_i2c0)==true){
				    pimu->turn_onGyro();
				    pimu->enableFIFO_continous();//Starts adquisition in continous mode
					set_store_struct(pbuffer_data,&imu);
					pbuffer_data->m.head.acq_time_start=xTaskGetTickCount();    //time the task started
					semaphore_release(&sem_i2c0);
				}
		      }
           }
        #endif
		#endif
	  }
	//-------this part does not relay on new actitities, so turn off
     #ifdef IMU_ANIMAL_ACTIVITY_CHANGE_SENSORS_ENABLED_IMU
    //check if animal is still for at leat IMU_STILL_TIMEOUT seconds
	if (pimu->settings.gyro.enabled==true){
     if ((new_animal_state.state==ANIMAL_STILL)and(new_animal_state.state_elapsed_time>state_still_elapsed_time) ){
            /*sprintf(animal_activity_str, "\r\nturning OFF gyro");//store timeout as parameters
            memory_store_str((void*)animal_activity_str,15);
             */
         store_data(pbuffer_data);//dump data before changing sensor
        if (semaphore_trytotake(&sem_i2c0)==true){
         pimu->turn_offGyro();//only turn off gyro, but accel in fifo mode will be active. Because of this, DO NOT TURN OFF FIFO MODE!!
         set_store_struct(pbuffer_data,pimu);//store the data with this setup
         semaphore_release(&sem_i2c0);
        }
      }
     }
    #endif

}
#endif

//-----------------------------------------------------------------
//----------------------------------------------------------------
inline int check_and_setup_data_acquisition_sensor_ag(LSM9DS1* pimu,TaskData* ptask_data){
    int setup_imu=-1;
    if (semaphore_trytotake_time(&sem_i2c0,SEC_TO(4))==true){
        setup_imu=pimu->begin();//with no arguments, this uses default addresses (AG:0x6B, M:0x1E) and i2c port (Wire).
        pimu->settings.gyro.sampleRate = IMU_ACCEL_GYRO_SAMPLE_RATE; // 1 = 14.9 Hz,2 = 59.5 , 3 = 119  4 = 238  5 = 476 6= 952 Hz
        semaphore_release(&sem_i2c0);//release the resource
    }
   #ifdef RTOS_PRINTU
    vTaskDelay(SEC_TO(0.1));//WAIT TO ENSURE A PROPER SET UP OF printuf task
    printuf("\r\n%s:",ptask_data->name);
    if (setup_imu ==-1){printuf((char const*)"Could not take I2C0 driver to set up IMU\n");}
    if (setup_imu ==0) {printuf((char const*)"IMU setted up succesfully");}
    if (setup_imu ==1) {printuf((char const*)"IMU i2c0 fail to set up \n");}
    if (setup_imu ==2) {printuf((char const*)"IMU i2c0 setted ok, but failed to communicate with both IMU ACCEL/GYRO and magnetomer\n");}
    if (setup_imu ==3) {printuf((char const*)"IMU i2c0 setted ok, but failed to communicate with IMU magnetomer, but could not communicate with ACCEL/GYRO \n");}
    if (setup_imu ==4) {printuf((char const*)"IMU i2c0 setted ok, but failed to communicate with IMU ACCEL/GYRO, but could not communicate with magnetomer\n");}
    if (setup_imu ==10){printuf((char const*)"IMU already initialized");}
   #endif
  return(setup_imu);
}
/*set up necesary means to start data adquisition*/
inline void start_data_acquisition_ag(mem_struct* pbuffer_data,LSM9DS1* pimu){
    //Display_printf(display, 0, 0, "ADQ state reach\n")
    if (semaphore_trytotake_time(&sem_i2c0,SEC_TO(4))==true){
        set_store_struct(pbuffer_data,&imu);
        //before staring the conversionn
        pimu->turn_onAccel();
        pimu->turn_onGyro();
        pimu->enableFIFO_continous();//Starts adquisition in continous mode
        semaphore_release(&sem_i2c0);
        }
   pbuffer_data->m.head.acq_time_start=xTaskGetTickCount();    //time the task ended
}
/*adquires X samples of data*/
static inline void acquire_data_ag(mem_struct* pbuffer_data,LSM9DS1* pimu){
    uint16_t i ;
    data_counter=pbuffer_data->m.head.data_written;
    if (semaphore_trytotake(&sem_i2c0)==true){
        pimu->readAccGyro(); //read all the data from gyro/acc using fifo mode
        data_to_add=pimu->FIFO_samples_available;//bytes to cpy
        //update the trackers if according
        if ((data_counter+data_to_add)<(sizeof(pbuffer_data->m.d)-1)){//overflow protection
           if(pimu->settings.gyro.enabled){
            for ( i = 0; i < (data_to_add); ++i){ pbuffer_data->m.d[data_counter+i].gyro_accel=pimu->buffer_ag.gyro_accel[i];}}//copy the data
           else{  for ( i = 0; i < (data_to_add); ++i){ pbuffer_data->m.d[data_counter+i].accel=pimu->buffer_ag.accel[i];}}//copy the data}
           data_counter=data_counter+data_to_add;
           pbuffer_data->m.head.data_written=data_counter;
          }
        semaphore_release(&sem_i2c0);
      }
}
/*store data if buffer is above limit*/
static inline void store_data_ag(mem_struct* pbuffer_data ,uint16_t data_limit){
 int mem_store_ret=1;
    if (pbuffer_data->m.head.data_written> (data_limit-16)){//if an overflow of data is going to happen
        pbuffer_data->m.head.acq_time_end=xTaskGetTickCount();    //time the task ended
      #ifdef SD_STORAGE_ON
        mem_store_ret=memory_store(pbuffer_data);
     #endif
        //reset the store struct after ending the write process
        if (mem_store_ret==0){reset_store_struct(pbuffer_data);} //this is to avoid the loss of data if the system cannot get the sd driver in time
        pbuffer_data->m.head.acq_time_start=xTaskGetTickCount();    //time the task has started
    }
}
/*stop and turn off any resource used to adquite this kind of data*/
static inline void stop_data_acquisition_ag(LSM9DS1* pimu){
if (semaphore_trytotake(&sem_i2c0)==true){
        pimu->turn_off(); //i2c_close();//at the moment this is close inside the driver
        semaphore_release(&sem_i2c0);}
}
/*print data, only used for debug or testing*/
#ifdef PRINT_IMU
#define PRINT_INTERVAL
uint32_t last_time_print=0;
inline void print_data(mem_struct* pbuffer_data,LSM9DS1* pimu){
  if((xTaskGetTickCount()-last_time_print)>SEC_TO(1)){
    last_time_print=xTaskGetTickCount();
    if (semaphore_trytotake(&sem_i2c0)==true){
        printuf("\r\nSD Buffer position is: %i ",pbuffer_data->m.head.data_written);
        printuf("\r\nTemperature IMU: %.2f",pimu->calcTemp(pimu->temperature));
        printuf("\r\nAdquired Samples: %i",data_to_add);
        printuf("\r\nGyro reading FIFO x: %.1f y: %.1f z: %.1f",pimu->calcGyro(pimu->buffer_ag.gyro_accel[0].gx),pimu->calcGyro(pimu->buffer_ag.gyro_accel[0].gy),pimu->calcGyro(pimu->buffer_ag.gyro_accel[0].gz));
        printuf("\r\nAccel reading FIFO x: %.1f y: %.2f z: %.2f",pimu->calcAccel(pimu->buffer_ag.gyro_accel[0].ax),pimu->calcAccel(pimu->buffer_ag.gyro_accel[0].ay),pimu->calcAccel(pimu->buffer_ag.gyro_accel[0].az));
        semaphore_release(&sem_i2c0);
        }

  }
}
#endif
//-----------------------------------------------------------------
//----------------------------------------------------------------
extern "C" {
 void  task_i2c0_imu_accgyro(void* ptask_data);
}
void task_i2c0_imu_accgyro(void* ptask_data){   //settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = ((TaskData*) ptask_data)->t;
	//TickType_t clocktick_start,clocktick_end;
	uint16_t sensor_data_limit=(sizeof(buffer_data_ag.m.d)/sizeof(buffer_data_ag.m.d[0]))-IMU0AG_DATA_OFFSET_LIMIT;
	//------check and setup sensor
	check_and_setup_data_acquisition_sensor_ag(&imu,(TaskData*) ptask_data);
	//wait for acq state
	state_machine_wait_for_ADQ_DATA_state();
    //init acq
    start_data_acquisition_ag(&buffer_data_ag,&imu);
    while (1){
		      acquire_data_ag(&buffer_data_ag,&imu);
		       //-----store data
		      store_data_ag(&buffer_data_ag,sensor_data_limit);
		      //------do thing based on animal activity
              #ifdef IMU_ANIMAL_ACTIVITY_ENABLED
		       animal_activity_accgyro_set(&buffer_data_ag,&imu);//sets the acceleerometer based on activity
		       update_tx_packet_animal_activity(new_animal_state.state,(uint32_t)(TO_SEC((new_animal_state.state_elapsed_time))));
              #endif
		       //-------print data
               #ifdef PRINT_IMU
		       print_data(&buffer_data_ag,&imu);
               #endif
		      //check to continue if data acq should continue
		      if(state_machine_is_lowpower()){break;}//exit process
		      //------------------delay until time of acq begins again
		     vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
       }
		     //end of task. If this point is reached, -->store all the data we have and turn off the sensor
  //end of task. If this point is reached, the System monitor task has deleted this task--->store all the data we have
#ifdef SD_STORAGE_ON
    memory_store(&buffer_data_ag);//dump buffer without checking the amount
#endif
stop_data_acquisition_ag(&imu);
    while(1){vTaskDelay(TASK_SLEEP_FOREVER);}
}
#endif

