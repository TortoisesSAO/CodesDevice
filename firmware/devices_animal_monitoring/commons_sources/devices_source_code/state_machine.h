
#ifndef _STATE_MACHINE_H_
 #define _STATE_MACHINE_H_

//header for main state machine
#include <device_system_setup.h>
#include <state_machine/device_tortoise1.1/Statechart.h>
//for testing state machine
//posix librarys
#include <drivers_AL/pwm_AL.h>
#include <drivers_AL/gpio_AL.h>

#define GPIO_LED_STATUS0_PIN       gpio_led_status0
#define GPIO_LED_STATUS1_PIN       gpio_led_status1
#define GPIO_LED_STATUS2_PIN       gpio_led_status2

//scales are defined in:
#ifdef LAUNCHPAD_TEST_MODE //to use a CC1312R launchpad as a hardware platform
#define ADC_BATTERY_LEVEL_SCALE    1//1 //
#define ADC_CHARGER_CURRENT_SCALE  1//1 //
#define ADC_USB_LEVEL_SCALE        1//1 //
#else
#define GPIO_LED_STATUS0_MODE          PERIOD_HZ_DUTY_FRACTION//PERIOD_COUNT_DUTY_FRACTION
#define GPIO_LED_STATUS1_MODE          PERIOD_HZ_DUTY_FRACTION //PERIOD_HZ_DUTY_FRACTION
#define GPIO_LED_STATUS2_MODE          PERIOD_HZ_DUTY_FRACTION//PERIOD_US_DUTY_FRACTION
#define GPIO_LED_STATUS0_PERIOD        4//16000000// //MAX PERIOD COUNT:16000000 MAX PERIOD US: 343330 US,MIN HZ=3
#define GPIO_LED_STATUS1_PERIOD        4//3
#define GPIO_LED_STATUS2_PERIOD        4//343330

//#define GPIO_LED_STATUS_LOW_POWER_LED             18 commented//2023/06: DEFINED IN #include "device_system_config.h"
#define GPIO_LED_STATUS_LOW_POWER_LED_MODE        GPIO_OUTPUT_PULLUP_LOWDRIVE
#endif
//Setup parameters
#define LED_DUTY_100                PWM_TO_PERCENT(100) //%
#define LED_DUTY_75                 PWM_TO_PERCENT(75) //%
#define LED_DUTY_50                 PWM_TO_PERCENT(50) //%
#define LED_DUTY_25                 PWM_TO_PERCENT(25) //%
#define LED_DUTY_10                 PWM_TO_PERCENT(10) //%
#define LED_DUTY_5                  PWM_TO_PERCENT(5) //%

#define LED_PERIOD_LEVEL0           3  //Hz
#define LED_PERIOD_LEVEL1           3  //Hz
#define LED_PERIOD_LEVEL2           5  //Hz
#define LED_PERIOD_LEVEL3           8  //Hz

#define STATE_MACHINE_DELAY         0.5 //[seconds]: time to loop

extern Statechart state_machine;

#ifdef __cplusplus
  extern "C" {
#endif

bool state_machine_is_shutdown(void);
bool state_machine_is_lowpower(void);
bool state_machine_is_acquisition(void);
void state_machine_wait_for_ADQ_DATA_state(void);
void state_machine_wait_for_state(Statechart::StatechartStates state);
bool state_machine_is_state_active(Statechart::StatechartStates state);
void set_pwm_state_machine(void );
void state_machine_leds_set(void);
void state_machine_led_ka_set_off(void);
void state_machine_led_ka_set_on(void);

#ifdef __cplusplus
   }
 #endif


#endif

