#ifndef MEMORY_STRUCT_IMU0_MAG_H
#define MEMORY_STRUCT_IMU0_MAG_H

#include <memory_structures/memory_struct_DEFINES.h>
#include <lsm9ds1/LSM9DS1_Types.h>    //for IMU
//----------------------------------------------------------------------------
//-----------------IMU0   MAG
//----------------------------------------------------------------------------
//main header to sotore critical information for data reconstruction
typedef struct {
    uint8_t  			 sample_rate;                // 1 byte
    uint8_t  			 scale;                     // 1 bytes
    char                 message_to_user[9];        // ;scalels; samplerate
}data_header_imu_mag;//6 bytes struct
//
typedef struct{
	mem_data_header        head;                    // Sum of all bytes:---------->  21 bytes
    data_header_imu_mag    conf;                    // Sum of all bytes:---------->  21 bytes
    struct  IMU_3xasis     d[(IMU0M_BLOCK_UNIT_SIZE-sizeof(mem_data_header)-sizeof(data_header_imu_mag))/sizeof(IMU_3xasis)];   //300*12=3600 bytes
}mem_imu0_mag;                                      // Has to be less than SD_BLOCK_UNIT_SIZE
//create an union of all to facilitate manipulation
typedef union {
	mem_imu0_mag 		   m;
	unsigned char 		   b[IMU0M_BLOCK_UNIT_SIZE];//array of x size: SHould test
}mem_imu0_mag_union;//union of all structures


#endif/**/
