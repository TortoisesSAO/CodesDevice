#include "device_system_information.h"

//system general  libraries
#include "rtos_tasks.h"          //allow the use of an operative system function
#include "state_machine.h"       //allow the use of an state machine
#ifdef RTOS_PRINTU
#include "task_uart_printu.h"    //for printing services
#endif
#ifdef SD_STORAGE_ON
#include "task_spi0_microsd.h"         //defines
#endif
#include <TI_CC13XX_id/device_id_TI_CC13X2.h>// main library_to get device ID
#include <time_AL/time_AL.h>               //to hanlde time in stf"time.h" compatible formats
#include <unistd.h>
#include <stdio.h>//to allow float to conversion
#include <string.h>
#include <time_AL/time_AL.h>               //to hanlde time in stf"time.h" compatible formats
//-----------------------------------------
#include "task_rfsub1ghz_tx_data.h"//.h to interact with send "RF data" Task
//---------for ADC driver acces
#include <drivers_AL/adc_AL.h> //for driver use
#include "ti_drivers_config.h" //for acces to .syscfg information
//power policy libraries (non used, just to be saved if you wish to create your own power policy)
#include <ti/drivers/Power.h> //power policy

//-----------------ADC-----------------------------------------------------
#define ADC_BATTERY_LEVEL_PINOUT   adc_battery_level
#define ADC_CHARGER_CURRENT_PINOUT adc_battery_current
#define ADC_USB_LEVEL_PINOUT       adc_5V_usb

/* Driver configuration */
adc_struct adc_battery_level_s={{{NULL},NULL,ADC_BATTERY_LEVEL_PINOUT},//driver,-->pinout
                                ADC_BATTERY_LEVEL_SCALE, //input_scale
                                0,0, };//0 DEFAULT
#if ((DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO))
adc_struct adc_current_charge_s={{{NULL},NULL,ADC_CHARGER_CURRENT_PINOUT},//driver,-->pinout
                                ADC_CHARGER_CURRENT_SCALE, //input_scale
                                0,0,};//0 DEFAULT
adc_struct adc_usb_level_s=     {{{NULL},NULL,ADC_USB_LEVEL_PINOUT},//driver,-->pinout
                                ADC_USB_LEVEL_SCALE, //input_scale
                                0,0,};//0 DEFAULT
#endif
//If we want to use an array to handle all (not worth it with only to inputs)
#if ((DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO))
adc_struct* adc_array[]={&adc_battery_level_s,&adc_current_charge_s,&adc_usb_level_s};
#else
adc_struct* adc_array[]={&adc_battery_level_s};
#endif
//------------amouns
const uint8_t adc_array_amount=(sizeof(adc_array))/sizeof(adc_struct*) ;//use four leds of real board

//----------------------------------------------------------------------
#if ((DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO))
static uint16_t usb_level, battery_level,battery_charge_current;
#else
static uint16_t battery_level,usb_level;
#endif
static uint8_t battery_level_percent;
static bool battery_level_ok=false;


//declare external function
extern uint8_t bat_level_percent_table_find_value( uint16_t bat_level_mv);//defined in "proyect_device_battery_percent_table.cpp"

void set_initial_time(void){
    time_t rawtime;
    tm* timeinfo;
    tm time_uncromp;
    uint32_t time_cromp;
    char buffer[80];
    time(&rawtime);
    vTaskDelay(SEC_TO(0.5)); //do not change
    localtime_set_date_time(1999,1,2,3,59,59);//set the system date
    vTaskDelay(SEC_TO(0.11)); //do not change for testing
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 80,"\r\nDefault system date is: %Y-%m-%d %X UTC",timeinfo);
   #ifdef RTOS_PRINTU //print result of values
    vTaskDelay(SEC_TO(0.1));
    printuf(buffer);
   #endif
    time_cromp=localtime_uint32();//Gets local time in compressed format
    time_uncromp=convertlocaltime_uint32_to_tm(time_cromp);
   // strftime(buffer, 80,"\r\nDecompresed date is %Y-%m-%d %X UTC",&time_uncromp);
   // printuf(buffer);
    vTaskDelay(SEC_TO(1));
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 80,"\r\nSystem date after 1 second is: %Y-%m-%d %X UTC",timeinfo);
#ifdef RTOS_PRINTU //print result of values
    printuf(buffer);
#endif
 }

/*update system time. Only to call insides this .c/cpp*/

