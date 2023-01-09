#ifndef MICRO_SD_FAT_H_
#define MICRO_SD_FAT_H_


//library indicating the driver number
#include "ti_drivers_config.h"
//libraries for fat app in C
#include <file.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//libraries for fat drivers related to platform
#include <third_party/fatfs/ffcio.h>
#include <ti/drivers/SDFatFS.h>



enum microsd_states{
 SD_MOUNT_OK, //mounted of file successfull
 SD_MOUNT_ERROR,//error when trying to mount file
 SD_FILE_OPEN_OK,//file opening succesfull
 SD_FILE_OPEN_ERROR,//
 SD_FILE_CLOSE_OK,//file opening succesfull
 SD_FILE_CLOSE_ERROR,//
 };

#define TEST_UART_PRINT
//For Testing Pourpose
#define RTOS_MEASURE_TIME
//for recording writing time into file
#define SD_WRITE_TIME_TO_FILE

#ifdef SD_WRITE_TIME_TO_FILE
typedef struct {
	time_t	tv_sec;		/* seconds */
	long	tv_nsec;	/* and nanoseconds */
}timespec;
#define      SD_START_TIME 1670198400 // 1670198400 Mon November 05 2022 00:00:00 GMT-0300 (Argentina Standard Time)
#endif


typedef struct{
#ifdef SD_WRITE_TIME_TO_FILE
struct timespec ts;/* Set this to the current UNIX time in seconds */
#endif
SDFatFS_Handle handle;//original structure must be defined inside in order to make this wotrk
uint8_t driver_num;
FILE *file_src; //
uint8_t mount_num;//mount number
}
microsd_driver;
typedef struct{
//do not change the order of this declarations!
char fatfs_prefix[6];//leave size as 6 to work ok
char mount_num_c[3];//drive mount number in character
char file_subdir1[8];//8 is the max folder name size supporterd by a FAT library
char file_name[24]; //8 is the max name size supporterd by a FAT library
char file_extention[4];//4 is the max name extention size supporterd by a FAT library
char file_name_all[40];//this is the complete file name
microsd_driver driver;  //driver values, like handle, ect
char open_mode[2];//details the mode to perform the open
uint8_t state_code;//details of the state based on
int state_error_code;//if an error ocurs, it will be recorded here
uint8_t state;//Binary state of Last operation. 1 is succes, 0 is erorr
//unsigned char data[SD_BLOCK_UNIT_SIZE];
}microsd_struct;
//functions
//this function will init the micro sd comunication driver
//in this case, because we are encapsulating a driver
//that already has been encapsulated, we will not use it

#ifdef __cplusplus
  extern "C" {
#endif


void microsd_spi_init(microsd_struct* microsd_p);

//this function will init the micro sd comunication driver
void microsd_init(microsd_struct* microsd_p);
//set inside microsd_struct mount parameters
void microsd_set_mount(microsd_struct* microsd_p,uint8_t driver_num,uint8_t mount_num, char* fat_prefix);
// mount micro sd drive
int microsd_mount_sd(microsd_struct* microsd_p);
// unmount last micro sd drive
void microsd_unmount_sd(microsd_struct* microsd_p);
// Set the current file properties inside struct (open mode, file name and file extention)
void microsd_set_file_properties(microsd_struct* microsd_p,char* open_mode,char* name, char* extention);
// Open file with the parameters pased during last "microsd_set_file_properties"
int microsd_open_file(microsd_struct* microsd_p);
//Closes the last opened file
int microsd_close_file(microsd_struct* microsd_p);
//for internal use only. Updates char structs parameters
void microsd_update_file_all(microsd_struct* microsd_p);
/* This functions "writes" an desire amount of bytes () */
void microsd_write_block(microsd_struct* microsd_p,unsigned char* data_p, uint16_t data_size );
/* Mount Open-Write-Close-Unmount function */
int microsd_open_write_close(microsd_struct* microsd_p,unsigned char* data_p, uint16_t data_size );
//this function will attempt to open and create a file to know
//if the parameters of the SD were settep up properly.
// return 1 to success or 0 otherwise
int check_micro_sd_connected(microsd_struct* microsd_p);
//------------------------------------------------------------------------------------------
//-------------example of init code-----------------------------------------------------------------------------
//-------------(example of init code)-----------------------------------------------------------------------------
/* must be declared in your main
  MCU SPI DRIVER ASIGNED, FIXED
#define SD_SPI_DRIVER_NUMBER       CONFIG_SDFatFS_0

// Drive number used for FatFs
#define SD_MOUNT_DRIVE_NUM      0
#define SD_FATFS_PREFIX         "fat32"
#define SD_FILE_NAME            "data1"//
#define SD_FILE_EXTENTION       ".dat"
#define SD_OPEN_MODE            "a "//operating mode=appends
void microsd_start(microsd_struct* microsd_p)
{
	//set variables relevant to the use of the system
	   //this will be setted with the default values.
	   // if you want to change them, just call the functions in your main.c
	microsd_set_mount(microsd_p,SD_SPI_DRIVER_NUMBER,SD_MOUNT_DRIVE_NUM,SD_FATFS_PREFIX);
    microsd_set_file_properties(microsd_p,SD_OPEN_MODE,SD_FILE_NAME,SD_FILE_EXTENTION);
	microsd_init(microsd_p);
}
*/

#ifdef __cplusplus
   }
 #endif
#endif
