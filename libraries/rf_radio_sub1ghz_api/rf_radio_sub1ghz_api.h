/*
 * rf_radio_api.h
 *
 *  Created on: 3 jul. 2022
 *      Author: Nicro
 */

#ifndef RF_RADIO_SUB1GHZ_API_H_
#define RF_RADIO_SUB1GHZ_API_H_


#include "rf_radio_sub1ghz_api_types.h"  //

//define the radio estatus
#define radio_status EasyLink_Status
//create the radio class
class rf_sub1ghz_radio
{
public:
	// We'll store the gyro, accel, and magnetometer readings in a series of
	// public class variables. Each sensor gets three variables -- one for each
	// axis. Call readGyro(), readAccel(), and readMag() first, before using

	// begin() and beginSPI() -- Initialize the gyro, accelerometer, and magnetometer.
	//starts up the Radio Driver
	//intis the radio
	//Common radio Functions
	radio_rf_struct radio;
	rf_sub1ghz_radio();//constructor declatation
	radio_status init_rfradio();//to init the radio
	//stops all the elements of the ratiuo
	radio_status stop_rfradio();//Turn oFF the radio
	radio_status transmit(); //bloquing / EasyLink_transmit()
	radio_status transmit_nobloquing();//EasyLink_transmitAsync()
	radio_status transmit_nobloquing_cca();//EasyLink_transmitCcaAsync() /Non-blocking Transmit with Clear Channel Assessment|
	// scans the channel to know if it is aviable
	radio_status scanchannel();
	//recieves a message in a bloquing way
	radio_status recieve();
	//recieves a message in a non bloquing way
	radio_status recieve_nobloquing();
	//Sets a central frequency, converting the desire frequency to an admisible value
	// by the hardware syntesiser, then stores that value in central frequency.
	void set_centralfrequency(uint32_t ui32Frequency); //EasyLink_setFrequency
	//gets the current central frecuency of the rf driver
	void get_centralfrequency(void); //EasyLink_getFrequency
	void set_txpower(int8_t pi8TxPowerDbm);
	void get_txpower();
	//Get the rssi
	void get_rxrssi(int8_t* rx_rssi);// EasyLink_getRssi()
	//set and specific RF parameter
	void get_rfparameter();// EasyLink_setCtrl()
	void set_rfparameter();// EasyLink_getCtrl()
	//EasyLink_enableRxAddrFilter() // Enables/Disables RX filtering on the Addr |
	//EasyLink_abort() Aborts a non blocking call
	//| EasyLink_getAbsTime() | Gets the absolute time in RAT ticks |
	//--------------------------------
	//Functions specific of the collector and transmiter
	//------------------------------------------
	//This functions allow the device to comunicate with a device that
	// will work as a SERVER
	//In this case, we called server to the device & app that handles all the information
	//This device can be a both backend  (only handeling & processing the data)
	//	                     or frontend (shows the data to the user)
	//based on the application side.

protected:
	float gRes, aRes, mRes;
	// init() -- Sets up gyro, accel, and mag settings to default.
	// to set com interface and/or addresses see begin() and beginSPI().
	//
	void init();
	//void I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data);
	//uint8_t I2CreadByte(uint8_t address, uint8_t subAddress);
	//uint8_t I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count);
};

#endif


