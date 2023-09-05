
/* Standard C Libraries */
#include <stdlib.h>
/* BIOS Header files */
#include "rtos_tasks.h"
#include "state_machine.h"

/* Undefine to not use async mode */


extern "C"	{
 void task_pwm0_1_2_leds(void* ptask_data);
}
//---------------------------------------------
 void task_pwm0_1_2_leds(void* ptask_data)
{   //settings main parameters
	 TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	 TickType_t xFrequency    = ((TaskData*) ptask_data)->t; //period of task
	 //TickType_t clocktick_start,clocktick_end;//uncomment if you want to meassure times
	 TickType_t len_on_duration=((TaskData*) ptask_data)->c; //duration of task
	 state_machine_wait_for_ADQ_DATA_state();
	 while(1){
		 	   state_machine_led_ka_set_on();   //turn on led
		 	   vTaskDelay(len_on_duration);//waits 10 msecs to ensure other radio operators ends
		 	   state_machine_led_ka_set_off();   //turn off led
		 	   //CHECK IF BAT IS CRITICAL
		 	   if(state_machine_is_shutdown()){break;}//exit process
		 	   //-------------task kP delay
		 	   vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
            }
    while(1){vTaskDelay(TASK_SLEEP_FOREVER);}
}

//
//power policy libraries (non used, just to be saved if you wish to create your own power policy)
//#include <ti/drivers/Power.h> //power policy
//#include <ti/drivers/power/PowerCC26XX.h> //power policy defin es
//#include <PowerCC26x2_tirtos.h>
//PowerCC26XX_standbyPolicy
//extern void PowerCC26XX_standbyPolicy2(void);

//PowerCC26XX_sleepPolicy

