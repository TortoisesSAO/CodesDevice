#ifndef STRUCT_RF_TO_COLLECTOR_H
#define STRUCT_RF_TO_COLLECTOR_H
//----------------------------------
#include <time_AL/time_AL.h>//time and date managment in standard time.c
//----------------------------------------------------------------------------
//-----------------RF Message to send
//----------------------------------------------------------------------------

//#define RF_TX_DATA_CCA_MODE

void update_tx_packet_gps(float lat, float lon, float hdop,tm Time);
void update_tx_packet_temperature(int8_t temperature_degrees,int8_t temperature_degrees_mcu);
void update_tx_packet_battery_level(uint8_t battery_percent);
void update_tx_packet_animal_activity(uint8_t animal_activity,uint32_t elapsed_time );

#endif/**/
