/*

 *  ======== pwmled2.c ========
 */

/* Driver Header files */
#include <drivers_AL/pwm_AL.h>
#include <drivers_AL/gpio_AL.h>
#include "state_machine.h"
#include "rtos_tasks.h"
/* Driver configuration */
static pwm_struct pwm_led_status0_s,pwm_led_status1_s,pwm_led_status2_s;
static gpio_struct led_status1_s;
//pwm_led_status0_s.mode=GPIO_LED_STATUS0_MODE;pwm_led_status0_s.period=GPIO_LED_STATUS0_PERIOD;pwm_led_status0_s.driver.pinout=GPIO_LED_STATUS0_PIN;
//pwm_struct pwm_led_status1_s = {.mode=GPIO_LED_STATUS1_MODE,.period=GPIO_LED_STATUS1_PERIOD,.driver.pinout=GPIO_LED_STATUS1_PIN};
//pwm_struct pwm_led_status2_s = {.mode=GPIO_LED_STATUS2_MODE,.period=GPIO_LED_STATUS2_PERIOD,.driver.pinout=GPIO_LED_STATUS2_PIN};
//If we want to use an array to handle all (not worth it with only to inputs)
static pwm_struct* pwm_array[]={&pwm_led_status0_s,&pwm_led_status1_s,&pwm_led_status2_s};

/*
 *  ======== mainThread ========
 *  Task periodically increments the PWM duty for the on board LED.
 */
//Yakindu state machine  example
Statechart state_machine;
Statechart* state_machine_p=&state_machine;

static bool state_machine_acq_active= false;
static bool state_machine_low_power_led_on=false;//enables the set up of a low power led
static TickType_t time_to_turn_led=0;

//turn off all and open keep alive led
void state_machine_leds_turn_off_PWM(void){
    //init the toggle
	if (state_machine_low_power_led_on==false){
	  if (pwm_array[0]->period>0){
	    pwm_stop(pwm_array[0]);
	    pwm_close(pwm_array[0]);
	    pwm_array[0]->period=0;}
	  if (pwm_array[1]->period>0){
	     pwm_stop(pwm_array[1]);
	     pwm_close(pwm_array[1]);
	     pwm_array[1]->period=0;}
	  if (pwm_array[2]->period>0){
	       pwm_stop(pwm_array[2]);
	       pwm_close(pwm_array[2]);
	       pwm_array[2]->period=0;}
      gpio_set(&led_status1_s,GPIO_LED_STATUS_LOW_POWER_LED,GPIO_LED_STATUS_LOW_POWER_LED_MODE,NULL);
      gpio_init(&led_status1_s);
	 state_machine_low_power_led_on=true;//to ensure PMW drivers are only closed one time
	}
}

