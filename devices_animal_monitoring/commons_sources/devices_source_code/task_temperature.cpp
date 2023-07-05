#include "device_system_information.h"
#if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD)
//system general  libraries
#include "rtos_tasks.h"          //allow the use of an operative system function
#include "state_machine.h"       //allow the use of an state machine
#ifdef RTOS_PRINTU
#include "task_uart_printu.h"    //for printing services
#endif
#ifdef SD_STORAGE_ON
#include "task_spi0_microsd.h"    //for storage service
#endif
//---------specific libraries
//storage unit library
#include <memory_structures/memory_struct_system.h> //for storage unit
//specific libraries of this task
//for printing service
#include  <memory_structures/memory_struct_temp.h>
#include "task_rfsub1ghz_tx_data.h"

#include <lsm9ds1/LSM9DS1.h> //imu main library

#include "ti_drivers_config.h"       //to allow temperature measurements
#include <ti/drivers/Temperature.h> //to allow temperature measurements

extern LSM9DS1 imu;//declares the variable

//------------------------------------------------------
typedef mem_temp_union mem_struct;
//------------------------------------------------------
static  mem_struct buffer_data_temp={
      {//.m
        {MEM_STR_ID_HEAD,TEMP_STR_ID,TEMP_BLOCK_UNIT_SIZE,0,0,0}, //head
        {0},                                                     //conf
        {{0}}                                                     //data
      }
      };
//-------------------------------
static uint16_t data_counter=0;/*counter of adquited data*/
static int16_t last_temp_mcu=0;/*counter of adquited data*/
static float last_temp_imu=0;/*counter of adquited data*/
//----------------------------
static void set_store_struct(mem_struct* pmem,float sample_time)
{
    pmem->m.conf.sample_rate_seconds=(uint16_t) sample_time;
    pmem->m.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
}
static void reset_store_struct(void* pmem){
((mem_data_union*) pmem)->m.head.data_written=0;//init the amount of written data arrays (this varies dependding on which is enabled)
}

//------inl;ine void function
inline int check_and_setup_data_acquisition_sensor_temp(LSM9DS1* pimu,TaskData* ptask_data){
    int setup_imu=-1;
    Temperature_init();//init MCU temperature meassure system
    if (semaphore_trytotake_time(&sem_i2c0,SEC_TO(4))==true){
        setup_imu=pimu->begin();//with no arguments, this uses default addresses (AG:0x6B, M:0x1E) and i2c port (Wire).
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
    printuf((char const*)"\r\nAlso MCU intern Temperature sensor was correcly setted up");
   #endif
  return(setup_imu);
}
/*set up necesary means to start data adquisition*/
static inline void start_data_acquisition_temp(mem_struct* pbuffer_data,LSM9DS1* pimu){
    //store all the character before enable the conversion
    // wait to driver becomes initialized
     vTaskDelay(SEC_TO(1));//hold 5 seconds to start
     pbuffer_data->m.head.acq_time_start=xTaskGetTickCount();    //time the task started
}
/*adquires X samples of data*/
static inline void acquire_data_temp(mem_struct* pbuffer_data,LSM9DS1* pimu){
     data_counter=pbuffer_data->m.head.data_written;
     if (semaphore_trytotake(&sem_i2c0)==true){
         //get the data
         #if (DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE)
         pimu->readTemp();
         #endif
         last_temp_imu=pimu->calcTemp(pimu->temperature);
         last_temp_mcu=Temperature_getTemperature();
         pbuffer_data->m.d[data_counter].temp_imu=last_temp_imu; //get temperature from IMU sensor. this task SHOULD NEVER ADQUIRE IT
         pbuffer_data->m.d[data_counter].temp_mcu=last_temp_mcu; //get temperature from MCU
         semaphore_release(&sem_i2c0);
       }
     //update the trackers if according
     if (data_counter<(sizeof(pbuffer_data->m.d)-1)){++data_counter;}
     pbuffer_data->m.head.data_written=data_counter;
}
/*store data if buffer is above limit*/
static inline void store_data_temp(mem_struct* pbuffer_data,uint16_t data_limit){
    int mem_ret=1;
    if (pbuffer_data->m.head.data_written> (data_limit-5)){//if an overflow of data is going to happen
     //copy data to SD buffer and perform opperation
      pbuffer_data->m.head.acq_time_end=xTaskGetTickCount();//time the task ended
#ifdef SD_STORAGE_ON
      mem_ret=memory_store(pbuffer_data);//copy all the bytes from the main structure to the data b8uffer ansd store it
#endif
      if (mem_ret==0) {reset_store_struct(pbuffer_data);}//reset the structure
      pbuffer_data->m.head.acq_time_start=xTaskGetTickCount();    //time the task started
     }
}
/*stop and turn off any resource used to adquite this kind of data*/
static inline void stop_data_acquisition_temp(void){
//in this sensor, there is no need to do something. This is left declared for consistency with other tasks
 ;;
}
//------------------------------------------------------
//------------------------------------------------------
//------------main task----------------------------------------------
extern "C" {
void  task_temperature(void* ptask_data);
}
void task_temperature(void* ptask_data)
{   //settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = ((TaskData*) ptask_data)->t;
	//wait for acquisition state to begin to adquire
	//declare adquisition limit
    uint16_t sensor_data_limit=(sizeof(buffer_data_temp.m.d)/sizeof(buffer_data_temp.m.d[0]))-TEMP_DATA_OFFSET_LIMIT;
    check_and_setup_data_acquisition_sensor_temp(&imu,(TaskData*) ptask_data);
    //update the RF packet with the value of the MCU sensor
     #if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD)
    update_tx_packet_temperature(Temperature_getTemperature(),0);
    #endif
    //WAIT FOR DATA ACQ STATE
    state_machine_wait_for_ADQ_DATA_state();
	/*configuring devices to use*/
    set_store_struct(&buffer_data_temp,TO_SEC(xFrequency));
	start_data_acquisition_temp(&buffer_data_temp,&imu);
    while (1) { //adquire data and stores into global buffer
               acquire_data_temp(&buffer_data_temp,&imu);
    	       //-----------------------//store data
               store_data_temp(&buffer_data_temp,sensor_data_limit);//stores data if it is above limit
               //update rf packet
               #if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD)
               update_tx_packet_temperature(last_temp_mcu,(int8_t) last_temp_imu);
                #endif
	  	       //check to continue if data acq should continue
		       if(state_machine_is_lowpower()){break;}//exit process
		       //------------------delay until time of acq begins again
	           vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
	  	      }
    //end of task. If this point is reached, -->store all the data we have and turn off the sensor
#ifdef SD_STORAGE_ON
    memory_store(&buffer_data_temp);
#endif
    stop_data_acquisition_temp();
    while(1){vTaskDelay(TASK_SLEEP_FOREVER);}

}
#endif
