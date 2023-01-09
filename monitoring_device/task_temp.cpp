
#include <RTOS_AL/rtos_AL.h> //allow the use of an operative system
#include <RTOS_AL/semaphore_AL.h> //allow the use of an operative system
#include <lsm9ds1/LSM9DS1.h> //imu main library
#include  <memory_structures/memory_struct_temp.h>

#include "ti_drivers_config.h"       //to allow temperature measurements
#include <ti/drivers/Temperature.h> //to allow temperature measurements

extern LSM9DS1 imu;//declares the variable
extern TaskData* task_data_p[];
//sempaphores shared
extern sempaphore_struct sem_i2c0;  //semaphore for sharing the i2c bus acces
extern sempaphore_struct sem_spi0_microsd;//semaphore for sharing the i2c bus acces



static void foocpy (unsigned char* dst, unsigned char* src, uint16_t n)
{uint16_t i=0;
  for( i=0; i<n; i++){*(dst+i) = *(src+i);}
}
//------------------------------------------------------
//unsigned char microsd_write_buffer[SD_BLOCK_UNIT_SIZE];
mem_temp_union temperature_buffer{.mem.head.id_head=MEM_STR_ID_HEAD,
	 .mem.head.id_name=TEMP_STR_ID,.mem.head.block_size=TEMP_BLOCK_UNIT_SIZE};
static void set_store_struct(mem_temp_union* mem_struct,float sample_time)
{
mem_struct->mem.conf.sample_rate_seconds=(uint16_t) sample_time;
mem_struct->mem.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
}
static void reset_store_struct(mem_temp_union* mem_struct)
{
mem_struct->mem.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
}

//------------main task----------------------------------------------
extern "C" {
void  task_temp(uint8_t task_index);
}
void task_temp(uint8_t task_index)
{   //settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = task_data_p[task_index]->t;
	//configuring devices to use
	int8_t samples_to_add;
	uint16_t curr_pos,i;
    Temperature_init();
	//store all the character before enable the conversion
	if (semaphore_trytotake(&sem_i2c0)==true){
	 set_store_struct(&temperature_buffer,TO_SEC(xFrequency));
	 semaphore_release(&sem_i2c0);}
	// wait to driver becomes initialized
	 vTaskDelay(SEC_TO(5));//hold 5 seconds to start
     temperature_buffer.mem.head.acq_time_start=xTaskGetTickCount();    //time the task started
    for (;;){
    	      curr_pos=temperature_buffer.mem.head.data_written;
    	       if (semaphore_trytotake(&sem_i2c0)==true){
    	    	   temperature_buffer.mem.data[curr_pos].temp_imu=imu.calcTemp(imu.temperature); //get temperature from IMU sensor
    	    	   temperature_buffer.mem.data[curr_pos].temp_mcu=Temperature_getTemperature(); //get temperature from MCU
			  	   //update the trackers
    	    	   samples_to_add=1;//add 1 byte
    	    	   temperature_buffer.mem.head.data_written=curr_pos+samples_to_add;
			  	   //release the resource
			  	   semaphore_release(&sem_i2c0);
		  	       }
    	    //--------handle the data
	  	      if (temperature_buffer.mem.head.data_written> TEMP_DATA_LIMIT)//if an overflow of data is going to happen
	  	      {//copy data to SD buffer and perform opperation
	  	    	temperature_buffer.mem.head.acq_time_end=xTaskGetTickCount();//time the task ended
	  	  	     if (semaphore_trytotake(&sem_spi0_microsd)==true)//takes the sempaphore
	  	  	     {//copy all the bytes from the main structure to the data b8uffer
	  	  	        foocpy(microsd_write_buffer,temperature_buffer.byte,sizeof(temperature_buffer));
	  	  	        //call the sd function to create the thread
	  	  	         thread_create(task_data_p[0]);
	  	  	       //create a microsd component to store all
	  	  	       //after finishing, the micro sd will release the resource
	  	         }
	 	  	     reset_store_struct(&temperature_buffer);//reset the structure
	    	     temperature_buffer.mem.head.acq_time_start=xTaskGetTickCount();    //time the task started
	  	       }
              //------------------delay
	        vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
	  	   }
}
