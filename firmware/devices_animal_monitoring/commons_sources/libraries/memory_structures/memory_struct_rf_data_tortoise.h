#ifndef MEMORY_STRUCT_RF_TORTOISE_H
#define MEMORY_STRUCT_RF_TORTOISE_H

#include <memory_structures/memory_struct_rf_head.h>
//----------------------------------------------------------------------------
//-----------------RF TORToise
//----------------------------------------------------------------------------
//main header to sotore critical information for data reconstruction
typedef struct {
	uint8_t              bat_level_percent;       //batery level expressed as a percent
	int8_t               temperature_c;    		  //temperature in degrees of mcu
	int8_t               temperature_c_imu;       //temperature in degrees of the most accurate sensor
	uint8_t              animal_activity;         //activity of the animal
    uint16_t             animal_activity_time;    //elapsed time of the activity of the animal
    uint16_t             day_time_2_sec_res;      //a mounts of 2seconds elapsed of the day. UTC
    uint32_t             gps_date_time_compressed;//current date/time compresed moment of day in which last gps message was recieved
	float                gps_lat;           	  //latitude obtanided through gps module
	float                gps_lon;		    	  //longitude obtanided through gps module
	uint16_t             gps_hdop_2dec;           //hdop (Horizontal Dilution of Precision) obtanided through gps module
}data_rfdata_tortoise;//6 bytes struct

typedef struct{
    data_id_tx_rf               id;//       Sum of all bytes:---------->  21 bytes
	data_rfdata_tortoise      	d; //data   300*12=3600 bytes
    uint16_t                    crc;                      //firmware checksum
}mem_rf_tx_data_tortoise;// Has to be LEsser than SD_BLOCK_UNIT_SIZE
typedef union {
    mem_rf_tx_data_tortoise      m;
    unsigned char                b[sizeof(mem_rf_tx_data_tortoise)];//array of x size: SHould test
}mem_rf_tx_data_tortoise_union;//union of all structures

typedef struct{
    mem_data_header             head;
    data_id_rx_rf               id_rx;
    data_id_tx_rf               id_tx;  // id from the transmiter
    data_rfdata_tortoise        d;      //data from the transmiter
}mem_rf_rx_data_tortoise;// Has to be LEsser than SD_BLOCK_UNIT_SIZE

//create an union of all to facilitate manipulation
typedef union {
    mem_rf_rx_data_tortoise      m;
    unsigned char                b[RFDATA_BLOCK_UNIT_SIZE];//array of x size: SHould test
}mem_rf_rx_data_tortoise_union;//union of all structures

#endif/**/
