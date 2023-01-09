#ifndef MEMORY_STRUCT_GPSNEOXM_H
#define MEMORY_STRUCT_GPSNEOXM_H

//#include <micro_sd_fat/micro_sd_fat.h>//for microsd
#include <gps_parser/TinyGPS_DEFINES.h>     //for gps
#include <memory_structures/memory_struct_DEFINES.h>

//----------------------------------------------------------------------------
//-----------------GPS UBLOX
//----------------------------------------------------------------------------
typedef struct{
  char     				line_break; 					 //char "\n"for break line
  char     				message[_GPS_MESSAGE_FIELD_SIZE];//GPS raw message content without breaklines. Last 2 chars are checksum
  char     				separator;						 // separator "|" to indicate the end of gps message recieved and starting of gps information
  uint32_t 				time_to_get; 					 //elapsed time since last recieve message ok/startup. If message_number=1 is since startup
  uint8_t  				message_type;					 //type of message stored: EX:
  uint8_t  				message_number;					 //number of recieved message of that type
}gpsneo7_sample;

typedef struct{
  mem_data_header		head;							// Sum of all bytes: 21 bytes
 gpsneo7_sample		    data[GPSNEOXM_DATA_ARRAY_SIZE];
}mem_gpsneo7;

typedef union {
	mem_gpsneo7 		mem;
	unsigned char 		byte[GPSNEOXM_BLOCK_UNIT_SIZE];	//array of x size: SHould test
}mem_gpsneo7_union;										//union of all structures


#endif /*  */
