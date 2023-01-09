#ifndef  _TinyGPS_DEFINES_h
#define  _TinyGPS_DEFINES_h
#define CC1312R //used platfor
#define _GPS_STORE_COMPLETE_MESSAGE

#ifdef  _GPS_STORE_COMPLETE_MESSAGE
 #define _GPS_MESSAGE_FIELD_SIZE 85 //73 is the max encountered, taken 85 just in case
 #define _CHAR_WHITEPACE_NUMBER  0x20 //0x20=32 is the number of white space character
#endif
#endif
