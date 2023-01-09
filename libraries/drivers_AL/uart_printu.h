/*
 * uart_driver.h
 *
 *  Created on: 20 jul. 2022
 *  Author: Andres Oliva Trevisan
 */

#ifndef UART_AL_H_
#define UART_AL_H_

//this a library created to use a sprintf in microcontrollers
//but alowing to use uart serial port as the same time
//because of the high footprint, you can disable the library compilation with this macro

//#define PRINTU_ENABLED
//-----------BEGIN OF LIBRARY DEF
 #ifdef PRINTU_ENABLED
//This is defined for the CC1312R
#include <drivers_Al/uart_AL.h> //driver
#define PRINTU_UART
//#define PRINTU_DEFAULT
/*--------------------------------------------------------------
 *******************Macros**************************************
 *--------------------------------------------------------------*/
/*--------------------------------------------------------------
 *******************Structures**********************************
 *--------------------------------------------------------------*/
//uart driver
#ifdef __cplusplus
  extern "C" {
#endif
/*--------------------------------------------------------------
 ******************Functions************************************
 *--------------------------------------------------------------*/
/* Pass an initlized uart pointer to this function*/
int printu_start(uart_struct* uart_pointer);
//--------------------------------------------
/* example of user
printu("The sum of %d and %d is %d \n\r", 5, 6, 13);
will print:The sum of 5 and 6 is 13
*/
int printu(const char *str, ...);
#ifdef __cplusplus
   }
 #endif

#endif /*end of condition  */
  //-----------END OF LIBRARY DEF
#endif /*  */
