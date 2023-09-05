#ifndef DEVICE_ID_TI_CC13X2_H_
#define DEVICE_ID_TI_CC13X2_H_

#include <string.h>//for memcpy ussage
#include "MAC_to.h" //for mac convertion
#define TI_CC13x2
//-----------------header--------------------------------------------------
typedef union {
    uint32_t b[2];
}uuint64_t;

typedef union {
      uint32_t b[2];
      uint64_t a;
}union_int64_t;

//-------------------------------
#define MAC_STR_SIZE    MAC_ADDRESS_STR_HEX_LENGTH   //DO NOT CHANGE
#define MAC_BUFFER_SIZE MAC_ADDRESS_STR_HEX_LENGTH+1   //DO NOT CHANGE
typedef struct
{
    uint8_t  device_number;
    uint32_t device_frequency;
} __attribute__((packed)) device_ID_data;

typedef struct
{
    char device_id[MAC_BUFFER_SIZE];
    device_ID_data device_data;
} __attribute__((packed)) device_ID_table_Entry;


#ifdef __cplusplus
  //"C" {  //removed just in case
#endif
    /*return keep alive frequency asigned to device id*/
    uint32_t        device_ID_get_ka_frequency(void);
    device_ID_data  device_ID_table_find_value( char* device_id);
    uint64_t        get_device_id(void);
    uint8_t         get_device_id_str(char * str);
    uint32_t        get_ka_frequency_from_num(uint8_t asigned_num);
    uint8_t         device_id_to_str(char * str,uint64_t device_id);
    uint8_t         get_asigned_num_from_id_str(char *id_str);
#ifdef __cplusplus
//  }    //removes just in case
 #endif

#endif

