#ifndef MAC_TO_H_
#define MAC_TO_H_



#include <stdio.h>
#include <stdint.h>
    //this LIBRARY HAS BEEN TESTED WITH COMPILERS IN little endian format

#define MAC_ADDRESS_BYTES_LENGTH  8
#define MAC_ADDRESS_STR_HEX_LENGTH    MAC_ADDRESS_BYTES_LENGTH*3-1
#define MAC_ADDRESS_SRT_HEX_SEP       ':'
#define MAC_ADDRESS_SRT_HEX_CASE      'A'//'A' is for caps letters, 'a' is for lower case


//use an union to map the ouput
union u_8bytes_uint64_t  {
	uint8_t  byte[8];
	uint64_t u64;
};


#ifdef __cplusplus
  extern "C" {
#endif
uint64_t MAC_ADDRESS_str_to_hex( char * mac_str_in);
uint8_t MAC_ADDRESS_hex_to_str(uint64_t mac_hex_in, char * mac_str_out );
void MAC_ADDRESS_get_last8_chars(char * mac_str_in, char * mac_str_out );
/* example of use
int main(void){
//char mac_addres_str[] = "10:05:02:0d:3f:cd:02:5f";
char mac_addres_str[MAC_ADDRESS_STR_LENGTH];
uint64_t mac_addres_hex={0xA412F4867C85472D};
uint64_t mac_addres_hex_converted;
MAC_ADDRESS_hex_to_str(mac_addres_hex,mac_addres_str);
printf("\n my mac addres is: ");
printf(mac_addres_str);
printf("\n");
mac_addres_hex_converted=MAC_ADDRESS_str_to_hex(mac_addres_str);
printf("\n my original hex max addres is: %.x ",mac_addres_hex);
printf("\n my converted hex max addres is: %.x ",mac_addres_hex_converted);
return 0;
}*/
#ifdef __cplusplus
   }
 #endif

#endif

