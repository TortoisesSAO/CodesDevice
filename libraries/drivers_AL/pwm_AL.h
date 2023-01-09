#ifndef PWM_AL_H_
#define PWM_AL_H_

/* Driver Header files */
#include <ti/drivers/PWM.h>//to acces the pwm driver
/* Driver configuration */
#include "ti_drivers_config.h"

typedef struct{
PWM_Handle handle;
uint8_t    pinout;
}
pwm_driver;
typedef struct{
pwm_driver driver;  //driver values, like handle, ect
uint32_t duty_cicle;//duty cicle of the led to light
uint32_t period_hz;// period expressed in Hz
}
pwm_struct;

#ifdef __cplusplus
  extern "C" {
#endif
//To change the state of the
void pwm_init(pwm_struct* pwm_p);
/* Initialize an PWM driver & an a group of PWM channels */
void pwm_init_array(pwm_struct* pwm_array[], uint8_t pwm_array_amount);
/* Initialize an array of PWM  drivers */
#ifdef __cplusplus
   }
 #endif

#endif
