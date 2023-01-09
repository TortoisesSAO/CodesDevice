
/*
 *  ======== i2copt3001_cpp.cpp ========
 */

#include <gps_ublox/gps_ublox.h>
#include <gps_parser/TinyGPS++.h>
#include <task_uart0_gps_DEFINES.h>
/*SD storing*/
//-----------------------------------
#include  <memory_structures/memory_struct_gpsneoxm.h>
//-----------------------------------
#include <drivers_AL/gpio_AL.h>//driver GPIO
#include <RTOS_AL/semaphore_AL.h>
#include <RTOS_AL/rtos_AL.h>
//----------------------------------
#include <unistd.h>//for uint def
#include <string.h>//for memcpy ussage
#include <stdio.h>//to allow float to conversion
//declare of structures
// The TinyGPSPlus object
static TinyGPSPlus gps_parser;
//gps ublox
static GPS_ublox gps;
static sempaphore_struct sem_gps_uart0_callback;
//gpio strucs
uart_struct uart0_gps;
gpio_struct gpio_gps;
//strutcs to sotre the mesagges
static char gps_last_message[_GPS_MESSAGE_FIELD_SIZE]; //D0 NOT CHANGE SIZE, MUST BE _GPS_MESSAGE_FIELD_SIZE
static char gps_date_text[8];//for storing date in string format. Used to create main folder by
#ifdef PRINT_UART
const char echoPrompt[] = "\r\n Starting GPS Task:\r\n";
#endif
#ifdef GPS_TEST_NO_GPS
uart_struct uart1_gps_print;
#endif
static void foocpy (unsigned char* dst, unsigned char* src, uint16_t n)
{uint16_t i=0;
  for( i=0; i<n; i++){*(dst+i) = *(src+i);}
}
/*
 *  ======== callbackFxn ========
 */
void callback_uart0_gps(UART2_Handle handle, void *buffer, size_t count, void *userArg, int_fast16_t status)
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
    semaphore_release(&sem_gps_uart0_callback);
}
//-----------------------------------------------------------------
//--------/*Function to set up all the hardware & RTOS related elements*/
//-----------------------------------------------------------------
void task_uart0_gps_setup(void){
    /* Create the  semaphore */
    /*Create uart display*/
    #ifdef PRINT_UART
     display = Display_open(Display_Type_UART, NULL);//open display driver]
      if (display == NULL) {while (1);}        /* Failed to open display driver */
     Display_printf(display, 0, 0, "Starting gps :  %.*s", sizeof(echoPrompt),echoPrompt);
      #endif
    /*Create uart*/
    uart_set(&uart0_gps,UART0_GPS_PORT_NUMBER,UART0_GPS_BAUD_RATE,UART0_GPS_MODE,UART0_GPS_CALLBACK);
    uart_init(&uart0_gps);//
    /*Create gpio*/
    gpio_set(&gpio_gps,gpio_on_gps,GPIO_OUTPUT_PULLUP_LOWDRIVE,NULL);
    gpio_init(&gpio_gps);
    //init gpio and pass the pointer to struct
    gps.init(&uart0_gps,&gpio_gps);
    //------------------------------------------------------------
    //for debbugingaa
    //create and init uart
    #ifdef GPS_TEST_NO_GPS
    uart1_gps_print.driver.handle=uart0_gps.driver.handle;
    #endif
}
//-----------------------------------------------------------------------------------
//--------functions for sd carg storing, all must be declared static to avoid issues
//-----------------------------------------------------------------------------------
extern TaskData* task_data_p[];
extern sempaphore_struct sem_spi0_microsd;//semaphore for sharing the i2c bus acces
extern unsigned char microsd_write_buffer[SD_BLOCK_UNIT_SIZE];
static mem_gpsneo7_union gpsneoxm_buffer{.mem.head.id_head=MEM_STR_ID_HEAD,
	  .mem.head.id_name=GPSNEOXM_STR_ID,.mem.head.block_size=GPSNEOXM_BLOCK_UNIT_SIZE};
