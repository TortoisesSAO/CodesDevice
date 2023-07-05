
#include <unistd.h>//for uint def
#include "device_id_TI_CC13X2.h"
#
#include <string.h>//for strcmp

#ifdef TI_CC13x2
//Reading device id (MAC ADDRESS for TI CC131X)
//Reading memmory map
#include <ti/devices/cc13x2_cc26x2/inc/hw_memmap.h> //for device id (MAC ADDRESS) reading  // <ti/devices/cc13X2_cc26x2/inc/hw_memmap.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_fcfg1.h> //for device id (MAC ADDRESS) reading   // <ti/devices/cc13X2_cc26x2/inc/hw_fcfg1.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_types.h> //for device id (MAC ADDRESS) reading   //  <ti/devices/cc13X2_cc26x2/inc/hw_types.h>
#endif



//-----------------------------------c file--------------------------------------------------
static device_ID_data device_id_data;
static bool device_id_data_set;
static char device_id_str[MAC_BUFFER_SIZE];
static uint64_t device_id_num;

//internal function

uint8_t set_device_id_data(void){
    uint8_t str_size;
        device_id_num=get_device_id(); //get the device id and store into an string
        str_size=MAC_ADDRESS_hex_to_str(device_id_num,device_id_str);//convert the number to a mac address
        //we now check if the mac addres is listes in the device list
        device_id_data =device_ID_table_find_value(device_id_str);
        device_id_data_set=true;
   return(str_size);
}
//

uint64_t get_device_id(void){
	union_int64_t device_id_num;
	device_id_num.b[0] = HWREG( FCFG1_BASE +  FCFG1_O_MAC_15_4_0 ); //MAC_15_4_0 The first 32-bits of the 64-bit MAC 15.4 address
	device_id_num.b[1] = HWREG( FCFG1_BASE +  FCFG1_O_MAC_15_4_1 ); //MAC_15_4_1  The last 32-bits of the 64-bit MAC 15.4 address
    return(device_id_num.a);
}
uint8_t device_id_to_str(char* str,uint64_t device_id){
    return(MAC_ADDRESS_hex_to_str(device_id,str));//convert the number to a mac address
}

uint8_t get_device_id_str(char* str){
    if(device_id_data_set==false){
          set_device_id_data();
          device_id_data_set=true;}
    memcpy(str,&device_id_str,sizeof(device_id_str));
    return(MAC_BUFFER_SIZE);
}
// table construct. Must contain all the possible listed devices
extern const device_ID_table_Entry device_ID_table[]; //table declared in other file
//example of table declaration:
/*#define DEVICE_ID_NONLISTED "00:00:00:00:00:00:00:00"
  //ID (MAC ADDRESS FOR CC1312R)|ASIGNED NUMBER OF DEVICE
      {"00:12:4B:00:1C:AA:49:4E",{1,148100000}},
	  {"00:12:4B:00:1C:AA:42:E4",{2,148200000}},
	  {"00:12:4B:00:1C:AA:4B:CB",{3,148300000}},
	  {"00:12:4B:00:1C:AA:44:65",{4,148400000}},
	  {"00:12:4B:00:1C:AA:44:2F",{5,148500000}},
	  {"00:12:4B:00:1C:AA:46:53",{6,148600000}},
	  {"00:12:4B:00:1C:AA:44:7D",{7,148700000}},
	  {DEVICE_ID_NONLISTED      ,{0,148000000}}//for non listed device, uses this
};*/

/*search for a value inside the table, otherwise return value non listed*/
device_ID_data device_ID_table_find_value(char* device_id_str){
    uint8_t i;
    // char device_id_non_listed[24]=device_ID_NONLISTED;
        for (i=0; i<255; i++){
        	//compare the strin
            if ( strcmp (device_ID_table[i].device_id, device_id_str)==0) {break;}
            if (device_ID_table[i].device_data.device_number==0) {break;} //for potection

         }
    return(device_ID_table[i].device_data); //return last value of the table if nothing is found
}

/*return keep alive frequency asigned to device id*/
uint32_t device_ID_get_ka_frequency(void){
    if(device_id_data_set==false){
          set_device_id_data();
          device_id_data_set=true;}
    return(device_id_data.device_frequency);
}
uint32_t get_ka_frequency_from_num(uint8_t asigned_num){
    uint8_t i;
    // char device_id_non_listed[24]=device_ID_NONLISTED;
        for (i=0; i<255; i++){
            if (device_ID_table[i].device_data.device_number==asigned_num) {
                break;}
            if (device_ID_table[i].device_data.device_number==0) {
                break;}
            }
   return(device_ID_table[i].device_data.device_frequency);
}
uint8_t  get_asigned_num_from_id_str(char *id_str){
    device_ID_data data=device_ID_table_find_value(id_str);
    return(data.device_number);
}


