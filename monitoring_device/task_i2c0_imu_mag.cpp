/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== i2copt3001_cpp.cpp ========
 */

#include <RTOS_AL/rtos_AL.h> //allow the use of an operative system
#include <RTOS_AL/semaphore_AL.h> //allow the use of an operative system
#include <lsm9ds1/LSM9DS1.h> //imu main library
#include  <memory_structures/memory_struct_imu0_mag.h>

extern LSM9DS1 imu;//declares the variable
extern TaskData* task_data_p[];
//sempaphores shared
extern sempaphore_struct sem_i2c0;  //semaphore for sharing the i2c bus acces
extern sempaphore_struct sem_spi0_microsd;//semaphore for sharing the i2c bus acces



inline void foocpy (unsigned char* dst, unsigned char* src, uint16_t n)
{uint16_t i=0;
  for( i=0; i<n; i++){*(dst+i) = *(src+i);}
}
//------------------------------------------------------
//unsigned char microsd_write_buffer[SD_BLOCK_UNIT_SIZE];
static mem_imu0_mag_union imu_mag_buffer{.mem.head.id_head=MEM_STR_ID_HEAD,
	 .mem.head.id_name=IMU0M_STR_ID,.mem.head.block_size=IMU0M_BLOCK_UNIT_SIZE};
static void set_store_struct(mem_imu0_mag_union* mem_struct,LSM9DS1* imu)
{
mem_struct->mem.conf.sample_rate=imu->settings.mag.sampleRate;
mem_struct->mem.conf.scale=imu->settings.mag.scale;
mem_struct->mem.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
}
static void reset_store_struct(mem_imu0_mag_union* mem_struct)
{
mem_struct->mem.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
}

//------------main task----------------------------------------------
extern "C" {
void  task_i2c0_imu_mag(uint8_t task_index);
}
void task_i2c0_imu_mag(uint8_t task_index)
{   //settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t clocktick_start,clocktick_end;
	//configuring devices to use
	int8_t samples_to_add;
	uint16_t curr_pos,i;
	//store all the character before enable the conversion
	if (semaphore_trytotake(&sem_i2c0)==true){
	 set_store_struct(&imu_mag_buffer,&imu);
	 semaphore_release(&sem_i2c0);}
	//Wait until a conversion is performed
    //imu.magAvailable();
	//before staring the conversionn
	 vTaskDelay(SEC_TO(5));//hold 5 seconds to start
	 imu_mag_buffer.mem.head.acq_time_start=xTaskGetTickCount();    //time the task started
    for (;;){
	  	       curr_pos=imu_mag_buffer.mem.head.data_written;
    	       if (semaphore_trytotake(&sem_i2c0)==true){
			  	   imu.readMag();
			  	   samples_to_add=1;//add 1 sample
			  	   //imu.magAvailable(); to check if a data has been converted since last check
			  	   imu_mag_buffer.mem.data[curr_pos].x=imu.mx;
				   imu_mag_buffer.mem.data[curr_pos].y=imu.my;
				   imu_mag_buffer.mem.data[curr_pos].z=imu.mz;
				   imu_mag_buffer.mem.head.data_written=curr_pos+samples_to_add;
			  	   semaphore_release(&sem_i2c0);
		  	       }
		  	      else{samples_to_add=0;}
    	    //--------handle the data
	  	      if (imu_mag_buffer.mem.head.data_written> IMU0M_DATA_LIMIT)//if an overflow of data is going to happen
	  	      {//copy data to SD buffer and perform opperation
	  	    	 imu_mag_buffer.mem.head.acq_time_end=xTaskGetTickCount();    //time the task ended
	  	  	     if (semaphore_trytotake(&sem_spi0_microsd)==true)//trys to take the sempaphore to write into sd carda
	  	  	     {//copy all the bytes from the main structure to the data b8uffer
	  	  	        foocpy(microsd_write_buffer,imu_mag_buffer.byte,sizeof(imu_mag_buffer.byte));
	  	  	        //call the sd function to create the thread
	  	  	         thread_create(task_data_p[0]);
	  	  	       //create a microsd component to store all
	  	  	       //after finishing, the micro sd will release the resource
	  	         }//reset the store struct after ending the write process
		  	    reset_store_struct(&imu_mag_buffer);
	  	  	    imu_mag_buffer.mem.head.acq_time_start=xTaskGetTickCount();    //time the task has started
	  	       }
              //------------------delay
	        vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
	  	   }
}
