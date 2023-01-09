/*
 * MAC_to.c
 *
 *  Created on: 13 nov. 2022
 *      Author: Nicro
 */
#include "MAC_to.h"
//function compatible with 64 and 48
void MAC_ADDRESS_hex_to_str(uint64_t mac_hex_in, char * mac_str_out )
{
	uint8_t i_byte, i_byte_c;
    uint8_t i_char = 0U;
    char nibble_char;
    uint8_t byte_nibble;

   union u_8bytes_uint64_t mac_hex={.u64=mac_hex_in};
    //this example works for little endian

    for( i_byte = 0U; i_byte < MAC_ADDRESS_BYTES_LENGTH ; i_byte++ )
    {   //iterates each byte
        for( i_byte_c = 0; i_byte_c < 2U; i_byte_c++ )
        {//generate
            if( i_byte_c == 0U )
            { byte_nibble = mac_hex.byte[i_byte] >> 4; }
            else
            {byte_nibble = mac_hex.byte[i_byte] & 0x0FU; }
            if( byte_nibble < 0x09U )
            { nibble_char = '0' + byte_nibble;  }
            else
            {nibble_char = MAC_ADDRESS_SRT_HEX_CASE + ( byte_nibble - 10U );}/*MAC_ADDRESS_SRT_CASE must be Either 'a' or 'A' */
            mac_str_out[ i_char++ ] = nibble_char;
        }
        if( i_byte == ( MAC_ADDRESS_BYTES_LENGTH - 1U ) )
        {mac_str_out[ i_char++ ] = 0;}
        else
        {mac_str_out[ i_char++ ] = MAC_ADDRESS_SRT_HEX_SEP;}
    }
}
uint64_t MAC_ADDRESS_str_to_hex( char * mac_str_in)
{
    size_t i_char, i_byte_c;
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

