#ifndef RTOS_TASK_H_
#define RTOS_TASK_H_

#include <RTOS_AL/rtos_AL.h>
#include <RTOS_AL/semaphore_AL.h> //allow the use of an operative system

//semaphore for all process that share the use of the state machine
extern sempaphore_struct sem_state_machine;
#define SEM_STATE_TIMEOUT			1	 //seconds
//semaphore for sharing the i2c0 bus acces
extern sempaphore_struct sem_i2c0;
#define SEM_I2C0_TIMEOUT			0.1	 //seconds
//semaphore for sharing the spi0 bus acces

extern sempaphore_struct sem_spi0;       //semaphore for sharing the spi bus access, with micro sd
#define SEM_SPI0_TIMEOUT		    0.2  // 0.2 for this kind of sensors
//semaphore for sharing the uart 0 bus acces
extern sempaphore_struct sem_uart0;
#define SEM_UART0_TIMEOUT			0.2	 //seconds
//semaphore for sharing the uart 0 bus acces
extern sempaphore_struct sem_uart1;  //semaphore for sharing the uart 1 bus acces
#define SEM_UART1_TIMEOUT			0.1	 //seconds
//semaphore for sharing the adc0  drive access
extern sempaphore_struct sem_adc0;   //semaphore for sharing the uart 1 bus acces
#define SEM_ADC0_TIMEOUT			0.11//11 ms is max time for conversion
//semaphore for sharing teh rf sub1GHZ radio
extern sempaphore_struct sem_rfsub1GHz;
#define SEM_RFSUB1GHZ_TIMEOUT		0.5  //0.5 is timeout is for packabes lower than 300 bytes at en effective 5 KBPS baud rate
//semaphore for updating package tx
extern sempaphore_struct sem_rfsub1GHz_packetTx;
#define SEM_RFSUB1GHZ_PACKETTX_TIMEOUT		1.5  //1.5 is ok


#endif