static void set_store_struct(mem_gpsneo7_union* mem_struct)
{ int i;
mem_struct->mem.head.id_head[0]='\r';
mem_struct->mem.head.id_head[1]='\n';
//mem_struct->mem.head.id_name=GPSNEOXM_STR_ID;
mem_struct->mem.head.block_size=GPSNEOXM_BLOCK_UNIT_SIZE;
for (i=0; i < GPSNEOXM_DATA_ARRAY_SIZE; i++) {
	mem_struct->mem.data[i].line_break='\n';
	mem_struct->mem.data[i].separator='|';
}
mem_struct->mem.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
}
static void reset_store_struct(mem_gpsneo7_union* mem_struct)
{int j,i;
mem_struct->mem.head.data_written=0;//initiliazeh amount of written data arrays (this varies dependding on which is enabled)
for (i=0; i < GPSNEOXM_DATA_ARRAY_SIZE; i++) {
	for (j=0; j < _GPS_MESSAGE_FIELD_SIZE; j++){
	   mem_struct->mem.data[i].message[j]=' ';
         }
}
}
//-----------------------------------------------------------------
//--------/*Function to store*/
//-----------------------------------------------------------------

GPS_message_type GPGSA_count={.type=GPS_SENTENCE_GPGSA,.n_max=GPS_GPGSA_MSG_MAX,.n=0};
GPS_message_type GPRMC_count={.type=GPS_SENTENCE_GPRMC,.n_max=GPS_GPRMC_MSG_MAX,.n=0};
//--------------message types counter
GPS_message_type* pGPS_messages[]={&GPGSA_count,&GPRMC_count};

int gps_messages_amount=sizeof(pGPS_messages)/sizeof(GPS_message_type*);
bool gps_memory_store(TinyGPSPlus* gps_parser,uint32_t timefix){
int i,id;
bool is_all_stored=true;
 //classify the last recieved message and perform an store in an internal buffer
 for (i=0; i < gps_messages_amount; i++) { //task_data_p_n
	 //check is each class of message must be stores
	 if (((pGPS_messages[i])->n)<((pGPS_messages[i])->n_max)){//(pGPS_messages[i])->n_max)
		//buffer is not full, check if the message is the proper to store
	    if (gps_parser->get_last_message_ok_type()==(pGPS_messages[i])->type){
	       //store message, store timesstamp
	    	//strncpy(dest_char_p, complete_message_ok,_GPS_MESSAGE_FIELD_SIZE);
	    	id=gpsneoxm_buffer.mem.head.data_written;                        //store the struc location
	    	gps_parser->get_last_message_ok((gpsneoxm_buffer.mem.data[id].message));                //copy the last message
	    	gpsneoxm_buffer.mem.data[id].message_type=pGPS_messages[i]->type;//copy the message type
	    	gpsneoxm_buffer.mem.data[id].message_number=pGPS_messages[i]->n; //copy the message type number
	    	gpsneoxm_buffer.mem.data[id].time_to_get=timefix;//copy the message type
	    	//to store the reception time
	    	/*if (n==0)
	    	else*/
			gpsneoxm_buffer.mem.head.data_written=1+gpsneoxm_buffer.mem.head.data_written;
	       //updates counter
	       pGPS_messages[i]->n=1+(pGPS_messages[i])->n ;
	     }
	  }
	 //check if all this messages has been stored
      if (((pGPS_messages[i])->n)<((pGPS_messages[i])->n_max)){
    	  is_all_stored=false;
        }
   }
 if (is_all_stored==true){//if finished to store all the messsages, reset the counters
     for (i=0; i < gps_messages_amount; i++) {pGPS_messages[i]->n=0 ;}
   }
 return is_all_stored;//return if all messages required had been stored
}

//-----------------------------------------------------------------
//------------for displaying options
//-----------------------------------------------------------------
#ifdef PRINT_UART
 void uart1_gps_print(GPS_ublox* gps)
{        //callbackhappened
           Display_printf(display, 0, 0, "%i char arrived to uart, New message:  %.*s",
           gps->uart_gps_bytes_read,gps->gps_bytes_acum ,gps->gps_buffer_acum);//pass data to gps processsor before starting recept

          Display_printf(display, 0, 0, "\n\r New valid message arrived",30);
         // if (gps_parser.get_last_message_ok_type()== GPS_SENTENCE_GPGGA){Display_printf(display, 0, 0, "  of type GPGGA");}
          //if (gps_parser.get_last_message_ok_type()== GPS_SENTENCE_GPGSA){Display_printf(display, 0, 0, "!!!!!!  of type GPGSA");}
          Display_printf(display, 0, 0, "Original   message:%.*s",sizeof(gps_last_message),gps_last_message);
      if (gps_parser.dop.isUpdated()){
     	 Display_printf(display, 0, 0, "\n Pdop:%2f Hdop:%2f Vdop:%2f",gps_parser.dop.pdop(),gps_parser.dop.hdop(),gps_parser.dop.vdop());
      }
      if (gps_parser.location.isUpdated())
        {gps_parser.location.lat(); //after reading the data, "isUpdated" becomes false
           Display_printf(display, 0, 0,(char *)"\r\n New Loc lat: %6f, long: %6f\n",
           		gps_parser.location.lat(),gps_parser.location.lng());
 	    //Display_printf(display, 0, 0,(char *)"\r\n New Loc lat: %i,gps.getcommas() )//print the amount of commas of rx message
        }
      //display information
       if (gps_parser.date.isUpdated())
        {gps_parser.date.day();//after reading the data, "isUpdated" becomes false
        gps_parser.date.date_str_inverted(gps_date_text);
         Display_printf(display, 0, 0,(char *)"\r\n New date, day: %i, month: %i ,year: %i \n",
          		        gps_parser.date.day(),gps_parser.date.month(),gps_parser.date.year());
         //Display_printf(display, 0, 0,gps_date_text);
        }
       if (gps_parser.time.isUpdated())
        {gps_parser.time.hour();//after reading the data, "isUpdated" becomes false
         Display_printf(display, 0, 0,(char *)"New time, hour: %i, min: %i ,sec: %i \n",
         		gps_parser.time.hour(),gps_parser.time.minute(),gps_parser.time.second());
        }
	}
