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

#include <unistd.h>
#include <stdio.h>//to allow float to conversion
//#include "cpy_tbl.h"//to allow the use of memcopy
#include <string.h>


#include <RTOS_AL/rtos_AL.h> //allow the use of an operative system
#include <RTOS_AL/semaphore_AL.h> //allow the use of an operative system
#include <lsm9ds1/LSM9DS1.h> //imu main library
#include <memory_structures/memory_struct_imu0_accgyro.h>
#include <ti/display/Display.h>
#include "ti_drivers_config.h"
#include <ti/drivers/Temperature.h> //to allow temperature measurements
 Display_Handle display;
/* Thread function prottype to be used by scheduller */

extern TaskData* task_data_p[];
 sempaphore_struct sem_i2c0;  //semaphore for sharing the i2c bus acces
//extern unsigned char microsd_write_buffer[SD_BLOCK_UNIT_SIZE];
 sempaphore_struct sem_spi0_microsd;//semaphore for sharing the i2c bus acces

void foocpy (unsigned char* dst, unsigned char* src, uint16_t n)
{uint16_t i=0;
  for( i=0; i<n; i++){*(dst+i) = *(src+i);}
}

//-----------------------------------------------------------------------------------
//--------functions for sd carg storing, all must be declared static to avoid issues
//-----------------------------------------------------------------------------------
unsigned char microsd_write_buffer[SD_BLOCK_UNIT_SIZE];
static mem_imu0_a_g_union imu_gyro_accel_buffer{.mem.head.id_head=MEM_STR_ID_HEAD,
	.mem.head.id_name=IMU0AG_STR_ID,.mem.head.block_size=IMU0AG_BLOCK_UNIT_SIZE};
static void set_store_struct(mem_imu0_a_g_union* mem_struct,LSM9DS1* imu)
{
mem_struct->mem.conf.gyro_on=imu->settings.gyro.enabled;
if (mem_struct->mem.conf.gyro_on==1){mem_struct->mem.conf.sample_rate=imu->settings.gyro.sampleRate;}//gyro ON: Determines sample rate
else{                                mem_struct->mem.conf.sample_rate=imu->settings.accel.sampleRate;}//Gyro off: ACCel determines sample rate
mem_struct->mem.conf.scale_gyro=imu->settings.gyro.scale;
mem_struct->mem.conf.scale_accel=imu->settings.accel.scale;
mem_struct->mem.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
}
static void reset_store_struct(mem_imu0_a_g_union* mem_struct)
{
mem_struct->mem.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
}
/*
 *  ======== imu_Thread ========
 */
