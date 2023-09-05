/*
 *  ======== pwmled2.c ========
 */

#include "pwm_AL.h"

/* Initialize an PWM driver engine*/

 /* This must be called before enable any PWM channel */
void pwm_init_driver(void)
{    PWM_init();
}
/* CLose the PWM Driver */
void pwm_close(pwm_struct* pwm_p){
    if ((pwm_p->driver).handle> 0) {PWM_close(pwm_p->driver.handle);}
}
/* Initialize an PWM driver & an specific PWM channel */
void pwm_set(pwm_struct* pwm_p, pwm_mode mode,uint32_t period,uint8_t pin ){
	pwm_p->mode=mode;
	pwm_p->period=period;
	pwm_p->driver.pinout=pin;
}
void pwm_set_duty(pwm_struct* pwm_p, uint32_t duty ){
	PWM_setDuty(pwm_p->driver.handle, duty);
}
void pwm_set_period(pwm_struct* pwm_p, uint32_t period ){
	PWM_setPeriod(pwm_p->driver.handle, period);
}
void pwm_set_period_and_duty(pwm_struct* pwm_p, uint32_t period, uint32_t duty ){
	pwm_set_period(pwm_p, period);
	pwm_set_duty(pwm_p,duty);
	//PWM_setDutyAndPeriod(pwm_p->driver.handle, duty,period);
	//pwm_set_duty(pwm_p,duty);
}
void pwm_stop(pwm_struct* pwm_p){
    if ((pwm_p->driver).handle> 0) {PWM_stop(pwm_p->driver.handle);}
}


void pwm_init(pwm_struct* pwm_p){
    pwm_p->driver.handle=0;
    PWM_Params_init(&(pwm_p->driver.params));//then we init the parameters

    switch (pwm_p->mode){
        case PERIOD_US_DUTY_US:
    		pwm_p->driver.params.dutyUnits    = PWM_DUTY_US;
    		pwm_p->driver.params.periodUnits  = PWM_PERIOD_US;
          break;
        case PERIOD_US_DUTY_FRACTION:
    		pwm_p->driver.params.dutyUnits    = PWM_DUTY_FRACTION;
    		pwm_p->driver.params.periodUnits  = PWM_PERIOD_US;
          break;
        case PERIOD_HZ_DUTY_US:
     	   pwm_p->driver.params.dutyUnits   = PWM_DUTY_US;
     	   pwm_p->driver.params.periodUnits = PWM_PERIOD_HZ;
        case PERIOD_HZ_DUTY_FRACTION:
     	   pwm_p->driver.params.dutyUnits   = PWM_DUTY_FRACTION;
     	   pwm_p->driver.params.periodUnits = PWM_PERIOD_HZ;
          break;
        case PERIOD_COUNT_DUTY_COUNT:
     	   pwm_p->driver.params.dutyUnits   = PWM_DUTY_COUNTS;
     	   pwm_p->driver.params.periodUnits = PWM_PERIOD_COUNTS;
          break;
        case PERIOD_COUNT_DUTY_FRACTION:
     	   pwm_p->driver.params.dutyUnits   = PWM_DUTY_FRACTION;
     	   pwm_p->driver.params.periodUnits = PWM_PERIOD_COUNTS;
          break;
         }


	/*if (pwm_p->mode==PERIOD_US_DUTY_US){
		pwm_p->driver.params.dutyUnits    = PWM_DUTY_US;
		pwm_p->driver.params.periodUnits  = PWM_PERIOD_US;
     }
   if (pwm_p->mode==PERIOD_HZ_DUTY_US){
	   pwm_p->driver.params.dutyUnits   = PWM_DUTY_US;
	   pwm_p->driver.params.periodUnits = PWM_PERIOD_HZ;
     }*/
   pwm_p->driver.params.dutyValue   = 0;
   pwm_p->driver.params.periodValue = pwm_p->period;
   //init
   pwm_p->driver.handle = PWM_open((pwm_p->driver).pinout, &(pwm_p->driver.params));
	if ((pwm_p->driver).handle> 0) {
		PWM_start(pwm_p->driver.handle);} /* PWM did not open */
    #ifdef DEBUG_MODE
       if ((pwm_p->driver).handle== NULL) {while (1); } /* PWM did not open */
   #endif
}

void pwm_init_array(pwm_struct* pwm_array[], uint8_t pwm_array_amount){
    uint8_t i;//Start to iterate over all the leds drivers
    for (i=0; i < pwm_array_amount; i++) {
    	pwm_init(pwm_array[i]);
	   // (pwm_array[i]->driver).handle = PWM_open((pwm_array[i]->driver).pinout, &(pwm_array[i]->driver.params));
     #ifdef LAUNCHPAD_TEST_MODE
        if ((led_pwm[i]->driver).handle== NULL) {while (1); } /* CONFIG_PWM did not open */
     #endif
    }
	}
