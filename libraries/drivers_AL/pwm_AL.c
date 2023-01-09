/*
 *  ======== pwmled2.c ========
 */

#include "pwm_AL.h"

#ifdef LAUNCHPAD_TEST_MODE
/* For usleep() */
#include <unistd.h> //for sleep
#endif
/* Initialize an PWM driver engine
 * This must be called before enable any PWM channel */

void pwm_init_driver(void)
{    PWM_init();}
/* Initialize an PWM driver & an specific PWM channel */

void pwm_init(pwm_struct* pwm_p){
    PWM_Params params;
    pwm_init_driver();          //first we init the app
    PWM_Params_init(&params);//then we init the parameters
   //init
	(pwm_p->driver).handle = PWM_open((pwm_p->driver).pinout, &params);
    #ifdef DEBUG_MODE
       if ((pwm_p->driver).handle== NULL) {while (1); } /* CONFIG_ADC did not open */
   #endif
}

void pwm_init_arraypwm_init_array(pwm_struct* pwm_array[], uint8_t pwm_array_amount){
    uint8_t total_elements;
    PWM_Params params;
    pwm_init_driver();          //first we init the app
    PWM_Params_init(&params);//then we init the parameters
    //if pwm_array_amount==0, inits all elements of the array
    if (pwm_array_amount==0){pwm_array_amount=(sizeof(pwm_array))/sizeof(pwm_struct*);}
    else                    {total_elements=pwm_array_amount;}
    uint8_t i;//Start to iterate over all the leds drivers
    for (i=0; i < total_elements; i++) {
	    (pwm_array[i]->driver).handle = PWM_open((pwm_array[i]->driver).pinout, &params);
     #ifdef LAUNCHPAD_TEST_MODE
        if ((led_pwm[i]->driver).handle== NULL) {while (1); } /* CONFIG_PWM did not open */
     #endif
    }
	}
