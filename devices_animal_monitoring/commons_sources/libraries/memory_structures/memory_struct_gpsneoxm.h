#ifndef MEMORY_STRUCT_GPSNEOXM_H
#define MEMORY_STRUCT_GPSNEOXM_H

//#include <micro_sd_fat/micro_sd_fat.h>//for microsd
#include <gps_parser/TinyGPS_DEFINES.h>     //for gps
#include <memory_structures/memory_struct_DEFINES.h>

//----------------------------------------------------------------------------
//-----------------GPS UBLOX
//----------------------------------------------------------------------------
typedef struct{  //this has to be filled with
 uint8_t                  power_mode;
 uint8_t                  sample_rate;
 uint8_t                  min_presition;   //min presion to fix
 uint32_t                 user_timeout;    //time out determined by user firmware
 uint32_t                 user_sample_rate;//sample rate  determined by user firmware
}gpsneo7_conf;

typedef struct{
  char     				line_break; 					 //char "\n"for break line
  char     				message[_GPS_MESSAGE_FIELD_SIZE];//GPS raw message content without breaklines. Last 2 chars are checksum
  char     				separator;						 // separator "|" to indicate the end of gps message recieved and starting of gps information
  char                  message_to_user[9];               //str message with time_to_get (5 chars) and numer. example:;01566;1;
  uint32_t 				time_to_fix; 					 //elapsed time in clock ticks since last recieve message ok/startup. If message_number=1 is since startup
  uint8_t  				message_type;					 //type of message stored: EX:
  uint8_t  				message_number;					 //number of recieved message of that type
}gpsneo7_sample;

typedef struct{
  mem_data_header		    head;							// Sum of all bytes: 21 bytes
  gpsneo7_conf              conf;
  gpsneo7_sample		    d[GPSNEOXM_DATA_ARRAY_SIZE];
}mem_gpsneo7;

typedef union {
	mem_gpsneo7 		m;
	unsigned char 		b[GPSNEOXM_BLOCK_UNIT_SIZE];	//array of x size: SHould test
}mem_gpsneo7_union;										//union of all structures


#endif /*  */
