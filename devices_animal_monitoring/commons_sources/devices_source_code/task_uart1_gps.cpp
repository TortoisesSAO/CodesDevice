#include "device_system_information.h"
#if !defined(STATE_MACHINE_GPS_DISABLED)
#include "task_uart1_gps.h"
#include <gps_ublox/gps_ublox.h>
#include <gps_parser/TinyGPS++.h>

/*SD storing*/
//-----------------------------------
#include  <memory_structures/memory_struct_gpsneoxm.h>
//-----------------------------------
#include <drivers_AL/gpio_AL.h>//driver GPIO
//----------------------------------
#include <time_AL/time_AL.h>//time and date managment in standard time.c
//----------------------------------
#include "rtos_tasks.h"
#include "state_machine.h"      //defines
//for printing services
#ifdef RTOS_PRINTU
#include "task_uart_printu.h"
#endif
//for using time
#include "time_AL/time_AL.h"
//for using this:
#ifdef GPS_ADQUIRE_IF_ANIMAL_NOT_STILL
#include "task_animal_activity.h"    //for IMU structures
static animal_activity_struct animal_activity;
#endif
#include "task_spi0_microsd.h"      //defines
#include "task_rfsub1ghz_tx_data.h"

#include <unistd.h>//for uint def
#include <string.h>//for memcpy ussage
#include <stdio.h>//to allow float to conversion and sprintf
//declare of structures
// The TinyGPSPlus object
static TinyGPSPlus gps_parser;
//gps ublox
static GPS_ublox gps;
static sempaphore_struct sem_gps_uart1_callback;
//gpio strucs
uart_struct uart1_gps;
gpio_struct gpio_gps;

static bool keep_gps_enabled;
static bool gps_acq_successful;

static TickType_t task_start_time,task_elapsed_time;
static 	TickType_t gps_timeout=SEC_TO(UART1_GPS_RECEPTION_TIMEOUT);//time out of task
//strutcs to sotre the mesagges
static char gps_last_message[_GPS_MESSAGE_FIELD_SIZE]; //D0 NOT CHANGE SIZE, MUST BE _GPS_MESSAGE_FIELD_SIZE
 char gps_date_text[9]="NOGPSON";//for storing date in string format. Used to create main folder by. asign a value for test
#ifdef PRINT_UART
const char echoPrompt[] = "\r\n Starting GPS Task:\r\n";
#endif
#ifdef GPS_TEST_NO_GPS
uart_struct uart1_gps_print;
#endif
/*
 *  ======== callbackFxn ========
 */