#ifdef RTOS_PRINTU
void print_device_ID(TaskData* ptask_data){
 char ID_MAC[MAC_BUFFER_SIZE];//for MAC storage
 int id_num;
 get_device_id_str(ID_MAC);
 id_num=(int)get_asigned_num_from_id_str(ID_MAC);
 vTaskDelay(SEC_TO(0.05));//WAIT TO ENSURE A PROPER SET UP OF printuf task
 printuf("\r\n%s:Device Type: \"%s\", ID:\"%s\", Device Number assigned: %i (0 means no assigned!)",ptask_data->name,DEVICE_TYPE,ID_MAC,id_num);
}
void print_time(TaskData* ptask_data){
    time_t rawtime;
    tm* timeinfo;
    char buffer[40];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printuf("\r\n%s:",ptask_data->name);
    strftime(buffer, 40,"System date time is %Y-%m-%d %X UTC",timeinfo);
    printuf(buffer);
}
#endif
//----------------------------------------------------------------------
static char str[150];
#ifdef SYSTEM_STORE_BATTERY_LEVEL
static uint8_t store_counter=0;
#ifdef SD_STORAGE_ON
const char file_name_bat[]="battery.csv";//file to create
#endif
static void store_bat_level(uint16_t battery_level,uint32_t time_rtos){
++store_counter;
	if (store_counter>1){
	    str[0]=0;
		sprintf(str, "\r\n %i, %i, [sec] [mV],", (int)TO_SEC(time_rtos),battery_level);//store timeout as parameters
		//for storage service
		#ifdef SD_STORAGE_ON
		microsd_write_file_into_device_folder((unsigned char*)str, 30 , (char*)file_name_bat, sizeof(file_name_bat));
        #endif
		store_counter=0;
	}
}
#endif

/*Check battery level and make IMPORTANT decitions, like, turn off the system*/
static void check_bat_level_and_do(uint16_t battery_level){
	 if(battery_level<BATTERY_LEVEL_LOW){
			if (semaphore_trytotake(&sem_state_machine)==true){
				 state_machine.raiseBattery_level_low();
			     semaphore_release(&sem_state_machine);//release the semaphore after implementing changes
			     }
			state_machine_leds_set();//update led val
			str[0]=0;
			sprintf(str, "\r\n SYSTEM ENTERING INTO LOW POWER MODE AT V= %i [mv] at %i [sec]. ONLY KA PULSE AND SYSMONITOR TASK ARE ON",battery_level,(int)TO_SEC(xTaskGetTickCount()));//store timeout as parameters
            #ifdef SD_STORAGE_ON
			memory_store_str((void*)str,sizeof(str));//stopre the string into memmory
            #endif
	 }
	 if(battery_level<BATTERY_LEVEL_CRITICAL){
			if (semaphore_trytotake(&sem_state_machine)==true){
				 state_machine.raiseBattery_level_critical();
			     semaphore_release(&sem_state_machine);//release the semaphore after implementing changes
			     }
			state_machine_leds_set();//update led val
			str[0]=0;
		    sprintf(str, "\r\n SYSTEM SHUTTING DOWN AT V= %i [mv] at %i [sec].                                  ",battery_level,(int)TO_SEC(xTaskGetTickCount()));//store timeout as parameters
			//tell the user that a power down mode has been triggered
            #ifdef SD_STORAGE_ON
		    memory_store_str((void*)str,sizeof(str));//store the string
            #endif
	 }
}

