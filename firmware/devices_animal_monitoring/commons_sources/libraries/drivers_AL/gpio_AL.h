#ifndef GPIO_AL_H_
#define GPIO_AL_H_


/* Driver Header files */
#include <ti/drivers/GPIO.h> //to access the TI driver
typedef enum
{  //here you should define any mode of operation
	GPIO_OUTPUT, //default mode
	GPIO_OUTPUT_PULLUP_LOWDRIVE,//open drain with pullup, low current output
	GPIO_INPUT,  //default mode
	GPIO_INPUT_INT_FALLING,
	GPIO_INPUT_INT_RISING,
	GPIO_INPUT_INT_BOTH,
}gpio_mode;
/*--------------------------------------------------------------
 *Structures
 *--------------------------------------------------------------*/
typedef struct{
GPIO_PinConfig         config;  //pinconfig
void*                  callback;//if callback function is used
}
gpio_driver;
//----------------------------------------------------------------
typedef struct{
gpio_driver             driver;  //driver values, like handle, ect
gpio_mode               mode;    //0 input, 1 output
uint16_t                pinout;  //pin number
uint8_t                 value;   //on or off
}
gpio_struct;
/*--------------------------------------------------------------
 *Functions
 *--------------------------------------------------------------*/
#ifdef __cplusplus
  extern "C" {

#endif
/* Read the gpio value, store the value and returns it*/
uint8_t gpio_read(gpio_struct* gpio_p);
/* Set the gpio to ON (1) or OFF (0)*/
void gpio_write(gpio_struct* gpio_p, uint8_t value);
/* Toggles gpio output to the opposite value*/
void gpio_toggle(gpio_struct* gpio_p);
/* Pass the parameters of gpio to set inside the structure
 * If you do not initialize parameters manually, you must call
 * this function before calling "gpio_init"
 * See the drivers examples for a better understanding*/
void gpio_set(gpio_struct* gpio_p,uint16_t pinout, gpio_mode mode, GPIO_CallbackFxn callback );
/* Initialize a gpio driver & one gpio channel */
void gpio_init(gpio_struct* gpio_p);
/* Initialize an gpio driver & an a group of gpio channels */
void gpio_init_array(gpio_struct* gpio_p[], uint8_t gpio_array_amount);
/* Initialize an gpio driver, used internally only */
//void gpio_init_driver(void);
#ifdef __cplusplus
   }
 #endif
#endif
