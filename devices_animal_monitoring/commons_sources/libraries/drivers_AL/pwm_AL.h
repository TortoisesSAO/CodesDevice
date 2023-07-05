#ifndef PWM_AL_H_
#define PWM_AL_H_

/* Driver Header files */
#include <ti/drivers/PWM.h>//to acces the pwm driver
/* Driver configuration */
#include "ti_drivers_config.h"

//convert a percentual value to an input to PWM DUTY CICLE EXPRESSED AS FRACTION
#define PWM_TO_PERCENT(val)            val*(((uint32_t)~0)/100)

typedef enum
{  //here you should define any mode of operation
	PERIOD_US_DUTY_US,         //default mode: BLOKING one
	PERIOD_US_DUTY_FRACTION,
	PERIOD_HZ_DUTY_US,
	PERIOD_HZ_DUTY_FRACTION,
	PERIOD_COUNT_DUTY_COUNT,
	PERIOD_COUNT_DUTY_FRACTION,
}pwm_mode;

typedef struct{
PWM_Handle handle;
PWM_Params params;
uint8_t    pinout;
}
pwm_driver;
typedef struct{
pwm_driver driver;  //driver values, like handle, ect
pwm_mode mode;
uint32_t duty_cicle;//duty cicle of the led to light
uint32_t period;// period expressed in usec/Hz
}
pwm_struct;

#ifdef __cplusplus
  extern "C" {
#endif
void pwm_init_driver(void);
void pwm_close(pwm_struct* pwm_p);
/*set the period of the pwm wave*/
void pwm_set_period(pwm_struct* pwm_p, uint32_t period );
/*set the duty cicle of pwm*/
void pwm_set_duty(pwm_struct* pwm_p, uint32_t duty );
/*set the period of the pwm wave and its duty cicle*/
void pwm_set_period_and_duty(pwm_struct* pwm_p, uint32_t period, uint32_t duty );
 /*set the PWM driver*/
void pwm_set(pwm_struct* pwm_p, pwm_mode mode,uint32_t period,uint8_t pin );
/* Initialize an PWM driver & an a group of PWM channels */
void pwm_init(pwm_struct* pwm_p);
void pwm_stop(pwm_struct* pwm_p);
  /* Initialize an array of PWM  drivers */
void pwm_init_array(pwm_struct* pwm_array[], uint8_t pwm_array_amount);
#ifdef __cplusplus
   }
 #endif

#endif
