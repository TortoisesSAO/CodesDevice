#ifndef MEMORY_STRUCT_DEFINES_H
#define MEMORY_STRUCT_DEFINES_H
#include <unistd.h>//for uint def
#include <device_system_information.h>
#include <easylink_custom/EasyLink.h>
//#define _GPS_MESSAGE_FIELD_SIZE 85
//------------defines for all cases
#define MEMMORY_STRUCT_SIZE	            MAX_SENSOR_BUFFER_AMOUNT//4096 for 2023/06/08 tortoise proyect //amounts of bytes to write
#define MEMMORY_STRUCT_START    	    2 //amounts of size to string chararec for ID
#define MEMMORY_STRUCT_ID_SIZE    	    12 //amounts of size to string chararec for ID
#define MEMMORY_STRUCT_HEAD_SIZE        MEMMORY_STRUCT_START + MEMMORY_STRUCT_ID_SIZE
#define MEM_STR_ID_HEAD                 {0x0D,0x0A}// "\r\n" in hex, to indicate beggining of packet
//Macro to control MEMMORY_STRUCT_HEAD_SIZE and MEMMORY_STRUCT_HEAD_SIZE +2 occupy the same
//#iftrue (MEMMORY_STRUCT_ID_SIZE+MEMMORY_STRUCT_START)!=MEMMORY_STRUCT_HEAD_SIZE
//      !!!ERROR, REDUCE YOUR MEMMORY STRUCT SIZE OR INCREASE YOUR HEAD SIZE
//#endif
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//---------------------Temperature from IMU 0 ------------------------------------
#define SYSTEM_STR_ID               "SYSTEM INFO" //NAME OF THE ID
#define SYSTEM_BLOCK_UNIT_SIZE         256 //1365 is max for 4096,we use 40 bytes for the header just in case
//-------------------------------------------------------------------------------------
//---------------------Temperature from IMU 0 ------------------------------------
#define TEMP_STR_ID                 "TEMPERATURE" //NAME OF THE ID
#define TEMP_BLOCK_UNIT_SIZE         356 //1365 is max for 4096,we use 40 bytes for the header just in case
#define TEMP_DATA_OFFSET_LIMIT       1
//---------------------IMU0-ACCELEREMOTER/GYROSCOPE ------------------------------------
#define IMU0AG_STR_ID                "IMU0AG" //NAME OF THE ID
#define IMU0AG_BLOCK_UNIT_SIZE        4096
#define IMU0AG_DATA_OFFSET_LIMIT      32  //337 is max . we use 305=337 -32 (max ammounts of samples to RX)
//---------------------IMU0-MAGNETOMETER------------------------------------
#define IMU0M_STR_ID                "IMU0M" //NAME OF THE ID
#define IMU0M_BLOCK_UNIT_SIZE        2048 //1365 is max for 4096,we use 40 bytes for the header just in case
#define IMU0M_DATA_OFFSET_LIMIT       1
//---------------------GPS0 NEO7M UBLOX------------------------------------
#define GPSNEOXM_STR_ID                "GPSNEO7M" //NAME OF THE ID
#define GPSNEOXM_BLOCK_UNIT_SIZE        1024 //we use 40 bytes for the header just in case
#define GPSNEOXM_DATA_ARRAY_SIZE        6    //
//---------------------RF TORTO------------------------------------
#define RFDATA_STR_ID              "RF:" //NAME OF THE ID
#define RFDATA_BLOCK_UNIT_SIZE      EASYLINK_MAX_DATA_LENGTH
#define RFDATA_BLOCK_UNIT_DATA_SIZE EASYLINK_MAX_DATA_LENGTH-30 //header is arround 30 bytes. Cannot be calculated with macros

#define MEMMORY_STRUCT_ID_LIG        LIG01 //NAME OF THE ID
#define MEMMORY_STRUCT_ID_MIC        MIC01 //NAME OF THE ID
//----------------------------------------------------------------------
//---------general memmory struct header
//----------------------------------------------------------------------
typedef struct {
	char 				id_head[MEMMORY_STRUCT_START];   //fixed for all the structures
	char 				id_name[MEMMORY_STRUCT_ID_SIZE]; //Unique name to identify the origin of the data
	uint16_t			block_size;//                    //amount of data to write into sd file
	uint16_t 			data_written;                    //amount of data written
	uint32_t 			acq_time_start;                  // timestamp
	uint32_t 			acq_time_end;                    // 4 bytes
}mem_data_header;

typedef struct{
	mem_data_header head;

}mem_data;// Has to be LEsser than SD_BLOCK_UNIT_SIZE

typedef union {
	mem_data 		    m;
	unsigned char 		b[MEMMORY_STRUCT_SIZE];	//array of x size: SHould test
}mem_data_union;

#endif /*  */
