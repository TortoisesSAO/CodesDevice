#ifndef MEMORY_STRUCT_SYSTEM_H
#define MEMORY_STRUCT_SYSTEM_H
#include <memory_structures/memory_struct_DEFINES.h>
//system info memory struct
//
#include <time_AL/time_AL.h>
//system information uses a different kind of header
typedef struct{
	 char 		        id_head[MEMMORY_STRUCT_START];//fixed for all the structures
	 char 				id_name[MEMMORY_STRUCT_ID_SIZE];//
}mem_data_header_esp;  //26 bytes struct
typedef struct{
    char                presentation_message[180]; //message to the user containing the information below
    char                separator;
    char                device_type[20];             //ex: MD_TORTOISE
    char                firmwareVersion[5];          //ex: 10.5
    char                hardwareVersion[5];          //ex: 1.52
    uint64_t            device_id;                   //ex: 11
    uint8_t             device_number;               //table assigned
    uint32_t            rtos_clock_tick_period_hz;   //rtos tick period expressed in Hz
    tm                  localtime;                    //expressed in tm format
    uint32_t            system_day_time_rtos;        //amount of seconds since system_day_time was asquired
    uint16_t            batery_level_in_mv;          //bat level in mv to avoid the need of a table to get values
}mem_system_data_conf;// Has to be LEsser than SD_BLOCK_UNIT_SIZE
typedef struct{
	mem_data_header_esp  head;
	mem_system_data_conf data;
}mem_system_data;// Has to be LEsser than SD_BLOCK_UNIT_SIZE

typedef union {
	mem_system_data 	m;
	unsigned char 		b[SYSTEM_BLOCK_UNIT_SIZE];	//array of x size: SHould test
}mem_system_data_union;

#endif /*  */
