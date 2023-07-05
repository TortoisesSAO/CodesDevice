/*
 * i2c_driver.c
 *
 *  Created on: 4 jul. 2022
 *      Author: Nicro
 */
#include "i2c_AL.h"  //
#include <stdbool.h>
bool i2c_init_flag=false;
void i2c_set(i2c_struct* i2c_p, uint8_t i2c_number,uint8_t slave_address,i2c_speed speed, i2c_Mode mode, i2c_callback callback){
i2c_p->speed = speed;
i2c_p->driver.mode = mode;
i2c_p->driver.number = i2c_number;
i2c_p->driver.callback = callback;
//set slave addres of the driver
i2c_p->driver.transaction.slaveAddress=slave_address;
i2c_p->driver.handle=NULL;
}
i2c_status_type i2c_init(i2c_struct* i2c_p)
{
if (i2c_init_flag==false) {
    I2C_init();
    i2c_init_flag=true;
        }
if (i2c_p->driver.handle==NULL){
    I2C_Params_init(&(i2c_p->driver.parameters));
//stets speed
    if(i2c_p->speed==i2c_100kbits){i2c_p->driver.parameters.bitRate=I2C_100kHz;}
    if(i2c_p->speed==i2c_400kbits){i2c_p->driver.parameters.bitRate=I2C_400kHz;}
    if(i2c_p->speed==i2c_1000kbits){i2c_p->driver.parameters.bitRate=I2C_1000kHz;}
    if(i2c_p->speed==i2c_3330kbits){i2c_p->driver.parameters.bitRate=I2C_3330kHz;}
    if(i2c_p->speed==i2c_3400kbits){i2c_p->driver.parameters.bitRate=I2C_3400kHz;}
//sets mode
    if(i2c_p->driver.mode==I2C_BLOCKING){i2c_p->driver.parameters.transferMode=I2C_MODE_BLOCKING;}
    if(i2c_p->driver.mode==I2C_CALLBACK){i2c_p->driver.parameters.transferMode=I2C_MODE_CALLBACK;}
    i2c_p->driver.parameters.transferCallbackFxn=i2c_p->driver.callback;
//open and init the interface
    i2c_p->driver.handle = I2C_open(i2c_p->driver.number, &(i2c_p->driver.parameters));
    if (i2c_p->driver.handle == NULL) {return(0);}/* i2c_open() failed:chek your hardware */
 return(1);
}
else {return(1);}
}

void i2c_start(i2c_struct* i2c_p){
if (i2c_p->driver.handle==NULL){
    i2c_p->driver.handle = I2C_open(i2c_p->driver.number, &(i2c_p->driver.parameters));
}
}
void i2c_stop(i2c_struct* i2c_p){
if (i2c_p->driver.handle!=NULL){
    I2C_close(i2c_p->driver.handle);
    i2c_p->driver.handle=NULL;
}
}
void i2c_set_slave_addres(i2c_struct* i2c_p,uint8_t slave_address){
i2c_p->driver.transaction.slaveAddress=slave_address;
}
i2c_status_type i2c_send_receive(i2c_struct* i2c_p, void * buff_w,uint16_t buff_w_max, void * buff_r,uint16_t buff_r_max){

i2c_p->driver.transaction.writeBuf = buff_w;//must be defined in this way to work
i2c_p->driver.transaction.writeCount = buff_w_max;
i2c_p->driver.transaction.readBuf = buff_r;//must be defined in this way to work
i2c_p->driver.transaction.readCount = buff_r_max;
return(I2C_transfer(i2c_p->driver.handle, &(i2c_p->driver.transaction)) );
}





