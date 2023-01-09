/*
 * rf_radio_api.cpp
 *
 *  Created on: 3 jul. 2022
 *      Author: Nicro
 */

#ifndef RF_RADIO_SUB1GHZ_API_CPP_
#define RF_RADIO_SUB1GHZ_API_CPP_

#include "rf_radio_sub1ghz_api.h"
rf_sub1ghz_radio::rf_sub1ghz_radio()
{}

void rf_sub1ghz_radio::init()
{}

radio_status rf_sub1ghz_radio::stop_rfradio()
{}
radio_status rf_sub1ghz_radio::transmit() //bloquing / EasyLink_transmit(
{}
radio_status rf_sub1ghz_radio::transmit_nobloquing()//EasyLink_transmitAsync()
{}
radio_status rf_sub1ghz_radio::transmit_nobloquing_cca()
{}
//EasyLink_transmitCcaAsync() /Non-blocking Transmit with Clear Channel Assessment|
// scans the channel to know if it is aviable
radio_status rf_sub1ghz_radio::scanchannel()
{}
//recieves a message in a bloquing way
radio_status rf_sub1ghz_radio::recieve()
{}
//recieves a message in a non bloquing way
radio_status rf_sub1ghz_radio::recieve_nobloquing()
{}
//gets the current central frecuency of the rf driver
void rf_sub1ghz_radio::get_centralfrequency(void){    //EasyLink_getFrequency setted
radio.central_frequency=EasyLink_getFrequency();

}

//Sets a central frequency, converting the desire frequency to a possible value
void rf_sub1ghz_radio::set_centralfrequency(uint32_t ui32Frequency) {//EasyLink_setFrequency. First values can be returned as error
//uint32_t setted_frequency
radio.status=EasyLink_setFrequency(ui32Frequency);
get_centralfrequency();//stores the central frequency

}
void rf_sub1ghz_radio::get_txpower(void){ //EasyLink_getRfPower()
radio.status=EasyLink_getRfPower(&(radio.tx_power));
}
void rf_sub1ghz_radio::set_txpower(int8_t pi8TxPowerDbm){//returns the power setted
radio.status=EasyLink_setRfPower(pi8TxPowerDbm);
EasyLink_getRfPower(&(radio.tx_power));//stores value in radio.tx_power
}
void rf_sub1ghz_radio::get_rfparameter(int8_t* pi8TxPowerDbm)){// EasyLink_setCtrl()
}
void rf_sub1ghz_radio::set_rfparameter(int8_t* pi8TxPowerDbm)){// EasyLink_getCtrl()
}
void rf_sub1ghz_radio::get_rxrssi(int8_t* rx_rssi){// EasyLink_getRssi()
radio.status=EasyLink_getRssi(&(radio.rx_rssi)); //return value and store it
}
#endif /* RF_RADIO_API_RF_RADIO_API_CPP_ */
