/*

 */
#include "gpio_AL.h"
//--------------------------
/* Read the gpio value, store the value and returns it*/
uint8_t gpio_read(gpio_struct* gpio_p)
{gpio_p->value=GPIO_read(gpio_p->pinout);
return(gpio_p->value);}
/* Set the gpio to ON (1) or OFF (0)*/
void gpio_write(gpio_struct* gpio_p, uint8_t value)
{GPIO_write(gpio_p->pinout, value);}
/* Toggles gpio output to the opposite value*/
void gpio_toggle(gpio_struct* gpio_p)
{GPIO_toggle(gpio_p->pinout);}
/* Initialize an gpio driver, used internally only */
void gpio_init_driver(void)
{GPIO_init();}
/* Initialize a gpio driver & one gpio channel */
void gpio_close(gpio_struct* gpio_p){
	;;//GPIO_close();
	}
/* Initialize a gpio driver & one gpio channel */
void gpio_init(gpio_struct* gpio_p)
{
//gpio_init_driver();
if (gpio_p->mode==GPIO_OUTPUT)
{GPIO_setConfig(gpio_p->pinout, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);}
if (gpio_p->mode==GPIO_OUTPUT_PULLUP_LOWDRIVE)
{GPIO_setConfig(gpio_p->pinout, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);}
if (gpio_p->mode==GPIO_INPUT)
{GPIO_setConfig(gpio_p->pinout, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);}
if (gpio_p->mode>GPIO_INPUT){
 GPIO_setConfig(gpio_p->pinout, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
 GPIO_setCallback(gpio_p->pinout, (gpio_p->driver.callback));
 GPIO_enableInt(gpio_p->pinout);
  }
}
/* Pass the parameters of gpio to set inside the structure
 * If you do not initialize parameters manually, you must call
 * this function before calling "gpio_init"
 * See the drivers examples for a better understanding*/
void gpio_set(gpio_struct* gpio_p,uint16_t pinout, gpio_mode mode, GPIO_CallbackFxn callback )
{
gpio_p->pinout=pinout;
gpio_p->mode=mode;
gpio_p->driver.callback=callback;
}

/* Initialize an gpio driver & an a group of gpio channels */
void gpio_init_array(gpio_struct* gpio_array_p[], uint8_t gpio_array_amount){
uint8_t i;//Start to iterate over all the adc drivers
    for (i=0; i < gpio_array_amount; i++) {
    	gpio_init(gpio_array_p[i]);
    }
}