void callback_uart1_gps(UART2_Handle handle, void *buffer, size_t count, void *userArg, int_fast16_t status)
{  //This function is called each time uart_read is called and count (bytes in RX uarty buffer) is greater than 0
	//until the max amounts of the buffer fills
	//The configured uart trigger count is independent of the size of the internal uart MCU buffer
	//Recieving process is purely NON-BLOCKING PROCESS (meaning that you can do task while reception is happening)
	//When receptions ends, the callback is called and the parameter "counts " store the bytes recieved
	//------------------------------------------------------------------------------
    //What happen if you have a 3 byte buffer and recieve 4 bytes?
	//Tested Answer: the 4 fourth byte is discarded. No callback happens with 4 fourth byte
	//tested: Count=amounts of bytes recieved since last time  "uart_read" was called
#ifdef GPS_TEST_NO_GPS
	//for testing pourpose, copy 10 characters to the uart buffer
	count=10;
    memcpy(buffer, &(gpsStream[gpsStream_index]), count);//add the recieved bytes to the buffer
	gpsStream_index=gpsStream_index+count;
#endif
    gps.uart_gps_bytes_read =count+gps.uart_gps_bytes_read; //add the recieved bytes to the total amounts of bytes recieved
    //this functions asumes that your uart buffer is lower than your gps reception buffer
    if (gps.gps_bytes_acum+gps.uart_gps_bytes_read>UART_GPS_BUFFER_MAX) //overflow prottection
        {gps.gps_bytes_acum=UART_GPS_BUFFER_MAX-count;}//only fills the buffer, avoiding overflow
    //add the new data to the gps-to-parse buffer accumulator
    memcpy(&(gps.gps_buffer_acum[gps.gps_bytes_acum]), buffer, count);//add the recieved bytes to the buffer
    gps.gps_bytes_acum=gps.gps_bytes_acum+gps.uart_gps_bytes_read;//a read occurred
    //release the semaphore after a succesfull read
    semaphore_release(&sem_gps_uart1_callback);
}
//-----------------------------------------------------------------
//--------/*Function to set up all the hardware & RTOS related elements*/
//-----------------------------------------------------------------
static void task_uart1_gps_setup(void){
    /* Create the  semaphore */
    /*Create uart display*/
    #ifdef PRINT_UART
     printuf("Starting gps :  %.*s", sizeof(echoPrompt),echoPrompt);
      #endif
    /*Create uart*/
    uart_set(&uart1_gps,UART1_GPS_PORT_NUMBER,UART1_GPS_BAUD_RATE,UART1_GPS_MODE,UART1_GPS_CALLBACK);
    uart_init(&uart1_gps);
    uart_close(&uart1_gps);
    /*Create gpio*/
    gpio_set(&gpio_gps,gpio_on_gps,GPIO_GPS_MODE,NULL);
    gpio_init(&gpio_gps);
    //init gpio and pass the pointer to struct
    gps.init(&uart1_gps,&gpio_gps);
    //------------------------------------------------------------
    //for debbugingaa
    //create and init uart
    #ifdef GPS_TEST_NO_GPS
    uart1_gps_print.driver.handle=uart1_gps.driver.handle;
    #endif
}
//-----------------------------------------------------------------------------------
//--------functions for sd carg storing, all must be declared static to avoid issues
//-----------------------------------------------------------------------------------
typedef mem_gpsneo7_union mem_struct;
//---------------------------------
/*static mem_struct buffer_data={.m.head.id_head=MEM_STR_ID_HEAD,
	  .m.head.id_name=GPSNEOXM_STR_ID,.m.head.block_size=GPSNEOXM_BLOCK_UNIT_SIZE,
	  .m.head.data_written=0};*/
static mem_gpsneo7_union buffer_data={
      {//.m
        {MEM_STR_ID_HEAD,GPSNEOXM_STR_ID,GPSNEOXM_BLOCK_UNIT_SIZE,0,0,0}, //head
        {0},                                                              //conf
        {{0}}                                                               //data
      }
      };
//----------------------------------------------
static void set_store_struct(mem_struct* pmem)
{ int i;
pmem->m.head.id_head[0]='\r';
pmem->m.head.id_head[1]='\n';
pmem->m.head.block_size=GPSNEOXM_BLOCK_UNIT_SIZE;
for (i=0; i < GPSNEOXM_DATA_ARRAY_SIZE; i++) {
    pmem->m.d[i].line_break='\n';
    pmem->m.d[i].separator='|';}
//-------------------------------------
pmem->m.conf.user_timeout=TASKP_GPS_TIME;
pmem->m.conf.user_sample_rate=TASKP_GPS_PERIOD;
pmem->m.conf.min_presition=gps.nav_accuracy_m;
//----------------------------------------
pmem->m.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
}
static void reset_store_struct(mem_struct* pmem)
{int j,i;
pmem->m.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
for (i=0; i < GPSNEOXM_DATA_ARRAY_SIZE; i++) {
	for (j=0; j < _GPS_MESSAGE_FIELD_SIZE; j++){
	    pmem->m.d[i].message[j]=' ';
	    pmem->m.d[i].time_to_fix=SEC_TO(998);//998 seconds to fix asdefault message. This means never fix.
	  }
	sprintf(pmem->m.d[i].message_to_user, "%i", UART1_GPS_RECEPTION_TIMEOUT);//store timeout as parameters
    }
}
//-----------------------------------------------------------------
//--------/*Function to store*/
//-----------------------------------------------------------------
GPS_message_type GPGSA_count={.type=GPS_SENTENCE_GPGSA,.n_max=GPS_GPGSA_MSG_MAX,.n=0};
GPS_message_type GPRMC_count={.type=GPS_SENTENCE_GPRMC,.n_max=GPS_GPRMC_MSG_MAX,.n=0};
//--------------message types counter
GPS_message_type* pGPS_messages[]={&GPGSA_count,&GPRMC_count};

