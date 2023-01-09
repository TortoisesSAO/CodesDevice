/*
 * uart_driver.c
 *
 *  Created on: 4 jul. 2022
 *      Author: Nicro
 */
#include "uart_AL.h"  //

void uart_set(uart_struct* uart_p,uint8_t uart_number,uint32_t speed, uart_mode mode, uart_callback callback)
{
uart_p->speed = speed;
uart_p->driver.mode=mode;//sets mode
uart_p->driver.number = uart_number;
uart_p->driver.callback =callback;
//uart_p->driver.parameters.readCallback = (uart_callback) callback;

}

uart_status_type uart_init(uart_struct* uart_p)
{
UART2_Params_init(&(uart_p->driver.parameters));
if(uart_p->driver.mode==UART_BLOCKING){uart_p->driver.parameters.readMode=UART2_Mode_BLOCKING;}
if(uart_p->driver.mode==UART_NONBLOCKING){uart_p->driver.parameters.readMode=UART2_Mode_NONBLOCKING;}
if(uart_p->driver.mode==UART_CALLBACK){
	uart_p->driver.parameters.readMode=UART2_Mode_CALLBACK;
	uart_p->driver.parameters.readCallback =(uart_p->driver.callback);
  }
 uart_p->driver.parameters.baudRate=uart_p->speed;
//open and init the interface
 uart_p->driver.handle = UART2_open(uart_p->driver.number, &(uart_p->driver.parameters));
 if (uart_p->driver.handle == NULL) {return(0);}/* UART2_open() failed */
 return(1);
}

void uart_close(uart_struct* uart_p)
{UART2_close(&(uart_p->driver.handle));
	}

uart_status_type uart_send(uart_struct* uart_p,const void * buffer_input,uint16_t buffer_max){
return(UART2_write(uart_p->driver.handle, buffer_input, buffer_max, NULL));
}

uart_status_type uart_recieve(uart_struct* uart_p,const void *buffer_input,uint16_t buffer_max){
return(UART2_read(uart_p->driver.handle, buffer_input, buffer_max, NULL));
}




