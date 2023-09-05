#ifndef MEMORY_STRUCT_IMU0_ACCGYRO_H
#define MEMORY_STRUCT_IMU0_ACCGYRO_H

#include <memory_structures/memory_struct_DEFINES.h>
#include <lsm9ds1/LSM9DS1_Types.h>    //for IMU

//----------------------------------------------------------------------------
//-----------------IMU0   ACC/GYRO
//----------------------------------------------------------------------------
//main header to sotore critical information for data reconstruction
typedef struct {
    uint8_t  sample_rate;                // 1 byte
    uint8_t  gyro_on;                    // 1 bytes
    uint8_t  scale_accel;                // 1 bytes
    uint16_t scale_gyro;                 // 2 bytes
    char     message_to_user[15];          // MESSAGE content: ",SR=,GY:0,AC_S:,AC_S:"
}data_header_imu0_ag;//6 bytes struct

typedef union{
    struct IMU_gyro_accel gyro_accel;
    struct IMU_accel accel;
}uimu_accel_gyro;

typedef struct{
  mem_data_header        head; //contains all data needed to identify the package
  data_header_imu0_ag    conf;// Sum of all bytes:---------->  21 bytes
  uimu_accel_gyro        d[(IMU0AG_BLOCK_UNIT_SIZE-sizeof(mem_data_header)-sizeof(data_header_imu0_ag))/sizeof(uimu_accel_gyro)];   //data array 300*12=3600 bytes
}mem_imu0_a_g;// Has to be LEsser than SD_BLOCK_UNIT_SIZE
//create an union of all to facilitate manipulation
typedef union {
	mem_imu0_a_g m;
	unsigned char b[IMU0AG_BLOCK_UNIT_SIZE];//array of x size: SHould test
}mem_imu0_a_g_union;


#endif/**/
