#ifndef TASK_SPI0_MICROSD_H_
#define TASK_SPI0_MICROSD_H_
#include "device_system_information.h"
#ifdef SD_STORAGE_ON
#include <string.h>//for memcpy ussage
#include <RTOS_AL/semaphore_AL.h> //allow the use of an operative system
//#include <micro_sd_fat/micro_sd_fat.h>//micro sd main libary
#define SD_BLOCK_UNIT_SIZE         SD_MAX_STORE_AMOUNT
/* MCU SPI DRIVER ASIGNED, FIXED */
#define SD_SPI_DRIVER_NUMBER       spi_sd//CONFIG_SDFatFS_0 is the default name
/* Drive number used for FatFs */
#define SD_MOUNT_DRIVE_NUM      0       //DO NOT CHANGE
#define SD_FATFS_PREFIX         "fat32"//DO NOT CHANGE
#define SD_FILE_EXTENTION       ".dat"
#define SD_OPEN_MODE            "a "//operating mode=appends
#ifdef __cplusplus
  extern "C" {
#endif

 void memory_store_give_shared_spi_semaphore(sempaphore_struct* psem_spi);
#ifdef __cplusplus
   }
 #endif
//------------------------

    void microsd_set_battery_level(uint16_t bat_level_mv, uint8_t bat_level_percent);
    //void microsd_set_date_and_time(char* date,uint32_t time, uint32_t rtos_clock);
    /*writes a specific file into the folder of the device*/
    void microsd_write_file_into_device_folder(unsigned char* data_p, uint16_t data_size, char* name_all,uint8_t name_size);
    void memory_create_first_file(void);
    int memory_check_sd_connected_ok(void);
	int memory_store(void* pMem_to_store);
	void memory_store_str(void* pMem_to_store,uint16_t str_size);

#endif
#endif
