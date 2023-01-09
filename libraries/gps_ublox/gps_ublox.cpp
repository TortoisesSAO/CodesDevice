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
void GPS_ublox::configure(void){
    //working
	//uart_tx_ublox(GPSlongupdate, sizeof(GPSlongupdate));//to check if makes a difference in DOA
	uart_tx_ublox(RMC_On, sizeof(RMC_On));
	uart_tx_ublox(GSA_On, sizeof(GSA_On));
	uart_tx_ublox(ZDA_Off, sizeof(ZDA_Off));
	uart_tx_ublox(GGA_Off, sizeof(GGA_Off));
	uart_tx_ublox(GLL_Off, sizeof(GLL_Off));
	uart_tx_ublox(THS_Off, sizeof(THS_Off));
	uart_tx_ublox(VTG_Off, sizeof(VTG_Off));
	uart_tx_ublox(GSV_Off, sizeof(GSV_Off));
}
void GPS_ublox::turn_on(void){
	gpio_on();
}
void GPS_ublox:: turn_off(void){
	gpio_off();
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

void GPS_ublox::uart_rx_ublox(const char* char_message,uint16_t size){
    uart_recieve(uart_p, char_message, size);
}