void state_machine_leds_turn_off_all_set_ka(void){
	   if((!state_machine_low_power_led_on)and(state_machine_acq_active)){
	     if (time_to_turn_led==0){time_to_turn_led=xTaskGetTickCount();}
	       else{
	    	    if( (xTaskGetTickCount()-time_to_turn_led)>SEC_TO(LED_PERIOD_ADQ_START_TIME) ){
	    	     state_machine_leds_turn_off_PWM();
	          }
       }
   }
}
void state_machine_leds(void){

if (state_machine_low_power_led_on==false){
if (state_machine_p->isStateActive(state_machine_p->SM_START)){
		pwm_set_period_and_duty(pwm_array[0],LED_PERIOD_LEVEL0, LED_DUTY_100);
		pwm_set_period_and_duty(pwm_array[1],LED_PERIOD_LEVEL0, 0);
		pwm_set_period_and_duty(pwm_array[2],LED_PERIOD_LEVEL0, 0);
		 }
//CHECKING BAT
if (state_machine_p->isStateActive(state_machine_p->SM_BAT_CHARGING)){
	pwm_set_period_and_duty(pwm_array[0],LED_PERIOD_LEVEL0, LED_DUTY_100);
	pwm_set_period_and_duty(pwm_array[1],LED_PERIOD_LEVEL0, LED_DUTY_50);
	pwm_set_period_and_duty(pwm_array[2],LED_PERIOD_LEVEL0, 0);
	 }
if (state_machine_p->isStateActive(state_machine_p->SM_BAT_FULL)){
	pwm_set_period_and_duty(pwm_array[0],LED_PERIOD_LEVEL0, LED_DUTY_100);
	pwm_set_period_and_duty(pwm_array[1],LED_PERIOD_LEVEL0, LED_DUTY_100);
	pwm_set_period_and_duty(pwm_array[2],LED_PERIOD_LEVEL0, 0);
	 }
//BAT OK
if (state_machine_p->isStateActive(state_machine_p->SM_BAT_CHECK)){
	pwm_set_period_and_duty(pwm_array[0],LED_PERIOD_LEVEL0, LED_DUTY_10);
	pwm_set_period_and_duty(pwm_array[1],LED_PERIOD_LEVEL0, 0);
	pwm_set_period_and_duty(pwm_array[2],LED_PERIOD_LEVEL0, 0);
	 }
if (state_machine_p->isStateActive(state_machine_p->SM_BAT_LOW)){
    #ifdef SYSTEM_FORCE_LOWPOWER_AFTER_START
    state_machine_leds_turn_off_PWM();//for testing: force a PWM engine shutdown
    #endif
    if(state_machine_low_power_led_on==false){//only will change the leds if they are running
        pwm_set_period_and_duty(pwm_array[0],LED_PERIOD_LEVEL0, LED_DUTY_5);
        pwm_set_period_and_duty(pwm_array[1],LED_PERIOD_LEVEL0, LED_DUTY_5);
        pwm_set_period_and_duty(pwm_array[2],LED_PERIOD_LEVEL0, 0);
        }
	 }
//SD
if (state_machine_p->isStateActive(state_machine_p->SM_SD_CHECK)){
	pwm_set_period_and_duty(pwm_array[0],LED_PERIOD_LEVEL1, LED_DUTY_50);
	pwm_set_period_and_duty(pwm_array[1],LED_PERIOD_LEVEL1, 0);
	pwm_set_period_and_duty(pwm_array[2],LED_PERIOD_LEVEL1, 0);
	 }
if (state_machine_p->isStateActive(state_machine_p->SM_SD_NONC0NNECT)){
	pwm_set_period_and_duty(pwm_array[0],LED_PERIOD_LEVEL1, LED_DUTY_75);
	pwm_set_period_and_duty(pwm_array[1],LED_PERIOD_LEVEL1, 0);
	pwm_set_period_and_duty(pwm_array[2],LED_PERIOD_LEVEL1, 0);
	 }
//GPS FIX OR NOT FIX
if (state_machine_p->isStateActive(state_machine_p->SM_GPS_WAIT_POS_FIX)){
	pwm_set_period_and_duty(pwm_array[0],LED_PERIOD_LEVEL2, LED_DUTY_50);
	pwm_set_period_and_duty(pwm_array[1],LED_PERIOD_LEVEL2, LED_DUTY_10);
	pwm_set_period_and_duty(pwm_array[2],LED_PERIOD_LEVEL2, 0);
	 }
if (state_machine_p->isStateActive(state_machine_p->SM_GPS_NONFIX)){
	pwm_set_period_and_duty(pwm_array[0],LED_PERIOD_LEVEL2, LED_DUTY_50);
	pwm_set_period_and_duty(pwm_array[1],LED_PERIOD_LEVEL2, LED_DUTY_50);
	pwm_set_period_and_duty(pwm_array[2],LED_PERIOD_LEVEL2, 0);
	 }
if (state_machine_p->isStateActive(state_machine_p->SM_ADQ_DATA)){
    if (state_machine_acq_active==false){
      pwm_set_period_and_duty(pwm_array[0],LED_PERIOD_LEVEL3, LED_DUTY_10);
      pwm_set_period_and_duty(pwm_array[1],LED_PERIOD_LEVEL3, LED_DUTY_10);
      pwm_set_period_and_duty(pwm_array[2],LED_PERIOD_LEVEL3, LED_DUTY_10);
      state_machine_acq_active=true;
      }
    }
}
if (state_machine_p->isStateActive(state_machine_p->SM_ADQ_DATA)){
    if (state_machine_acq_active==false){    state_machine_acq_active=true;}
}
//Start Data ACQ.
if (state_machine_p->isStateActive(state_machine_p->SM__final_)){
		state_machine_leds_turn_off_PWM();
}
}
/*Turn designed  keep alive led on*/
void state_machine_led_ka_set_on(void){
  if (semaphore_trytotake(&sem_state_machine)==true){
	   if (state_machine_low_power_led_on){gpio_write(&led_status1_s,1);}
	   else{state_machine_leds_turn_off_all_set_ka();}
	    semaphore_release(&sem_state_machine);//release the semaphore after implementing changes}
   }
}
/*Turn designed  keep alive led off*/
void state_machine_led_ka_set_off(void){
  if (semaphore_trytotake(&sem_state_machine)==true){
	    if (state_machine_low_power_led_on){gpio_write(&led_status1_s,0);};
	    semaphore_release(&sem_state_machine);//release the semaphore after implementing changes}
   }
}
void state_machine_leds_set(void){
  if (semaphore_trytotake(&sem_state_machine)==true){
	    state_machine_leds();
	    semaphore_release(&sem_state_machine);//release the semaphore after implementing changes}
   }
}
bool state_machine_is_state_active(Statechart::StatechartStates state){
bool result=false;
	if (semaphore_trytotake(&sem_state_machine)==true){
		if (state_machine.isStateActive(state)){result=true;}
	     semaphore_release(&sem_state_machine);//release the semaphore after implementing changes
	     }
return result;
}
bool state_machine_is_shutdown(void){
    bool result=false;
    if (semaphore_trytotake(&sem_state_machine)==true){
        if (state_machine.isStateActive(state_machine.SM_SHUTDOWN)){result=true;}
         semaphore_release(&sem_state_machine);//release the semaphore after implementing changes
         }
    return result;
}
/*if state is low power or lowest (such as shutdown, must return true!)*/
bool state_machine_is_lowpower(void){
    bool result=false;
    if (semaphore_trytotake(&sem_state_machine)==true){
        if (state_machine.isStateActive(state_machine.SM_BAT_LOW)){result=true;}
        if (state_machine.isStateActive(state_machine.SM_SHUTDOWN)){result=true;}
         semaphore_release(&sem_state_machine);//release the semaphore after implementing changes
         }
    return result;
}
bool state_machine_is_acquisition(void){
    bool result=false;
    if (semaphore_trytotake(&sem_state_machine)==true){
        if (state_machine.isStateActive(state_machine.SM_ADQ_DATA)){result=true;}
         semaphore_release(&sem_state_machine);//release the semaphore after implementing changes
         }
    return result;
}

void state_machine_wait_for_ADQ_DATA_state(void){
 while(state_machine_is_acquisition()==false){
      vTaskDelay(SEC_TO(STATE_MACHINE_DELAY));}
}
void state_machine_wait_for_state(Statechart::StatechartStates state){
  while(state_machine_is_state_active(state)==false){
	  vTaskDelay(SEC_TO(STATE_MACHINE_DELAY));}
}
void set_pwm_state_machine(void ){
    pwm_init_driver();
    pwm_set(&pwm_led_status0_s,GPIO_LED_STATUS0_MODE,GPIO_LED_STATUS0_PERIOD,GPIO_LED_STATUS0_PIN);
    pwm_set(&pwm_led_status1_s,GPIO_LED_STATUS1_MODE,GPIO_LED_STATUS1_PERIOD,GPIO_LED_STATUS1_PIN);
    pwm_set(&pwm_led_status2_s,GPIO_LED_STATUS2_MODE,GPIO_LED_STATUS2_PERIOD,GPIO_LED_STATUS2_PIN);
    pwm_init(&pwm_led_status0_s);
    pwm_init(&pwm_led_status1_s);
    pwm_init(&pwm_led_status2_s);
}