void adc0_read_all_channels(void){
    uint8_t i;//Start to iterate over all the adc drivers
    if (semaphore_trytotake(&sem_adc0)==true){
      for (i=0; i < adc_array_amount; i++) {
        adc_start(adc_array[i]);
        adc_get(adc_array[i]);
        adc_to_mv(adc_array[i]);
        adc_stop(adc_array[i]);
        }
      semaphore_release(&sem_adc0);
    }
}
/*set up necesary means to start data adquisition*/
static inline void start_data_acquisition(void){
    adc_init_array(adc_array, adc_array_amount);
    ///---------------------corection of scale
    adc_battery_level_s.input_scale=(ADC_BATTERY_LEVEL_SCALE*1.02266);
}
/*adquires X samples of data*/
static inline void acquire_data(void){
    adc0_read_all_channels();
    battery_level=adc_battery_level_s.result_mV;
    battery_level_percent=bat_level_percent_table_find_value(adc_battery_level_s.result_mV);
#if ((DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO))
    battery_charge_current=adc_current_charge_s.result_mV;
    usb_level=adc_usb_level_s.result_mV;//=1000;//adc_usb_level_s.result_mV;
#endif
        #ifdef TEST_UART_DISPLAY
         for (i=0; i < adc_array_amount; i++) {
         printuf("ADC CHANNEL[%d] result: %4d [bits] or %4d [mV]\n",  i,adc_array[i]->result_bits,adc_array[i]->result_mV);
         }
         #endif
}/*store data if buffer is above limit*/
static inline void store_data(void){
    //store bat level
   #ifdef SYSTEM_STORE_BATTERY_LEVEL
   store_bat_level(battery_level,xTaskGetTickCount());
  #endif
}
/*stop and turn off any resource used to adquite this kind of data*/
static inline void stop_data_acquisition(void){
  /*  Beacuse
    if (semaphore_trytotake(&sem_adc0)==true){
        for (i=0; i < adc_array_amount; i++) {adc_stop(adc_array[0]);}
                semaphore_release(&sem_adc0);}*/
    ;;
}
static inline void transtition_to_sdcheck_state(void){
    if (battery_level_percent>BATTERY_LEVEL_LOW_PERCENT){
       battery_level_ok=true;
       #ifdef SD_STORAGE_ON
       microsd_set_battery_level(battery_level,battery_level_percent);// pass to micro SD card battery and battery_level_percent value
       #endif
    }
     #ifdef STATE_MACHINE_USB_CHECK_DISABLED
     usb_level=0;
     #endif
     #ifdef STATE_MACHINE_BAT_CHECK_DISABLED
     battery_level_ok=true;
     #endif
    //updates the state machine accordingly
           if (semaphore_trytotake(&sem_state_machine)==true){
            state_machine.enter();
            set_pwm_state_machine();
            semaphore_release(&sem_state_machine);
           }
            if ((usb_level<USB_LEVEL_CONNECTED)or((usb_level>1.3*USB_LEVEL_CONNECTED))){ //USB non connected
                if (semaphore_trytotake(&sem_state_machine)==true){
                  state_machine.raiseUSB5V_NONconnected();
                   if (battery_level_ok){state_machine.raiseBattery_level_ok();} //bat ok, state machine follows next state
                   else                 {state_machine.raiseBattery_level_low();}
                   semaphore_release(&sem_state_machine);
                  }
                }
            else{
                if (semaphore_trytotake(&sem_state_machine)==true){
                  state_machine.raiseUSB5V_connected();
                  semaphore_release(&sem_state_machine);}
                  state_machine_leds_set();
                  while(1){
                          adc0_read_all_channels();
                         #if ((DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO))
                          if ((adc_current_charge_s.result_mV<CHARGE_CURRENT_charged)and(adc_battery_level_s.result_mV>BATTERY_LEVEL_FULL_charged)){break;}
                            #endif
                          vTaskDelay(SEC_TO(10));
                      }
                  if (semaphore_trytotake(&sem_state_machine)==true){
                  state_machine.raiseBattery_charge_completed();
                  semaphore_release(&sem_state_machine);
                  }
                 state_machine_leds_set();
                 vTaskDelay(TASK_SLEEP_FOREVER);  //user must reset the device to adquire agaim
                 }

state_machine_leds_set();//call outside "semaphore_trytotake" statement, this function calls the state machine sem for you ;)
}
/*this functions checks micro sd unit and move to next state*/
static inline void transtition_to_waittogpsfix_state(void){
    int try_to_open_sd_result;
//-----------------------------------------------------------
#if (!(defined(SD_STORAGE_ON)))
    try_to_open_sd_result=0;//bypass engine
#else
    try_to_open_sd_result=memory_check_sd_connected_ok();//run this first to improve speed
#endif
         if (semaphore_trytotake(&sem_state_machine)==true){
             if (state_machine.isStateActive(state_machine.SM_SD_CHECK)){
                 if (try_to_open_sd_result==0){state_machine.raiseSd_check_ok();   }//0: opening succesfull
                 else{                         state_machine.raiseSd_check_NOTok();}
                 semaphore_release(&sem_state_machine);
             }
         }
   state_machine_leds_set();//set the outside "semaphore_trytotake(&sem_state_machine)"
}
//----------------------------------------------------------
bool gps_first_adquisition_and_set_datetime(void); //define here to allow call
/*----------generate a first fix to allow a transition to gps state*/
static inline void transtition_to_adq_state(void){
#if !(defined(DEVELOPMENT_BOARD_EMPTY)or defined(STATE_MACHINE_GPS_DISABLED)) //force ADQ state
  //------out of while loop, generate state transition
    bool gps_fix_in_interval=gps_first_adquisition_and_set_datetime();
     if (semaphore_trytotake(&sem_state_machine)){
         if (gps_fix_in_interval==true){state_machine.raiseGps_fixed_ok();}
         else                   {state_machine.raiseGps_fixed_timeout();}//timeout reached
        semaphore_release(&sem_state_machine);
         }//last statement
   state_machine_leds_set();//updates leds

#else /*otherwise, bypass the engine*///Semaphore_pend(handle,timeout);
if (semaphore_trytotake(&sem_state_machine)==true){
//if (Semaphore_pend(sem_state_machine.driver.handle,sem_state_machine.time_out)){
   state_machine.raiseGps_fixed_ok();
   semaphore_release(&sem_state_machine);}
state_machine_leds_set();//set the outside "semaphore_trytotake(&sem_state_machine)"
#endif

}
//---------------------------
//*--------------------------------
//-------------------------------------
extern "C"	{
void task_adc0_sysmonitor(void* ptask_data);
}
void task_adc0_sysmonitor(void* ptask_data)
{//settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = ((TaskData*) ptask_data)->t;
    //--------------------------------init first system
    #ifdef RTOS_PRINTU
	print_device_ID((TaskData*) ptask_data);
	printuf("\r\nTask led:After %i seconds of reaching \"ADQ_DATA\" state, only one led will reamin blinking during %.3f seconds every %.1f secs.",
         LED_PERIOD_ADQ_START_TIME,(float)LED_TIMEON_ADQ , (float)LED_PERIOD_ADQ  );
    #endif
	set_initial_time();
    /* Call driver init functions */
	start_data_acquisition();
    /*adquire samples form all chyannels*/
    acquire_data();//after getting the sampples, print the valued
    //update RF struct
    #if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD)
    update_tx_packet_battery_level(bat_level_percent_table_find_value(adc_battery_level_s.result_mV));
    #endif
    #ifdef RTOS_PRINTU //print result of values
    vTaskDelay(SEC_TO(0.1));
    printuf("\r\n%s:Battery level:%i mV, which is %i percent",((TaskData*)ptask_data)->name,(int)battery_level,(int)battery_level_percent);
    #endif
/*  system to monitor state transition inside the device*/
    transtition_to_sdcheck_state(); //atempts to transitate to sd check state. Will also monitor the device charge if bat is low
    //------------wait for adq state-------------------------------------------------------------------------
    state_machine_wait_for_state(Statechart::SM_SD_CHECK); ////wait for start acq
   //------------wait for adq state-------------------------------------------------------------------------
    transtition_to_waittogpsfix_state();
   //----------------------------------------------------------------------------------------------------
    state_machine_wait_for_state(Statechart::SM_GPS_WAIT_POS_FIX);////wait for start acq

    transtition_to_adq_state();

    //----------write into SD after getting gps fix
    #ifdef SD_STORAGE_ON
    memory_create_first_file();
    #endif
   //------------END OF TRANSITION, SM_ADQ_DATA REACHED -------------------------------------------------------------------------
    state_machine_wait_for_ADQ_DATA_state();
   //------------starting while loop-------------------------------------------------------------------------
    while(1)  {
               #ifdef RTOS_PRINTU //print result of values
                //print_time((TaskData*)ptask_data);
                #endif
                 //adquire all channels
                acquire_data();
                //store data
                store_data();
                //update RF struct
                #if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD)
                update_tx_packet_battery_level(bat_level_percent_table_find_value(adc_battery_level_s.result_mV));
                #endif
		        //check if system turn down is requested
		        check_bat_level_and_do(battery_level);
		        //CHECK IF BAT IS CRITICAL
                if(state_machine_is_shutdown()){break;}//exit process
              //------------------delay
               vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
    }
 //end of mine while
    stop_data_acquisition();
    while(1){vTaskDelay(TASK_SLEEP_FOREVER);}
}
//power policy libraries (non used, just to be saved if you wish to create your own power policy)
//#include <ti/drivers/Power.h> //power policy
//#include <ti/drivers/power/PowerCC26XX.h> //power policy defin es
//#include <PowerCC26x2_tirtos.h>
//PowerCC26XX_standbyPolicy
//extern void PowerCC26XX_standbyPolicy2(void);
//PowerCC26XX_sleepPolicy
