/*

 */

#include "gps_ublox.h"

GPS_ublox::GPS_ublox()
{
	//gpio_p = NULL;
}
void GPS_ublox::init(uart_struct* uart_pointer,gpio_struct* gpio_pointer){
	gpio_p=gpio_pointer;
	uart_p=uart_pointer;
	uart_gps_bytes_read=0;
	gps_bytes_acum=0;

}
void GPS_ublox::setNAV5_XXm_accuracy(uint8_t nav_accuracy){
if ((10>=nav_accuracy) and(0<nav_accuracy)){uart_tx_ublox(setNAV5_10m_accuracy, sizeof(setNAV5_10m_accuracy));}
if ((20>=nav_accuracy) and(10<nav_accuracy)){uart_tx_ublox(setNAV5_20m_accuracy, sizeof(setNAV5_20m_accuracy));}
if ((30>=nav_accuracy) and(10<nav_accuracy)){uart_tx_ublox(setNAV5_30m_accuracy, sizeof(setNAV5_30m_accuracy));}
if ((50>=nav_accuracy) and(20<nav_accuracy)){uart_tx_ublox(setNAV5_50m_accuracy, sizeof(setNAV5_50m_accuracy));}
if ((100>=nav_accuracy) and(50<nav_accuracy)){uart_tx_ublox(setNAV5_100m_accuracy, sizeof(setNAV5_100m_accuracy));}
}
void GPS_ublox::configure(uint8_t nav_accuracy){
	//sets resolution
    nav_accuracy_m=nav_accuracy;
    setNAV5_XXm_accuracy(nav_accuracy);
    //set reception of messages
	uart_tx_ublox(RMC_On, sizeof(RMC_On));
	uart_tx_ublox(GSA_On, sizeof(GSA_On));
	uart_tx_ublox(ZDA_Off, sizeof(ZDA_Off));
	uart_tx_ublox(GGA_Off, sizeof(GGA_Off));
	uart_tx_ublox(GLL_Off, sizeof(GLL_Off));
	uart_tx_ublox(THS_Off, sizeof(THS_Off));
	uart_tx_ublox(VTG_Off, sizeof(VTG_Off));
	uart_tx_ublox(GSV_Off, sizeof(GSV_Off));
}
void GPS_ublox::configure_rx_all(void){
    //working, allow the reception of all frames with an accuracy of 10
    uart_tx_ublox(set_UBLOXdefaut_config, sizeof(set_UBLOXdefaut_config));
    uart_tx_ublox(setNAV5_10m_accuracy, sizeof(setNAV5_10m_accuracy));
}
void GPS_ublox::turn_on(void){
	gpio_on();
	uart_start();
}
void GPS_ublox:: turn_off(void){
	gpio_off();
	uart_stop();
}

void GPS_ublox:: recieve(void){
	 uart_rx_ublox(uart_gps_rx_buffer,UART_CALLBACK_BUFFER_TRIGGER);

}
void GPS_ublox:: reset_acum_buffer(void){
	 uart_gps_bytes_read=0;
	 gps_bytes_acum=0;
	 uart_rx_ublox(uart_gps_rx_buffer,UART_CALLBACK_BUFFER_TRIGGER);
}

void GPS_ublox::gpio_on(void){
gpio_write(gpio_p, GPIO_GPS_ON);
};
void GPS_ublox::gpio_off(void){
gpio_write(gpio_p, GPIO_GPS_OFF);
};
void GPS_ublox::uart_tx_ublox(const  char* char_message,uint16_t size){
uart_send(uart_p,char_message, size);//sends ubblox message to setup
uart_send(uart_p,"\r\n", 2);         //sends breakline message to make message valid
}

void GPS_ublox::uart_rx_ublox( char* char_message,uint16_t size){
    uart_recieve(uart_p, char_message, size);
}
void GPS_ublox::uart_stop(void){
	uart_recieve_reset(uart_p);
	uart_close(uart_p);
}
void GPS_ublox::uart_start(void){
	uart_open(uart_p);
}



