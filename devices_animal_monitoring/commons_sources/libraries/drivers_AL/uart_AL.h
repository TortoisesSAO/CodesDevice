/*
 * uart_driver.h
 *
 *  Created on: 20 jul. 2022
 *  Author: Andres Oliva Trevisan
 */

#ifndef UART_AL_H_
#define UART_AL_H_

#include <stddef.h>//for declaration of variables
/*--------------------------------------------------------------
 ---------------------Structures--------------------------------
 --------------------------------------------------------------*/
typedef enum
{  //here you should define any mode of operation
	UART_BLOCKING,         //default mode: BLOKING one
	UART_NONBLOCKING,//NON  BLOKING one
	UART_CALLBACK,         //Callback non bloquing mode
    UART_TX_BLOCKING,         //default mode: BLOKING one
    UART_TX_NONBLOCKING,//NON  BLOKING one
    UART_TX_CALLBACK,         //Callback non bloquing mode
    UART_RX_BLOCKING,         //default mode: BLOKING one
    UART_RX_NONBLOCKING,//NON  BLOKING one
    UART_RX_CALLBACK,         //Callback non bloquing mode
}uart_mode;
//uart driver
#ifdef __cplusplus
  extern "C" {
#endif
//This is defined for the CC1312R
#include "ti_drivers_config.h"
#include <ti/drivers/UART2.h> //driver
#define uart_status_type int16_t
#define uart_callback  UART2_Callback//void*

  //UART2_Callback
typedef struct{
UART2_Handle     handle;     //handle of the obj
UART2_Params     parameters;  //parameters to set up+
uart_callback    callback; //if callback function is used // UART2_Callback
uart_mode        mode;       //mode of function (blocking, no blocking, ect)
uint8_t          number;//number of UARTs aviable to use. CC1312R has only 2.
}
uart_driver;
//--------------------------------------------------------------
typedef struct{
uart_driver driver;  //driver values, like handle, ect
uint32_t    speed;  // speed value, the max relies on the supported speed of the device
}
uart_struct;
/*--------------------------------------------------------------
 ******************Functions************************************
 *--------------------------------------------------------------*/

void uart_set(uart_struct* uart_p, uint8_t uart_number,uint32_t speed, uart_mode mode, uart_callback callback);
void uart_set_nonblocking(uart_struct* uart_p,uart_mode mode_tx, uart_callback callback_tx,uart_mode mode_rx, uart_callback callback_rx);

uart_status_type uart_init(uart_struct* uart_p);
uart_status_type uart_open(uart_struct* uart_p);
void uart_close(uart_struct* uart_p);
uart_status_type uart_send(uart_struct* uart_p,  const void* buffer_input,uint16_t buffer_max);//this function is called by "data_transfer" using a serial uart port
uart_status_type uart_recieve(uart_struct* uart_p, void* buffer_input,uint16_t buffer_max);//this function is called by "data_recieve" using a serial uart port
uart_status_type uart_recieve_reset(uart_struct* uart_p);
uart_status_type uart_recieve_cancel(uart_struct* uart_p);
#ifdef __cplusplus
   }
 #endif
#endif /*  */
