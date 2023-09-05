#ifndef TASK_UART1_GPS_H
#define TASK_UART1_GPS_H
#include <drivers_AL/uart_AL.h>//driver uart
#include "RTOS_TASKS_PARAMS.h"//driver uart
#include <gps_parser/TinyGPS++.h>
//function to track the store all desired
#define GPS_GPGSA_MSG_MAX   GPS_GPS_NUMBER_OF_MESSAGES  //Number of GPGSA messages to store
#define GPS_GPRMC_MSG_MAX   GPS_GPS_NUMBER_OF_MESSAGES  //Number of GPRMC messages to store

struct GPS_message_type{
    GPS_senstence_types type;//type of sentence
    int n_max;       //max number of messages to store
    int n;           //number of messages stores at the moment
    uint32_t timefix;//realtive time when the current n message has been fixed since gps turn on
};
#define GPS_MIN_CHAR_TO_PROCESS                  1
//UART 0 POARAMETERS
#define UART1_GPS_PORT_NUMBER                    uart_mu1
#define UART1_GPS_BAUD_RATE                      9600 // usse9408=9600*0.98 if gps is a replica like some NEO-6M units arround the marke
#define UART1_GPS_MODE                           UART_CALLBACK
#define UART1_GPS_CALLBACK                       callback_uart1_gps

//-----GPS ADQUISTION PARAMETERS
#define UART1_GPS_PARSER_TIMEOUT                 1                  //seconds if no message is revied, it will reset the app
#define UART1_GPS_RECEPTION_TIMEOUT              TASKP_GPS_TIME     //use operative system parameters
#define UART1_GPS_RECEPTION_PERIOD               TASKP_GPS_PERIOD   //use operative system parameters

#define UART1_GPS_FIRST_RECEPTION_TIMEOUT        1800 // //300seconds


#define GPIO_GPS_PIN                            gpio_on_gps
#define GPIO_GPS_MODE                           GPIO_OUTPUT_PULLUP_LOWDRIVE





#ifdef GPS_TEST_NO_GPS
   #ifndef PRINT_UART
     #define PRINT_UART
   #endif

#define UART1_GPS_BAUD_RATE                UART0_GPS_PRINT_BAUD_RATE
#define UART1_GPS_PORT_NUMBER              UART0_GPS_PRINT_PORT_NUMBER
static char gpsStream[] =
  "$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
  "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n"
  "$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n"
  "$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
  "$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A7D\r\n"//this has an error in checksym,
  "$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n"
  "$GPGSA,M,3,05,02,31,06,19,29,20,12,24,25,,,0.9,0.5,0.7*35\r\n" //this is non detected
  "$GNGSA,M,3,03,14,17,06,12,19,02,01,24,32,,,0.8,0.5,0.6*22\r\n"//     Example 2 (Multi-constellation):
    ;
static volatile size_t gpsStream_index=0;
#endif


/*returns result of fist adquisition timeout*/
bool get_gps_first_adquisition_result(void );

#endif
