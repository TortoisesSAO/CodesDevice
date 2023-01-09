#ifndef MEMORY_STRUCT_IMU0_ACCGYRO_H
#define MEMORY_STRUCT_IMU0_ACCGYRO_H

#include <memory_structures/memory_struct_DEFINES.h>
#include <lsm9ds1/LSM9DS1_Types.h>    //for IMU

//----------------------------------------------------------------------------
//-----------------IMU0   ACC/GYRO
//----------------------------------------------------------------------------
//main header to sotore critical information for data reconstruction
typedef struct {
    uint64_t acq_start_date;             // NOT REMOVE FOR NOW
    uint8_t  sample_rate;                // 1 byte
    uint8_t  gyro_on;                    // 1 bytes
    uint8_t  scale_accel;                // 1 bytes
    uint8_t  scale_gyro;                 // 1 bytes
}data_header_imu0_ag;//6 bytes struct
typedef struct{
  mem_data_header       head; //contains all data needed to identify the package
  data_header_imu0_ag   conf;// Sum of all bytes:---------->  21 bytes
  struct IMU_gyro_accel data[IMU0AG_DATA_ARRAY_SIZE];   //300*12=3600 bytes
}mem_imu0_a_g;// Has to be LEsser than SD_BLOCK_UNIT_SIZE
//create an union of all to facilitate manipulation
typedef union {
	mem_imu0_a_g mem;
	unsigned char byte[SD_BLOCK_UNIT_SIZE];//array of x size: SHould test
}mem_imu0_a_g_union;


#endif/**/
