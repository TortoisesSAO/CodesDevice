/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 *  ======== fatsd.c ========
 */

//Reading memmory map
#include <ti/devices/cc13X2_cc26x2/inc/hw_memmap.h> //for device id reading
#include <ti/devices/cc13X2_cc26x2/inc/hw_fcfg1.h> //for device id reading
#include <ti/devices/cc13X2_cc26x2/inc/hw_types.h> //for device id reading


#include <RTOS_AL/rtos_AL.h>          //allow the use of an operative system
#include <RTOS_AL/semaphore_AL.h> //allow the use of an operative system
#include <micro_sd_fat/micro_sd_fat.h>//micro sd main libary
#include <memory_structures/memory_struct_DEFINES.h>
#include <task_spi0_microsd.h>      //defines
/* Driver configuration */
#ifndef TEST_UART_PRINT
#include <ti/display/Display.h>
//uart display
static Display_Handle display;
#endif
static microsd_struct microsd;

const char first_writting[] = "\r\nFirst written message containing device id, firmware id and other data";

//--------This elements shall be copy with each character
extern unsigned char microsd_write_buffer[SD_BLOCK_UNIT_SIZE];
extern sempaphore_struct sem_spi0_microsd;
//--------------------------------

#ifdef RTOS_MEASURE_TIME
#include <ti/sysbios/knl/Clock.h> //TI RTOSto acces task sleep fro
u_int32_t clocktick_start,clocktick_end;
float clocktick_delta;
#endif


//semaphore utilities
// must be declared to start the microSD
void microsd_start(microsd_struct* microsd_p)
{//set variables relevant to the use of the system
	   //this will be setted with the default values.
	   // if you want to change them, just call the functions in your main.c
	microsd_set_mount(microsd_p,SD_SPI_DRIVER_NUMBER,SD_MOUNT_DRIVE_NUM,SD_FATFS_PREFIX);
    microsd_set_file_properties(microsd_p,SD_OPEN_MODE,SD_FILE_NAME,SD_FILE_EXTENTION);
	microsd_init(microsd_p);
}
//

void gen_character(uint8_t* char_array_pointer, uint16_t array_size)
{//for either char or unsigned char, uint8_t* is the same
uint16_t i_min=1;
uint16_t i;
uint8_t j;
uint8_t j_min=0;
uint8_t j_max=255;
*(char_array_pointer+0)='\n';//first character='\n'(linebreak)

j=j_min;
for (i=i_min; i < (array_size-i_min); i++) {
	 *(char_array_pointer+i)=(unsigned char)(j);
	 j++;
	 if (j>j_max){j=j_min;}
}
*(char_array_pointer+array_size-i_min)='\n';//Last character='\n'(linebreak)
}
extern TaskData* task_data_p[2];

extern void task_spi0_microsd( uint8_t task_index);
extern void task_spi0_microsd_write( uint8_t task_index);

void task_spi0_microsd_write_block(uint8_t task_index)
{
//microsd_start(&microsd);
uint16_t bytes_to_write; //verfied
bytes_to_write=microsd_write_buffer[MEMMORY_STRUCT_HEAD_SIZE]+microsd_write_buffer[MEMMORY_STRUCT_HEAD_SIZE+1]*256;
microsd_open_write_close(&microsd,microsd_write_buffer,bytes_to_write);
semaphore_release(&sem_spi0_microsd);
pthread_detach(task_data_p[task_index]->handle);
}

void task_spi0_microsd( uint8_t task_index)
{/* Return variables */
   int result;
   /* Call driver init functions */
   // gen_character(microsd_write_buffer, SD_BLOCK_UNIT_SIZE);
   microsd_start(&microsd);
   check_micro_sd_connected(&microsd);//run this first to improve speed
   microsd_open_write_close(&microsd,first_writting,sizeof(first_writting));
   //microsd_open_write_close(&microsd,microsd_write_buffer,sizeof(microsd_write_buffer));
   //End of thread: Distroy itself to relealse the memory space:
   //pthread_create(&thread, &attrs, task_data_p[i_tk]->pxTaskCode, task_data_p[i_tk]->i_tk);
   //prepares the future task parsameter for creation:
   task_data_p[task_index]->pxTaskCode=*task_spi0_microsd_write_block;
   //destroy the current task
   pthread_detach(task_data_p[task_index]->handle);
}


