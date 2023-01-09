/*
 * i2c_driver.h
 *
 *  Created on: 20 jul. 2022
 *  Author: Andres Oliva Trevisan
 */

#ifndef I2C_AL_H_
#define I2C_AL_H_

#include <stddef.h>//for declaration of variables
//This is defined for the CC1312R
#include <ti/drivers/i2c.h> //driver
/*--------------------------------------------------------------
 *******************Macros**************************************
 *--------------------------------------------------------------*/
#define i2c_status_type int_fast16_t
/*--------------------------------------------------------------
 *******************Structures**********************************
 *--------------------------------------------------------------*/
//i2c driver
#define i2c_callback  I2C_CallbackFxn
typedef enum
{  //here you should define any mode of operation
	I2C_BLOCKING,//default mode: BLOKING one
	//NONBLOCKING, leave commented
	I2C_CALLBACK,//Callback non bloquing mode
}i2c_Mode;
typedef enum
{
    i2c_100kbits, /*!<      100 kbit/s. */
	i2c_400kbits, /*!<    400 kbit/s. */
	i2c_1000kbits, /*!< Up to 1Mbit/s. */
	i2c_3330kbits, /*!< Up to 3.4Mbit/s. */
    i2c_3400kbits, /*!< Up to 3.4Mbit/s. */
}i2c_speed;
typedef struct{
I2C_Handle       handle;       //handle of the obj
I2C_Params       parameters;   //parameters to set up+
I2C_Transaction  transaction;  //parameters to send/revieve.
void*            callback;     //if callback function is used // i2c2_Callback
i2c_Mode         mode;         //mode of function (blocking, no blocking, ect)
uint8_t          number;       //number of i2cs aviable to use. CC1312R has only 2.
}
i2c_driver;
//--------------------------------------------------------------
typedef struct{
i2c_driver        driver;  //driver values, like handle, ect
i2c_speed         speed;// wait
uint8_t           slave_address;//slave address of the device
}
i2c_struct;
/*--------------------------------------------------------------
 ******************Functions************************************
 *--------------------------------------------------------------*/
#ifdef __cplusplus
  extern "C" {
#endif
void i2c_set(i2c_struct* i2c_p, uint8_t i2c_number,uint8_t slave_address,i2c_speed speed, i2c_Mode mode, i2c_callback callback);
i2c_status_type i2c_init(i2c_struct* i2c_p);
void i2c_close(i2c_struct* i2c_p);
void i2c_set_slave_addres(i2c_struct* i2c_p,uint8_t slave_address);
//this function is called by "data_transfer" using a serial i2c port
i2c_status_type i2c_send_receive(i2c_struct* i2c_p, void * buff_w,uint16_t buff_w_max, void * buff_r,uint16_t buff_r_max);
#ifdef __cplusplus
   }
 #endif
#endif /*  */
