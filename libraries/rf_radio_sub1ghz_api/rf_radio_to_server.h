/*
 * rf_radio_to_server.h
 *
 *  Created on: 4 jul. 2022
 *      Author: Nicro
 */

typedef struct{
uart_struct interface; // interface selected to perform the
uint8_t operation_mmode; //the mode in wich the device works
uint8_t header[10];//data header containing information of the message
uint8_t* data_input_pointer;
uint8_t data_input_max;
uint8_t* data_output_pointer;
uint8_t data_output_max;
class radio_to_server
{
public:
	radio_to_server_struct server;

	radio_to_server::radio_to_server()

	//Either the case, this
	void init_server();//this function initialize the drivers to communicate with the server
	void server_transfer();//Transfer data from one device to a server
	void server_recieve();//Transfer data from one device to another
	//After doing the transmit
protected:
	float gRes, aRes, mRes;
	// init() -- Sets up gyro, accel, and mag settings to default.
	// to set com interface and/or addresses see begin() and beginSPI().
	//
	void init();
};

