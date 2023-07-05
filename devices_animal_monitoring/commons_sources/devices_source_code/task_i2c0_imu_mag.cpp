#include "device_system_information.h"
#if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD)
#include <device_system_setup.h>
#include "rtos_tasks.h"          //allow the use of an operative system function
#include "state_machine.h"       //allow the use of an state machine
#ifdef RTOS_PRINTU
#include "task_uart_printu.h"   //for printing services
#endif
#include  <memory_structures/memory_struct_imu0_mag.h> //for storage
#ifdef SD_STORAGE_ON
#include "task_spi0_microsd.h"         //for storage service
#endif
//for using the IMU LSM9DS1 driver
#include <lsm9ds1/LSM9DS1.h> //imu main library
//----------------------------------------------------
extern LSM9DS1 imu;//declares the variable

//------------------------------------------------------
typedef mem_imu0_mag_union mem_struct;
static mem_struct buffer_data_mag={
      {//.m
        {MEM_STR_ID_HEAD,IMU0M_STR_ID,IMU0M_BLOCK_UNIT_SIZE,0,0,0}, //head
        {0},                                                              //conf
        {{0}}                                                               //data
      }
      };
//-------------------------------
static uint16_t data_counter=0;/*counter of adcquired data*/
//-----------------------------------------------------
static void set_store_struct(mem_struct* pmem,LSM9DS1* imu)
{
pmem->m.conf.sample_rate=imu->settings.mag.sampleRate;
pmem->m.conf.scale=imu->settings.mag.scale;
pmem->m.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
}
static void reset_store_struct(void* pmem){
((mem_struct*) pmem)->m.head.data_written=0;//init the amount of written data arrays (this varies dependding on which is enabled)
}
//-----------------------------------------------------------------
//----------------------------------------------------------------
inline int check_and_setup_data_acquisition_sensor_mag(LSM9DS1* pimu,TaskData* ptask_data){
    int setup_imu=-1;
    if (semaphore_trytotake_time(&sem_i2c0,SEC_TO(4))==true){
        setup_imu=pimu->begin();//with no arguments, this uses default addresses (AG:0x6B, M:0x1E) and i2c port (Wire).
        pimu->settings.mag.sampleRate = IMU_MAG_SAMPLE_RATE; //0=0.625 Hz ,1=1.25 ,2=2.5, 3=5,4 = 10,5= 20,6=40, 7= 80 Hz
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
static inline void start_data_acquisition_mag(mem_struct* pbuffer_data,LSM9DS1* pimu){
    vTaskDelay(SEC_TO(1));//waits  to ensure this started properly by imu task
    if (semaphore_trytotake(&sem_i2c0)==true){
       set_store_struct(pbuffer_data,pimu);
       pimu->turn_onMag();//init the magnetometer engine
       semaphore_release(&sem_i2c0);}
     //for first adquisition
    pbuffer_data->m.head.acq_time_start=xTaskGetTickCount();    //time the task started
}
/*adquires X samples of data*/
static inline void acquire_data_mag(mem_struct* pbuffer_data,LSM9DS1* pimu){
    data_counter=pbuffer_data->m.head.data_written;
    if (semaphore_trytotake(&sem_i2c0)==true){
        pimu->readMag();
        pbuffer_data->m.d[data_counter].x=pimu->mx;
        pbuffer_data->m.d[data_counter].y=pimu->my;
        pbuffer_data->m.d[data_counter].z=pimu->mz;
        semaphore_release(&sem_i2c0);
        //update the trackers if according
        if (data_counter<(sizeof(pbuffer_data->m.d)-1)){++data_counter;}
        pbuffer_data->m.head.data_written=data_counter;
        }
}
/*store data if buffer is above limit*/
static inline void store_data_mag(mem_struct* pbuffer_data,uint16_t data_limit){
 int mem_store_ret=1;
  if (pbuffer_data->m.head.data_written> (data_limit-20))//if an overflow of data is going to happen, store the buffer
    {//copy data to SD buffer and perform opperation
       pbuffer_data->m.head.acq_time_end=xTaskGetTickCount();    //time the task ended
       #ifdef SD_STORAGE_ON
       mem_store_ret=memory_store(pbuffer_data);
        #endif
       //reset the store struct after ending the write process (if this was succesfull (ret=0))
       if (mem_store_ret==0){reset_store_struct(pbuffer_data);} //this is to avoid the loss of data if the system cannot get the sd driver in time
       pbuffer_data->m.head.acq_time_start=xTaskGetTickCount();    //time the task has started
     }
}
/*stop and turn off any resource used to adquite this kind of data*/
static inline void stop_data_acquisition_mag(LSM9DS1* pimu){
    if (semaphore_trytotake(&sem_i2c0)==true){
                pimu->turn_off(); //i2c_close();//at the moment this is close inside the driver
                semaphore_release(&sem_i2c0);}
}
//-----------------------------------------------------------------
//----------------------------------------------------------------
//------------main task----------------------------------------------
extern "C" {
void  task_i2c0_imu_mag(void* ptask_data);
}
void task_i2c0_imu_mag(void* ptask_data){
	//settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = ((TaskData*) ptask_data)->t;
	//TickType_t clocktick_start,clocktick_end;
    uint16_t sensor_data_limit=(sizeof(buffer_data_mag.m.d)/sizeof(buffer_data_mag.m.d[0]))-IMU0M_DATA_OFFSET_LIMIT;
	//configuring devices to use
	check_and_setup_data_acquisition_sensor_mag(&imu,(TaskData*) ptask_data);
	//wait for acq state
	 state_machine_wait_for_ADQ_DATA_state();
   //--------------
    start_data_acquisition_mag(&buffer_data_mag,&imu);
    while (1) { //adquire data and stores into global buffer
               acquire_data_mag(&buffer_data_mag,&imu);
               //-----------------------//store data
               store_data_mag(&buffer_data_mag,sensor_data_limit);
               //check to continue if data acq should continue
               if(state_machine_is_lowpower()){break;}//exit process
               //------------------delay until time of acq begins again
               vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
              }
    //end of task. If this point is reached, -->store all the data we have and turn off the sensor
#ifdef SD_STORAGE_ON
    memory_store(&buffer_data_mag);
    #endif
    stop_data_acquisition_mag(&imu);
    while(1){vTaskDelay(TASK_SLEEP_FOREVER);}
}
#endif
