#ifndef GPS_UBLOX_H
#define GPS_UBLOX_H

/*
typedef enum
{  //here you should define any mode of operation
	GGA_GGRC_ON,
}gps_ublox_modes; */
//You can find other functions in the following repository:
//https://github.com/TomasTT7/TT7F-Float-Tracker/blob/master/Software/ARM_UBLOX.h
/*--------------------------------------------------------------
 *Functions
 *--------------------------------------------------------------*/
#include "gps_ublox_DEFINES.h"
#include "gps_ublox_TYPES.h"
//-----for com,munication drivers
#include <drivers_AL/gpio_AL.h>//driver uart
#include <drivers_AL/uart_AL.h>//driver uart
//for gps max buffer size
#include <gps_parser/TinyGPS_DEFINES.h>
//if you are NOT ussing other gps parser libary
/*#define _GPS_MESSAGE_FIELD_SIZE 85 */ //LEAVE COMMENTED UNLESS YOU ARE NOT USING THE INCLUDED TinyGPS PARSER.
//class define
class GPS_ublox
{
public:
	    GPS_ublox();
	    //settings;
	    //variables
	    volatile size_t uart_gps_bytes_read;//bytes readed in the last uart_read callback.
	    char uart_gps_rx_buffer[UART_GPS_BUFFER_MAX];
	    volatile size_t gps_bytes_acum;
	    char gps_buffer_acum[UART_GPS_BUFFER_MAX];
	    char gps_last_message[_GPS_MESSAGE_FIELD_SIZE]; //D0 NOT CHANGE SIZE, MUST BE _GPS_MESSAGE_FIELD_SIZE
	    char gps_date_text[8];
	    uint8_t nav_accuracy_m;
	   //strutcs
	   void init(uart_struct* uart_pointer,gpio_struct* gpio_pointer);
	   void configure(uint8_t nav_accuracy);//set up gps
	   void configure_rx_all(void);//set up gps
	   void turn_on(void);
	   void turn_off(void);
	   void recieve(void);
	   void reset_acum_buffer(void);

protected:
	   gpio_struct* gpio_p; //if use an external turn on/off
	   uart_struct* uart_p; //
	   void gpio_on(void);  //sets ON or OFF
	   void gpio_off(void);
	   void uart_tx_ublox(const char* char_p,uint16_t size);
	   void uart_rx_ublox( char* char_p,uint16_t size);
	   void uart_stop(void);
	   void uart_start(void);
	   void setNAV5_XXm_accuracy(uint8_t nav_accuracy);
};

#endif //GPS_UBLOX_H //