LSM9DS1 imu;//declares the variable
//-----------------------------------------------
extern "C" {
 void  task_i2c0_imu_accgyro(uint8_t task_index);
}
void task_i2c0_imu_accgyro(uint8_t task_index)
{   //settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t clocktick_start,clocktick_end;
	//configuring devices to use
	int8_t samples_to_add;
	uint16_t curr_pos,i;
   // Temperature_init();
	float clocktick_delta;
    display = Display_open(Display_Type_UART, NULL);//open display driver
    if (display == NULL) {while (1);}        /* Failed to open display driver */
    //-----------------------IMU START
    Display_printf(display, 0, 0, "Setting imu\n");
	if (semaphore_trytotake_time(&sem_i2c0,SEC_TO(4))==true){
    int setup_imu=imu.begin();//with no arguments, this uses default addresses (AG:0x6B, M:0x1E) and i2c port (Wire).
	if (setup_imu ==1) {Display_printf(display, 0, 0, "i2c fail to set up\n");
	    while(1);}
	if (setup_imu ==2) {Display_printf(display, 0, 0, "i2c setted2, fail to communicate with imu\n");
	    while(1);}
	//store all the character before enable the conversion
	set_store_struct(&imu_gyro_accel_buffer,&imu);
	//before staring the conversionn
	imu.enableFIFO_continous();//Starts adquisition in continous mode
	semaphore_release(&sem_i2c0);
	imu_gyro_accel_buffer.mem.head.acq_time_start=xTaskGetTickCount();    //time the task ended
    Display_printf(display, 0, 0, "Set imu success at  %i ticks\n",(xTaskGetTickCount()));
    Display_printf(display, 0, 0, "Set imu success at  %i\n",imu_gyro_accel_buffer.byte[20]);
    Display_printf(display, 0, 0, "Set imu success at  %i\n",imu_gyro_accel_buffer.byte[21]);
	}
        for (;;){
	  		  curr_pos=imu_gyro_accel_buffer.mem.head.data_written;
	  	     if (semaphore_trytotake(&sem_i2c0)==true){
	  	    	 imu.readAll(); //read all the data
	  	    	 samples_to_add=imu.FIFO_samples_available;//bytes to cpy
	  	    	 //coppy the data
	  	    	 for (i = 0; i < (samples_to_add); ++i){ imu_gyro_accel_buffer.mem.data[curr_pos+i]=imu.buffer.gyro_accel[i];}//copy the data
	  	    	 imu_gyro_accel_buffer.mem.head.data_written=curr_pos+samples_to_add;
	  	    	 semaphore_release(&sem_i2c0);
	  	       }
	  	      if (imu_gyro_accel_buffer.mem.head.data_written> IMU0AG_DATA_LIMIT)//if an overflow of data is going to happen
	  	      {//copy data to SD buffer and perform opperation
	        	 imu_gyro_accel_buffer.mem.head.acq_time_end=xTaskGetTickCount();    //time the task ended
	  	  	     if (semaphore_trytotake(&sem_spi0_microsd)==true)//takes the sempaphore
	  	  	     {uint16_t bytes_to_write;//copy all the bytes from the main structure to the data b8uffer
	  	  	        foocpy(microsd_write_buffer,imu_gyro_accel_buffer.byte,sizeof(imu_gyro_accel_buffer.byte));
		  	  	      bytes_to_write=microsd_write_buffer[MEMMORY_STRUCT_HEAD_SIZE]+microsd_write_buffer[MEMMORY_STRUCT_HEAD_SIZE+1]*256;
		  	  	      Display_printf(display, 0, 0, "Micro SD semaphore taken, bytes to write %i\n",bytes_to_write);
		  	  	      Display_printf(display, 0, 0, "time to finish task imu %3f\n",TO_SEC( imu_gyro_accel_buffer.mem.head.acq_time_end));
	  	  	           thread_create(task_data_p[0]);//resource aviable
	  	  	       //create a microsd component to store all
	  	  	       //after finishing, the micro sd will release the resource
	  	           //semaphore_release(&sem_spi0_microsd);/* Unlock resource */
	  	         }
		  	      reset_store_struct(&imu_gyro_accel_buffer);
	        	  imu_gyro_accel_buffer.mem.head.acq_time_start=xTaskGetTickCount();    //time the task started
	  	      }
	  	         Display_printf(display, 0, 0,(char *)"SD Buffer position is: %i ",imu_gyro_accel_buffer.mem.head.data_written);
	           //Display_printf(display, 0, 0,(char *)"Temperature MCU: %i,Temperature IMU: %2f",(int)Temperature_getTemperature(),imu.calcTemp(imu.temperature));
	             Display_printf(display, 0, 0,(char *)"Muestras adquiridas: %i",samples_to_add);
	         // Display_printf(display, 0, 0,(char *)"Gyro reading FIFO x: %2f y: %2f z: %2f \n",imu.calcGyro(imu.buffer.gyro_accel[0].gx),imu.calcGyro(imu.buffer.gyro_accel[0].gy),imu.calcGyro(imu.buffer.gyro_accel[0].gz));
	             Display_printf(display, 0, 0,(char *)"Accel reading FIFO x: %2f y: %2f z: %2f \n",imu.calcAccel(imu.buffer.gyro_accel[0].ax),imu.calcAccel(imu.buffer.gyro_accel[0].ay),imu.calcAccel(imu.buffer.gyro_accel[0].az));
             //-------------end of task, delay until next one--------------------
	         vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
	  	     }
}


