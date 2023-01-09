/*
 * rf_radio_to_server.cpp
 *
 *  Created on: 4 jul. 2022
 *      Author: Nicro
 */


#include "uart_driver.h"  //

//radio_to_server::radio_to_server(){}

void radio_to_server::server_transfer(void)//Transfer data from one device to a server
{UART_send(uart_struct* server,&(server.data_input),server.data_input_max);
}