#endif
void gps_start (GPS_ublox*gps){
gps->turn_on();//turns on the gps
vTaskDelay(SEC_TO(0.5));     //1 second sleep to wait for the gps to properly awaken
gps->configure();//configure the GPS to recive only the desired frames
vTaskDelay(SEC_TO(1));       //1 second to ensure nom requested frames are not parsed
gps->recieve();//enables gps uart-callbkack reaaception};
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//------------Task thread---------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
extern "C" {
 void  task_uart0_gps(uint8_t task_index);
}
void task_uart0_gps(uint8_t task_index)
{ //settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = task_data_p[task_index]->t;;
    //---------------------------------------------------
	bool keep_gps_enabled=true; //to store the status of gps
	//set clock
	TickType_t task_start_time,task_elapsed_time;
	TickType_t gps_timeout=SEC_TO(UART0_GPS_RECEPTION_TIMEOUT);//time out of task
	//set up shared semaphore
    semaphore_init(&sem_gps_uart0_callback,SEC_TO(UART0_GPS_PARSER_TIMEOUT));
    //set up all the pheriphericals needed
    task_uart0_gps_setup();
    set_store_struct(&gpsneoxm_buffer);
    //after all is properly set up, is time to start the gps
    for(;;){
        gpsneoxm_buffer.mem.head.acq_time_start=xTaskGetTickCount();    //time the task started
        reset_store_struct(&gpsneoxm_buffer);
    	task_start_time=xTaskGetTickCount();//store the tastk time
    	keep_gps_enabled=true;//enable gps logging
    	gps_start(&gps);
    	gps_parser.parse_reset();//resets the parser mechanism before adquiring a new data
         #ifdef PRINT_UART
          Display_printf(display, 0, 0, "awaiting rececption");
         #endif
        /*Recieves messages until timeout expires or all messages have been recieved*/
        while(keep_gps_enabled){//keep_gps_enabled IS TRUE
          if (semaphore_trytotake(&sem_gps_uart0_callback)==true) {
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
            	  //process to store the message
            	  //process this message and store it. If all messages were stored, shutdown gps
                  task_elapsed_time=xTaskGetTickCount()-task_start_time;//init the timeout
            	  keep_gps_enabled=not(gps_memory_store(&gps_parser,task_elapsed_time));
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
        //store the messages collected, even its there are no one to store
      gpsneoxm_buffer.mem.head.acq_time_end=xTaskGetTickCount();//time the task ended
	  if (semaphore_trytotake(&sem_spi0_microsd)==true){//takes the sempaphore
	  	        //copy all the bytes from the main structure to the data b8uffer
		        //store the times
	  	    	foocpy(microsd_write_buffer,gpsneoxm_buffer.byte,sizeof(gpsneoxm_buffer.byte));
	  	        //call the sd function to create the thread
	  	         thread_create(task_data_p[0]);
	  	      }
	  	 //create a microsd component to store all.After finishing, the micro sd will release the resource
       gps.turn_off();//turn off the gps because either a timeout happened or the system collacted all the requested frames
        #ifdef PRINT_UART
         Display_printf(display, 0, 0,(char *)"GPS turned off after %4f seconds,sleeping task until programmed awake...zzZ",TO_SEC(task_elapsed_time));
        #endif
       //-------------end of task, delay until next one--------------------
       vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS scheduller
      }
}
