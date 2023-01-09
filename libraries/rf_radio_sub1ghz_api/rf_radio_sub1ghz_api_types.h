/*
 * rf_radio_sub1ghz_api_types.h
 *
 *  Created on: 4 jul. 2022
 *      Author: Nicro
 */

#ifndef RF_RADIO_SUB1GHZ_API_TYPES_H_
#define RF_RADIO_SUB1GHZ_API_TYPES_H_

//to search for defines
#include "rf_radio_sub1ghz_api_defines.h"
#ifdef TI_CC1312R
/* EasyLink API Header files */
//hardware radio driver
#include "easylink/EasyLink.h"

//Modes of the radio
enum radio_api_mode
{
	RF_TRANSMITER_NODE, //RF set up as a transmiter node.
	RF_TRACKER_DEVICE, // RF mode as a tracker devicer
	RF_COLLECTOR_STATION,//RF works as a collector station. Get all the information of the nodes and send it trough uart
};
//----------------------------------------------
//Radio struct
//----------------------------------------------
//uart structure for serial transfer
typedef struct{
	uint8_t handle; //handler
}
radio_rf_driver;

typedef struct{
radio_rf_driver driver;  //driver values, like handle, ect
uint8_t status; // status of the last operation, including setteting ups
uint32_t central_frequency;//current centered frequency of the transmition
uint32_t rx_bandwidth; //bandwidth selected
uint32_t modulation_frequency; //Level of deviation frequency of the modulation
uint32_t baudrate_raw; //raw baudrate selected
uint32_t baudrate_effective; //raw baudrate selected
int8_t tx_power; // current tx power
int8_t rx_rssi;//power recieved in last radio reception
}radio_rf_struct;



#endif

#endif /* RF_RADIO_SUB1GHZ_API_RF_RADIO_SUB1GHZ_API_TYPES_H_ */
