/*
 * MAC_to.c
 *
 *  Created on: 13 nov. 2022
 *      Author: Nicro
 */
#include "MAC_to.h"
#include <string.h>
//function compatible with 64 and 48 bytes array
char  MAC_ADDRESS_NIB_to_str(uint8_t i_byte,uint8_t i_byte_c)
{ char nibble_char;
uint8_t byte_nibble;
    if( i_byte_c == 0U ){ byte_nibble = i_byte >> 4; }
    else 				 {byte_nibble = i_byte & 0x0FU; }
    //get the char associated to nibble
    if( byte_nibble < 0x9U ){ nibble_char = '0' + byte_nibble;  }/*MAC_ADDRESS_SRT_CASE must be Either 'a' or 'A' */
    else {                    nibble_char = MAC_ADDRESS_SRT_HEX_CASE + ( byte_nibble - 10U );}
    if (byte_nibble==0x9U){   nibble_char = '0' + byte_nibble;}//fix non related char
    //store the char
return  nibble_char;
}

//example  MAC_ADDRESS_hex_to_str(1746841685148645569,c); should return " MAC_ADDRESS_hex_to_str(1746841685148645569,c);"
uint8_t MAC_ADDRESS_hex_to_str(uint64_t mac_hex_in, char * mac_str_out )
{
	uint8_t i_byte, i_byte_c;
    uint8_t i_char = 0U;
   // uint64_t mac_hex_in_set=;
    mac_str_out[0]='0';//init
    union u_8bytes_uint64_t mac_hex={.u64=mac_hex_in};
    //this example works for little endian
    for( i_byte = MAC_ADDRESS_BYTES_LENGTH-1; i_byte>0 ; i_byte=i_byte-1)
    {   //iterates each byte
        for( i_byte_c = 0; i_byte_c < 2U; i_byte_c++ )
        {   //check the byte for the nibble
        	//strcat(mac_str_out,MAC_ADDRESS_NIB_to_str(mac_hex.byte[i_byte],i_byte_c));//original
		    //i_char++;//original
        	mac_str_out[ i_char++ ]=MAC_ADDRESS_NIB_to_str(mac_hex.byte[i_byte],i_byte_c); //working
        }
      if( i_byte>(0U) ) {mac_str_out[ i_char++ ] = MAC_ADDRESS_SRT_HEX_SEP;}
    }
  //  strcat(mac_str_out,MAC_ADDRESS_NIB_to_str(mac_hex.byte[0],0)); //original
   // strcat(mac_str_out,MAC_ADDRESS_NIB_to_str(mac_hex.byte[0],1)); //original
    mac_str_out[ i_char++ ] = MAC_ADDRESS_NIB_to_str(mac_hex.byte[0],0); //working
    mac_str_out[ i_char++ ] = MAC_ADDRESS_NIB_to_str(mac_hex.byte[0],1); //working
    mac_str_out[MAC_ADDRESS_STR_HEX_LENGTH]='\0';//marcs the end.
return(MAC_ADDRESS_STR_HEX_LENGTH);
}


uint64_t MAC_ADDRESS_str_to_hex( char * mac_str_in) //non tested
{
    size_t i_char;
    size_t i_byte = 0;
    size_t i_nibble=0;
    uint8_t byte_nibble[2];
    uint8_t b_char_s=MAC_ADDRESS_SRT_HEX_CASE;
    union u_8bytes_uint64_t mac_hex;
    //this example works for little endian

    for( i_char = 0; i_char < MAC_ADDRESS_STR_HEX_LENGTH ; i_char++ )
    {   //iterates each char to find a number
       if (mac_str_in[i_char]!=MAC_ADDRESS_SRT_HEX_SEP)
       {//character is not a spacer(not':' or "-" or '.')
         if (('9'>=mac_str_in[i_char]))
            {byte_nibble[i_nibble]=(mac_str_in[i_char])-48;}//-48  to perform conversion from char to uint (range 0 to 9)
         if ((mac_str_in[i_char]>=(MAC_ADDRESS_SRT_HEX_CASE)))
            {byte_nibble[i_nibble]=mac_str_in[i_char]-b_char_s+10;} //+10 to perform conversion from char to uint (range a/A to f/F)
        //if we have iterated the second nibble, perform conversion to byte
         if (i_nibble==1)
           {mac_hex.byte[i_byte]=byte_nibble[1]+byte_nibble[0]*16;
            ++i_byte;} //increase the bytes converted counter
         ++i_nibble;
        }
       else {i_nibble=0;}
    }
    return(mac_hex.u64);
}

void MAC_ADDRESS_get_last8_chars(char * mac_str_in, char * mac_str_out )
{  //example if char mac_str_in[24]="01:23:45:67:89:AB:CD:EF"-> char mac_str_out[9]="89ABCDEF"
    mac_str_out[0]=mac_str_in[12];
    mac_str_out[1]=mac_str_in[13];
    mac_str_out[2]=mac_str_in[15];
    mac_str_out[3]=mac_str_in[16];
    mac_str_out[4]=mac_str_in[18];
    mac_str_out[5]=mac_str_in[19];
    mac_str_out[6]=mac_str_in[21];
	mac_str_out[7]=mac_str_in[22];
}


