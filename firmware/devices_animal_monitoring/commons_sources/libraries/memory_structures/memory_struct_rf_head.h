#ifndef MEMORY_STRUCT_RF_HEAD_H
#define MEMORY_STRUCT_RF_HEAD_H

#include <memory_structures/memory_struct_DEFINES.h>
#include <unistd.h>//for uint def
//----------------------------------------------------------------------------
//-----------------RF GENERAL DEFINES
//----------------------------------------------------------------------------
typedef enum {//Types of messages sended by RF command
    RF_MSSG__,                   //default value: NONUSED
    RF_MSSG_DATA,                //the sended rf packet contains data
    RF_MSSG_CMD,                //command code*NON PROGRAMMED*
    RF_MSSG_ACK,                //ack code *NON PROGRAMMED*
  }tx_rf_packets_type;
//data of the transmited message
typedef struct {
    uint8_t  			 type_of_message;        // 1 bytes
    uint8_t              type_of_device;
	uint64_t             device_id;
}data_id_tx_rf;//6 bytes struct
//data added at the momento
typedef struct {
    uint32_t                 date_time_compressed; //rx (reception) date-time compressed in 32 bits.
    int8_t                   rssi;                 //rssi of rx packet
}data_id_rx_rf;
//structure with id used for parsing
typedef struct {
    data_id_tx_rf               id;//       Sum of all bytes:---------->  21 bytes
    uint8_t                     d; //data   300*12=3600 bytes
    uint16_t                    crc;                      //firmware checksum
}mem_data_id_tx_rf;//6 bytes struct

//example of animal struture
typedef struct{
    uint8_t                     d[RFDATA_BLOCK_UNIT_DATA_SIZE];//d[RFDATATORTO_BLOCK_UNIT_SIZE-sizeof(mem_data_header)-sizeof(data_id_rx_rf)-sizeof(data_id_tx_rf)];      //data from the transmiter
}data_rfdata_animal;// Has to be LEsser than SD_BLOCK_UNIT_SIZE

typedef struct{
    mem_data_header             head;   //data from the transmiter
    data_id_rx_rf               id_rx;  //reception data from the transmiter
    data_id_tx_rf               id_tx;  // id from the transmiter
    data_rfdata_animal          d;        //data from the transmiter
}mem_rf_rx_data_animal;// Has to be LEsser than SD_BLOCK_UNIT_SIZE

//create an union of all to facilitate manipulation
typedef union {
    mem_rf_rx_data_animal      m;
    unsigned char              b[RFDATA_BLOCK_UNIT_SIZE];//array of x size: SHould test
}mem_rf_rx_data_animal_union;//union of all structures
//-------------------------------------------------------
//-------------------------------------------------------
//-------------------------------------------------------
//-------------------------------------------------------
//for TD (Tracking device)
typedef struct{
    mem_data_header             head;
    data_id_rx_rf               id_rx;
    uint32_t                    freq;//                 Sum of all bytes:---------->  21 bytes
    uint8_t                     sec_to_next_packet_tx; //NON USED YET: time for a new data _packet _rx
}mem_rf_rx_data_animal_TD;// Has to be LEsser than SD_BLOCK_UNIT_SIZE

typedef union {
    mem_rf_rx_data_animal_TD      m;
    unsigned char                 b[sizeof(mem_rf_rx_data_animal_TD)];//array of x size: SHould test
}mem_rf_rx_data_animal_TD_union;//union of all structures

#endif/**/
