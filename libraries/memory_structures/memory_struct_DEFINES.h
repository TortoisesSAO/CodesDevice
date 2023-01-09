#ifndef MEMORY_STRUCT_DEFINES_H
#define MEMORY_STRUCT_DEFINES_H

//#include <micro_sd_fat/micro_sd_fat.h>//for microsd
//variable to pass the amounts of bytes to write
//#define _GPS_MESSAGE_FIELD_SIZE 85
//------------defines for all cases
#define SD_BLOCK_UNIT_SIZE	            4096 //amounts of bytes to write
#define MEMMORY_STRUCT_START    	    2 //amounts of size to string chararec for ID
#define MEMMORY_STRUCT_ID_SIZE    	    12 //amounts of size to string chararec for ID
#define MEMMORY_STRUCT_HEAD_SIZE        MEMMORY_STRUCT_START + MEMMORY_STRUCT_ID_SIZE
#define MEM_STR_ID_HEAD                 {0x0D,0x0A}// "\r\n" in hex, to indicate beggining of packet
//Macro to control MEMMORY_STRUCT_HEAD_SIZE and MEMMORY_STRUCT_HEAD_SIZE +2 occupy the same
//#iftrue (MEMMORY_STRUCT_ID_SIZE+MEMMORY_STRUCT_START)!=MEMMORY_STRUCT_HEAD_SIZE
//      !!!ERROR, REDUCE YOUR MEMMORY STRUCT SIZE OR INCREASE YOUR HEAD SIZE
//#endif
//-------------------------------------------------------------------------------------
//--------------------definition of main structure to save-------------------------
extern unsigned char microsd_write_buffer[SD_BLOCK_UNIT_SIZE];
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//---------------------IMU0-ACCELEREMOTER/GYROSCOPE ------------------------------------
#define TEMP_STR_ID                "TEMPERATURE" //NAME OF THE ID
#define TEMP_BLOCK_UNIT_SIZE         512 //1365 is max for 4096,we use 40 bytes for the header just in case
#define TEMP_DATA_ARRAY_SIZE         70   //6 bytes for data
#define TEMP_DATA_LIMIT              30 //for testing
//---------------------IMU0-ACCELEREMOTER/GYROSCOPE ------------------------------------
#define IMU0AG_STR_ID                "IMU0AG" //NAME OF THE ID
#define IMU0AG_BLOCK_UNIT_SIZE        4096 //1365 is max for 4096,we use 40 bytes for the header just in case
#define IMU0AG_DATA_ARRAY_SIZE        337 //337 is max for 4046,we use 40 bytes for the header just in case
#define IMU0AG_DATA_LIMIT             320 //330 for testing
//---------------------IMU0-MAGNETOMETER------------------------------------
#define IMU0M_STR_ID                "IMU0M" //NAME OF THE ID
#define IMU0M_BLOCK_UNIT_SIZE        2048 //1365 is max for 4096,we use 40 bytes for the header just in case
#define IMU0M_DATA_ARRAY_SIZE        332 //332 is max for 2000,we use 40 bytes for the header just in case
#define IMU0M_DATA_LIMIT             320 //for testing
//---------------------GPS0------------------------------------
#define GPSNEOXM_STR_ID                "GPSNEO7M" //NAME OF THE ID
#define GPSNEOXM_BLOCK_UNIT_SIZE        1024 //1365 is max for 4096,we use 40 bytes for the header just in case
#define GPSNEOXM_DATA_ARRAY_SIZE        6    //
#define GPSNEOXM_DATA_LIMIT             6    //for testing

#define MEMMORY_STRUCT_ID_LIG    LIG01 //NAME OF THE ID
#define MEMMORY_STRUCT_ID_MIC    MIC01 //NAME OF THE ID
//----------------------------------------------------------------------
//---------general memmory struct header
//----------------------------------------------------------------------
typedef struct {
	char id_head[MEMMORY_STRUCT_START];//fixed for all the structures
	char id_name[MEMMORY_STRUCT_ID_SIZE];//
	uint16_t block_size;//               //amount of data to write into sd file
	uint16_t data_written;               //amount og data written
	//char  pad;             // 4 bytes
	uint32_t acq_time_start;             // 4 bytes
	//char  pad2;             // 4 bytes
	uint32_t acq_time_end;               // 4 bytes
}mem_data_header;//26 bytes struct


#endif /*  */