int gps_messages_types_amount=sizeof(pGPS_messages)/sizeof(GPS_message_type*);
bool gps_memory_store(TinyGPSPlus* gps_parser,uint32_t timefix){
char c[10];
int i,id;
bool is_all_stored=true;
 //classify the last recieved message and perform an store in an internal buffer
 for (i=0; i < gps_messages_types_amount; i++) { //task_data_p_n
	 //check is each class of message must be stores
	 if (((pGPS_messages[i])->n)<((pGPS_messages[i])->n_max)){//(pGPS_messages[i])->n_max)
		//buffer is not full, check if the message is the proper to store
	    if (gps_parser->get_last_message_ok_type()==(pGPS_messages[i])->type){
	       //store message, store timesstamp
	    	//strncpy(dest_char_p, complete_message_ok,_GPS_MESSAGE_FIELD_SIZE);
	    	id=buffer_data.m.head.data_written;                        //store the struc location
	    	gps_parser->get_last_message_ok((buffer_data.m.d[id].message));                //copy the last message
	    	buffer_data.m.d[id].message_type=pGPS_messages[i]->type;//copy the message type
	    	buffer_data.m.d[id].message_number=pGPS_messages[i]->n; //copy the message type number
	    	buffer_data.m.d[id].time_to_fix=timefix;//copy the message type
			buffer_data.m.head.data_written=1+buffer_data.m.head.data_written;
	    	//generate message to user
	    	buffer_data.m.d[id].message_to_user[0]=0;
	    	strncat(buffer_data.m.d[id].message_to_user,";",1);
	    	sprintf(c, "%i", (int)TO_SEC(buffer_data.m.d[id].time_to_fix));
	    	strncat(buffer_data.m.d[id].message_to_user,c,5); //5: do not change
	    	strncat((buffer_data.m.d[id].message_to_user),";",1);
	    	c[0]=buffer_data.m.d[id].message_number+'0';
	    	strncat((buffer_data.m.d[id].message_to_user),c,1);
	    	strncat((buffer_data.m.d[id].message_to_user),";",1);//updates counter
	       pGPS_messages[i]->n=1+(pGPS_messages[i])->n ;
	     }
	  }
	 //check if all this messages has been stored
      if ( ((pGPS_messages[i])->n)<((pGPS_messages[i])->n_max) ){
    	  is_all_stored=false;
        }
   }
 if (is_all_stored==true){//if finished to store all the messsages, reset the counters
     for (i=0; i < gps_messages_types_amount; i++) {pGPS_messages[i]->n=0 ;}
   }
 return is_all_stored;//return if all messages required had been stored
}

//-----------------------------------------------------------------
//------------for displaying options
//-----------------------------------------------------------------
#ifdef PRINT_UART
 void uart1_gps_print(GPS_ublox* gps)
{        //callbackhappened
           printuf("%i char arrived to uart, New message:  %.*s",
           gps->uart_gps_bytes_read,gps->gps_bytes_acum ,gps->gps_buffer_acum);//pass data to gps processsor before starting recept

           printuf("\n\r New valid message arrived",30);
         // if (gps_parser.get_last_message_ok_type()== GPS_SENTENCE_GPGGA){Display_printf(display, 0, 0, "  of type GPGGA");}
          //if (gps_parser.get_last_message_ok_type()== GPS_SENTENCE_GPGSA){Display_printf(display, 0, 0, "!!!!!!  of type GPGSA");}
           printuf("Original   message:%.*s",sizeof(gps_last_message),gps_last_message);
      if (gps_parser.dop.isUpdated()){
          printuf("\n Pdop:%2f Hdop:%2f Vdop:%2f",gps_parser.dop.pdop(),gps_parser.dop.hdop(),gps_parser.dop.vdop());
      }
      if (gps_parser.location.isUpdated())
        {gps_parser.location.lat(); //after reading the data, "isUpdated" becomes false
        printuf((char *)"\r\n New Loc lat: %6f, long: %6f\n",
           		gps_parser.location.lat(),gps_parser.location.lng());
 	    //Display_printf(display, 0, 0,(char *)"\r\n New Loc lat: %i,gps.getcommas() )//print the amount of commas of rx message
        }
      //display information
       if (gps_parser.date.isUpdated())
        {gps_parser.date.day();//after reading the data, "isUpdated" becomes false
        gps_parser.date.date_str_inverted(gps_date_text);
        printuf((char *)"\r\n New date, day: %i, month: %i ,year: %i \n",
          		        gps_parser.date.day(),gps_parser.date.month(),gps_parser.date.year());
        }
       if (gps_parser.time.isUpdated())
        {gps_parser.time.hour();//after reading the data, "isUpdated" becomes false
        printuf((char *)"New time, hour: %i, min: %i ,sec: %i \n",
         		gps_parser.time.hour(),gps_parser.time.minute(),gps_parser.time.second());
        }
	}
