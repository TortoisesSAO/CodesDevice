#ifndef MEMORY_STRUCT_TEMP_H
#define MEMORY_STRUCT_TEMP_H

#include <memory_structures/memory_struct_DEFINES.h>
//----------------------------------------------------------------------------
//-----------------IMU0   MAG
//----------------------------------------------------------------------------
//main header to sotore critical information for data reconstruction
typedef struct {
    uint16_t  			 sample_rate_seconds;        // 2 bytes
}data_conf_temp;//6 bytes struct
typedef struct {
	float 			     temp_imu;             // 8 bytes
	int16_t 			 temp_mcu;             // 8 bytes
}data_array_temp;//6 bytes struct
typedef struct{
	mem_data_header      head;// Sum of all bytes:---------->  21 bytes
	data_conf_temp       conf;// Sum of all bytes:---------->  21 bytes
	data_array_temp      d[(TEMP_BLOCK_UNIT_SIZE-sizeof(mem_data_header)-sizeof(data_conf_temp))/sizeof(data_array_temp)];   //300*12=3600 bytes
}mem_temp;// Has to be LEsser than SD_BLOCK_UNIT_SIZE
//create an union of all to facilitate manipulation
typedef union {
	mem_temp 		     m;
	unsigned char 		 b[TEMP_BLOCK_UNIT_SIZE];//array of x size: SHould test
}mem_temp_union;//union of all structures


#endif/**/