#endif
void gps_start (GPS_ublox*gps, uint8_t nav_accuracy){
gps->turn_on();//turns on the gps
vTaskDelay(SEC_TO(0.5));     //1 second sleep to wait for the gps to properly awaken
gps->configure(nav_accuracy);//configure the GPS to recive only the desired frames
vTaskDelay(SEC_TO(1));       //1 second to ensure nom requested frames are not parsed
gps->recieve();//enables gps uart-callbkack reaaception};
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//------------Task thread---------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

#ifdef GPS_SAVEALL_NOPARSE
void gps_adquire_and_save(uint32_t gps_timeout ){
    uint32_t time_start=xTaskGetTickCount();
    gps.turn_on();//turns on the gps
    vTaskDelay(SEC_TO(0.5));     //1 second sleep to wait for the gps to properly awaken
    gps.configure_rx_all();//configure the GPS to recive only the desired frames
    vTaskDelay(SEC_TO(1));       //1 second to ensure nom requested frames are not parsed
    gps.recieve();//enables gps uart-callbkack reaaception};
    while ( (xTaskGetTickCount()-time_start)<gps_timeout){
        if (semaphore_trytotake(&sem_gps_uart1_callback)==true) {
           /* CALLBACK OR SEMPAHORE TIMEOUT HAPPENED */
            if (gps.uart_gps_bytes_read > 0){
                 /*SUCCESFULL UART CALLBACK! (new bytes had been recieved)*/
                 #ifdef PRINT_UART
                 uart1_gps_print(&gps);
                 #endif
                 //store data into SD
                 memory_store_str(gps.gps_buffer_acum,gps.gps_bytes_acum);
                 //reset acum buffer
                 gps.reset_acum_buffer();//reset acum bufffer to indicate parsed character
                 //enable reception again if gps still needs more data
            }
         }
        gps.recieve();
      }
    gps.turn_off();

}
#endif
bool gps_adquisition(void ){
  if  (semaphore_trytotake(&sem_uart1)){
        buffer_data.m.head.acq_time_start=xTaskGetTickCount();    //time the task started
        reset_store_struct(&buffer_data);
    	task_start_time=xTaskGetTickCount();//store the tastk time
    	keep_gps_enabled=true;//enable gps logging. will be set to false if adquisition fails before timeout
        gps_acq_successful=false;//If all messages were gatered, set this flag to true
    	gps_start(&gps,GPS_MAX_RESOLUTION_ERROR);
    	gps_parser.parse_reset();//resets the parser mechanism before adquiring a new data
         #ifdef PRINT_UART
          Display_printf(display, 0, 0, "awaiting rececption");
         #endif
        /*Recieves messages until timeout expires or all messages have been recieved*/
        while(keep_gps_enabled){//keep_gps_enabled IS TRUE
          if (semaphore_trytotake(&sem_gps_uart1_callback)==true) {
        	 /* CALLBACK OR SEMPAHORE TIMEOUT HAPPENED */
        	  if (gps.uart_gps_bytes_read > 0){
        	  /*SUCCESFULL UART CALLBACK! (new bytes had been recieved)*/
				  #ifdef PRINT_UART
        		   uart1_gps_print(&gps);
				  #endif
                 gps_parser.parse_chararray(gps.gps_buffer_acum,gps.gps_bytes_acum); //process the data
                 gps.reset_acum_buffer();//reset acum bufffer to indicate parsed character
        	     }
        	   //check if new valid&desired message was recievd since last succesfull parsing
               if (gps_parser.isUpdated()){//new valid message arrived
            	  gps_parser.get_last_message_ok(gps_last_message);//copy the mesage to the buffer
            	  //process this message and store it. If all needed messages were collected, shutdown gps and finish gps acq
                  task_elapsed_time=xTaskGetTickCount()-task_start_time;//init the timeout
                  gps_acq_successful=gps_memory_store(&gps_parser,task_elapsed_time);
            	  if (gps_acq_successful){keep_gps_enabled=false;}
               }
              //enable reception again if gps still needs more data
              if(keep_gps_enabled) {gps.recieve();}//enable the uart callback to recieve data again
           }/*Else for "semaphore_trytotake" */
          else{/*timeout for reception ocurred*/
                   #ifdef PRINT_UART
	                Display_printf(display, 0, 0,(char *)"Message Timeout reached, reseting parser");
			      #endif
	            gps_parser.parse_reset();
              }
          task_elapsed_time=xTaskGetTickCount()-task_start_time;//init the timeout
      	  if (task_elapsed_time>gps_timeout){//if timeout happened
      	      keep_gps_enabled=false;} //disable the gps
      }//--------------------------------------------------------------------
        //------------------END of while----------------------------------------
        //store the end of adquisition
         buffer_data.m.head.acq_time_end=xTaskGetTickCount();//time the task ended
	  	 //at this point, you can perform a write of the gps into the buffer
         gps.turn_off();//turn off the gps because either a timeout happened or the system collacted all the requested frames
        #ifdef PRINT_UART
         Display_printf(display, 0, 0,(char *)"GPS turned off after %4f seconds,sleeping task until programmed awake...zzZ",TO_SEC(task_elapsed_time));
        #endif
  //-----end of acq. Release the resource
  semaphore_release(&sem_uart1);
  }

return gps_acq_successful;
}


static bool gps_last_acq;
static bool gps_was_updated;

//-----------------------------------------------------------------
//----------------------------------------------------------------


/*calls a function to pass data to RF engine*/
static void pass_data_to_rf_module(void){
    gps_was_updated=gps_parser.location.isUpdated();//check if position was updated since last read
    tm Time;
    if(gps_was_updated){
        Time=date_time_to_tm(gps_parser.date.year(),gps_parser.date.month(),gps_parser.date.day(),
                             gps_parser.time.hour(),gps_parser.time.minute(),gps_parser.time.second());
        update_tx_packet_gps((float) gps_parser.location.lat(),(float)gps_parser.location.lng(), (float)gps_parser.dop.hdop(),Time);
        }
}
/*set up necesary means to start data adquisition*/
static inline void start_data_acquisition(void* pgps_setup){
  //set up shared semaphore
    semaphore_init(&sem_gps_uart1_callback,SEC_TO(UART1_GPS_PARSER_TIMEOUT));
    //set up all the pheriphericals needed
    task_uart1_gps_setup();
    set_store_struct(&buffer_data);
    //after all is properly set up, is time to start the gps
}

/*adquires X samples of data*/
static inline void acquire_data(TickType_t xFrequency){
              #ifdef GPS_ADQUIRE_IF_ANIMAL_NOT_STILL
               animal_activity=get_animal_activity();
               //if the state of the animal
               if ( !( (animal_activity.state==ANIMAL_STILL)&(animal_activity.state_elapsed_time>xFrequency) ) ){
                   gps_adquisition();} //acq all the posible gps data before seted time out
               #else
               gps_adquisition();
               #endif
}
/*store data if buffer is above limit*/
static inline void store_data(void){
    memory_store(&buffer_data);  //store the first gps data after
}
/*stop and turn off any resource used to adquite this kind of data*/
static inline void stop_data_acquisition(void){
    //no actions are needed for GPS app because the system shuts down their requested drivers after each adquisition
    ;;
}

/*-------performs first gps adquisition and set time--------*/
extern bool gps_first_adquisition_and_set_datetime(void ){
      bool ret;
      uint8_t seconds_fix;//offset of time between gps Date&time was adquired and then seted
      //--------performs first adquisition
      if (!state_machine_is_acquisition()){//added for testing pourpose. This allow to bypass the gps engine
        while(1){
                gps_last_acq=gps_adquisition();//performs an asquision until timeout
                if (gps_last_acq){//gps_acquistion successful
                                 //set time using last acq
                                 seconds_fix=TO_SEC((xTaskGetTickCount()-buffer_data.m.head.acq_time_start)-buffer_data.m.d[2*GPS_GPS_NUMBER_OF_MESSAGES-1].time_to_fix);//
                                 //commented for debugprintuf("seconds to compensate: %i",seconds_fix);
                                 localtime_set_date_time(gps_parser.date.year(),gps_parser.date.month(),gps_parser.date.day(),
                                                          gps_parser.time.hour(),gps_parser.time.minute(),gps_parser.time.second()+seconds_fix);//got at time buffer_data.m.head.acq_time_end)
                                 break;}
                if( (xTaskGetTickCount()-gps_last_acq)>SEC_TO(UART1_GPS_FIRST_RECEPTION_TIMEOUT) ){//check for timeout
                                 gps_last_acq=false; //time out reached, seting adquisition result as false and leave
                                 break;}
                }
        }//end of if statement
    ret=gps_last_acq;
 return(ret);
}

//-----------------------------------------------------------------
//----------------------------------------------------------------
extern "C" {
 void  task_uart1_gps(void* ptask_data);
}
void task_uart1_gps(void* ptask_data){
    //settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = ((TaskData*) ptask_data)->t;
    //--------------systems checks ok: set up the gps adquisition and wait for first fix
#ifdef RTOS_PRINTU
    //print task configuration
vTaskDelay(SEC_TO(0.2));//WAIT TO ENSURE A PROPER SET UP OF printuf task
printuf("\r\n%s: Max Timeout for messages adcquisition is %.1f secs with a period of %i secs.",((TaskData*) ptask_data)->name, (float)GPS_ACQ_MAX_TIME,GPS_ACQ_PERIOD);
printuf(" Stores %i messages with an accuracy (geolocalization error) of %i meters.",GPS_GPS_NUMBER_OF_MESSAGES, GPS_MAX_RESOLUTION_ERROR);
#endif
	start_data_acquisition(ptask_data);//
   //---------------------------------------------------------------------------------
    state_machine_wait_for_state(state_machine.SM_ADQ_DATA);//awaits for the system to reach min state
    //-------------testing function for adquiring only gps data for a time amount
    #ifdef GPS_SAVEALL_NOPARSE     //this must go before the transition of states to allow the system to only adquire gps data
      gps_adquire_and_save(SEC_TO(GPS_SAVEALL_NOPARSE));
    #endif
    // shared data with rf engine
    pass_data_to_rf_module();
	//---------------
	//acq has started and SD was set properly set up->>> store data
	store_data(); //store the first gps data after

    #ifndef STATE_MACHINE_GPS_DISABLED
	//wait for next acq.
	vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS scheduller //vTaskDelay(xFrequency);
	//now begin periodic adquisition
    while(1){
              acquire_data(xFrequency);
    	      //performs a write besides the result of adquisition
    	      memory_store(&buffer_data);//store acq gps data
    	      //----------update rf packet------------
    	      pass_data_to_rf_module();
              //---------------------------
    	      if(state_machine_is_lowpower()){break;}//exit process
    	      //-------------end of task, delay until next one--------------------
             vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS scheduller
      }
    //end of task. If this point is reached, the System monitor task has deleted this task--->store all the data we have
    memory_store(&buffer_data);//store acq gps data
    stop_data_acquisition(); //no need to call this
#endif
while(1){vTaskDelay(TASK_SLEEP_FOREVER);}
}
#endif
