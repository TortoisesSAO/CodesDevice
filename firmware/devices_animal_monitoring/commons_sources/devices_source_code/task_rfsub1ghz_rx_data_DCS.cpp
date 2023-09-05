#include "device_system_information.h"
#if (DEVICE_TYPE_KIND==DEVICE_TYPE_DCS)
/* Standard C Libraries */
#include <stdlib.h>
//reception of diferent kind of animals message
#include "memory_structures/memory_struct_rf_head.h"
#include "memory_structures/memory_struct_rf_data_tortoise.h"
#include "memory_structures/memory_struct_rf_data_lizard.h"
#include <TI_CC13XX_id/device_id_TI_CC13X2.h>//micro sd main libary
/* BIOS Header files */
#include "rtos_tasks.h"
#include "state_machine.h"
#include "task_spi0_microsd.h"
#include "task_animal_activity.h"
#include "task_rfsub1ghz_tx_data.h"
//----------------------------------
#include <time_AL/time_AL.h>//time and date managment in standard time.c
/* Application Header files */
#include "easylink_custom/EasyLink.h"
#include <drivers_AL/uart_AL.h>//driver uart

#ifdef RTOS_PRINTU
#include "task_uart_printu.h"
#else
#define  printuf(...)    ;//replace the function for a ;;
#endif

uint8_t get_type_of_message(char* c,uint8_t num){
    uint8_t c_amount;
    c[0]='\0';
    if(num==0){ c_amount=sprintf(c,"UNKNOWN");}
    if(num==RF_MSSG_DATA){c_amount=sprintf(c,"DATA");}
    if(num==RF_MSSG_CMD){c_amount=sprintf(c,"CMD");}
    if(num==RF_MSSG_ACK){ c_amount=sprintf(c,"ACK");}
//    if (c[0]=='\0'){c_amount=sprintf(c,"UNKNOWN");}
    return(c_amount);
 }
//-----------------------------------------
uint8_t get_type_of_device(char* c,uint8_t num){

uint8_t c_amount;
c[0]='\0';
if(num==0){ c_amount=sprintf(c,"UNKNOWN_TYPE");}
if(num==DEVICE_TYPE_RF_MD_TORTO){c_amount=sprintf(c,DEVICE_TYPE_MD_TORTOISE);}
if(num==DEVICE_TYPE_RF_TD_TORTO){c_amount=sprintf(c,DEVICE_TYPE_TD_TORTOISE);}
if(num==DEVICE_TYPE_RF_DCS_TORTO){c_amount=sprintf(c,DEVICE_TYPE_DCS_TORTOISE);}
if(num==DEVICE_TYPE_RF_MD_LIZARD){c_amount=sprintf(c,DEVICE_TYPE_MD_LIZARD);}
if(num==DEVICE_TYPE_RF_TD_LIZARD){c_amount=sprintf(c,DEVICE_TYPE_TD_LIZARD);}
if(num==DEVICE_TYPE_RF_DCS_LIZARD){c_amount=sprintf(c,DEVICE_TYPE_DCS_LIZARD);}
//if (c[0]=='\0'){c_amount=sprintf(c,"UNKNOWN_TYPE");}
return(c_amount);
}
static  sempaphore_struct sem_rfsub1ghz_api;
//static  sempaphore_struct sem_uart_callback;
#define SEM_RFSUB1GHZ_API_TIMEOUT            70 //70 seconds for reset just in case system crash
#define RF_RX_RADIO_SETUP                    EasyLink_TX_DATA_Phys
#define RF_RX_DATA_FREQ                      RF_TX_DATA_FREQ //central tx freq

static EasyLink_rf_Params EasyLink_RfParams_TX_DATA;
static EasyLink_RxPacket rxPacket = {0, 0, 0, 0, 0, {0}};
//static EasyLink_TxPacket txPacket = {0, 0, 0, {0}};//declared to maintain consistency, commented for warnings
static EasyLink_Status   rx_status; //result of reception
static uint32_t          rx_time;    //result of reception
static int8_t            rx_rssi;    //result of reception
#ifdef RF_TX_DATA_CCA_MODE
#include <ti/drivers/TRNG.h>
#define CONFIG_TRNG_EASYLINK CONFIG_TRNG_0
static TRNG_Params trngParams;
static TRNG_Handle trngHandle;
#define RANDOM_BYTES_LENGTH (4U)
/* Array used to store random bytes */
static uint8_t randomBytesBuffer[RANDOM_BYTES_LENGTH];
#define MAX_TRNG_RETRIES      (2U)

uint32_t getRandomNumber( void ){
    int_fast16_t result = TRNG_STATUS_ERROR;
    uint8_t breakCounter = MAX_TRNG_RETRIES;
    do {if(0U == breakCounter--)  {printuf("ERROR TASK RF TX DATA:Unable to generate a random value");}
        else{ result = TRNG_getRandomBytes(trngHandle, randomBytesBuffer, RANDOM_BYTES_LENGTH);}
       }while(TRNG_STATUS_SUCCESS != result);
    return(*((uint32_t *)randomBytesBuffer));
}
#endif

//uart port for interfacing
static  sempaphore_struct sem_uart_callback;
#define UART0_TD_INTERFACE_PORT_NUMBER                    uart_mu0
#define UART0_TD_INTERFACE_BAUD_RATE                      115200
#define UART0_TD_INTERFACE_MODE                           UART_CALLBACK//UART_NONBLOCKING
static uart_struct uart0_interface;
//---------------------------------------------------
static char data_message_str[520];
//---------day counter system--------------

typedef   mem_rf_rx_data_animal_union mem_struct; //GENERAL structure for animal data RX.
//-------------------------------------------------------------
static mem_struct buffer_data_rx={
      {//.m
        {MEM_STR_ID_HEAD,RFDATA_STR_ID,RFDATA_BLOCK_UNIT_SIZE,1,0,0}, //head //.m.head.data_written=1 in this case.
        {0},                                                              //d
        {0},                                                              //d
        {0}                                                               //d
      }
      };
static void set_store_struct(mem_struct* pmem,EasyLink_RxPacket * prxPacket,uint32_t local_rx_time)
{
   //here you can add a classification of the RX data. for example
    memcpy(&(pmem->m.id_tx),&(prxPacket->payload),sizeof(data_id_tx_rf));//DO NOT COMMENT
    pmem->m.id_rx.date_time_compressed=local_rx_time;  //store reception time
    pmem->m.id_rx.rssi=prxPacket->rssi;
    //--------copy to data_id struct. at this time, you can parse the type of device to choose where to store
    if (pmem->m.id_tx.type_of_device ==DEVICE_TYPE_RF_MD_LIZARD){
        mem_rf_tx_data_lizard data;//to allow an easy manipulation
        memcpy(&(data),&(prxPacket->payload),sizeof(data));
        memcpy(&(pmem->m.d),&(data.d),sizeof(data.d)); //copy info to data buffer
      }
    if (pmem->m.id_tx.type_of_device ==DEVICE_TYPE_RF_MD_TORTO){
        mem_rf_tx_data_tortoise data;//to allow an easy manipulation
        memcpy(&(data),&(prxPacket->payload),sizeof(data));
        memcpy(&(pmem->m.d),&(data.d),sizeof(data.d)); //copy info to data buffer
     }
}

//
static void print_error_message(uint32_t time_32t,int8_t rssi, EasyLink_Status status){
char str_time[30];
date_time_uint32_to_string(str_time, time_32t);
printuf("\r\nDCS RX failed at %s UTC.RSSI: %i, error code:%i,",str_time,(int)rssi,(int)status);
}
/*convert the data recieved in the last packet into a human readable one*/
static void data_tortoise_to_message_str(mem_rf_rx_data_tortoise_union* buffer,char* str){
    char c[100];
    int c_amount;
    char c_mac[24];
    uint8_t i_aux;
    str[0]=0;//to init the string to store
    //---------------------------------------
    c_amount=sprintf(c, "\r\n");//to indicate new line
    strncat(str,c,c_amount);
    c_amount=sprintf(c, "DCS Rx Date UTC:");//ccontent example"2023/05/21 00:00:00 "
    strncat(str,c,c_amount);
    c_amount=date_time_uint32_to_string(c, buffer->m.id_rx.date_time_compressed);
    strncat(str,c,c_amount);
    strcat(str,", TX: ");
    day_time_in_seconds_to_string(c,(((uint32_t)buffer->m.d.day_time_2_sec_res)<<1));
    strncat(str,c,8);
    //----------add RSSI, Device number, device ID
    device_id_to_str(c_mac,buffer->m.id_tx.device_id);
    i_aux=get_asigned_num_from_id_str(c_mac);
    c_amount=sprintf(c, ", RSSI:%i,NUM: %i, ID: %s|",(int)buffer->m.id_rx.rssi,i_aux,c_mac);
    strncat(str,c,c_amount);
    //add message type and device type
    c_amount=get_type_of_message(c_mac,buffer->m.id_tx.type_of_message);
    c_amount=sprintf(c, ",MSSJ:\"%s\",",c_mac);
    //c_amount=sprintf(c, "MSSJ:%i", buffer->m.id_tx.type_of_message);
    strncat(str,c,c_amount);
    c_amount=get_type_of_device(c_mac,buffer->m.id_tx.type_of_device);
    c_amount=sprintf(c, "DE_type:\"%s\".",c_mac);
    //c_amount=sprintf(c, "DE_type:%i.",buffer->m.id_tx.type_of_device);
    strncat(str,c,c_amount);
	c_amount=sprintf(c, "|activity: %i, act.time: %i [sec], BAT percent: %i, T_MCU: %i, T_IMU: %i,",
	      (int) buffer->m.d.animal_activity,(int) buffer->m.d.animal_activity_time,
	      (int) buffer->m.d.bat_level_percent,(int)buffer->m.d.temperature_c, (int)buffer->m.d.temperature_c_imu);
	strncat(str,c,c_amount);
	c_amount=sprintf(c, "GPS lat: %.4f, lon: %.4f,hdop: %.1f, fix at date:",
	           buffer->m.d.gps_lat,buffer->m.d.gps_lon,((float)buffer->m.d.gps_hdop_2dec)/100);
	strncat(str,c,c_amount);
    //add gps date and time
    c_amount=date_time_uint32_to_string(c, buffer->m.d.gps_date_time_compressed);
    strncat(str,c,c_amount);
	strcat(str," UTC.");
}
static void data_lizard_to_message_str(mem_rf_rx_data_lizard_union* buffer,char* str){
    char c[100];
    int c_amount;
    char c_mac[24];
    uint8_t i_aux;
    str[0]=0;//to init the string to store

    //---------------------------------------
    c_amount=sprintf(c, "\r\n");//to indicate new line
    strncat(str,c,c_amount);
    c_amount=sprintf(c, "DCS Rx Date UTC:");//ccontent example"2023/05/21 00:00:00 "
    strncat(str,c,c_amount);
    c_amount=date_time_uint32_to_string(c, buffer->m.id_rx.date_time_compressed);
    strncat(str,c,c_amount);
    strcat(str,", TX: ");
    day_time_in_seconds_to_string(c,(((uint32_t)buffer->m.d.day_time_2_sec_res)<<1));
    strncat(str,c,8);
    //----------add RSSI, Device number, device ID
    device_id_to_str(c_mac,buffer->m.id_tx.device_id);
    i_aux=get_asigned_num_from_id_str(c_mac);
    c_amount=sprintf(c, ", RSSI:%i,NUM: %i, ID: %s|",(int)buffer->m.id_rx.rssi,i_aux,c_mac);
    strncat(str,c,c_amount);
    //add message type and device type
    c_amount=get_type_of_message(c_mac,buffer->m.id_tx.type_of_message);
    c_amount=sprintf(c, ",MSSJ:\"%s\",",c_mac);
    //c_amount=sprintf(c, "MSSJ:%i", buffer->m.id_tx.type_of_message);
    strncat(str,c,c_amount);
    c_amount=get_type_of_device(c_mac,buffer->m.id_tx.type_of_device);
    c_amount=sprintf(c, "DE_type:\"%s\".",c_mac);
    //c_amount=sprintf(c, "DE_type:%i.",buffer->m.id_tx.type_of_device);
    strncat(str,c,c_amount);
    c_amount=sprintf(c, "|activity: %i, act.time: %i [sec], BAT percent: %i, T_MCU: %i, T_IMU: %i.",
          (int) buffer->m.d.animal_activity,(int) buffer->m.d.animal_activity_time,
          (int) buffer->m.d.bat_level_percent,(int)buffer->m.d.temperature_c, (int)buffer->m.d.temperature_c_imu);
    strncat(str,c,c_amount);
}

#ifdef RF_SEND_DATA_TO_DCS
//Generates a message to be parsed by a python structure:
static void data_to_message_str_to_plotter(mem_rf_rx_data_animal_union* buffer_rx,char* str){
    char c[100];
    int c_amount;
    char c_mac[24];
    uint8_t i_aux;
    str[0]=0;//to init the string to store
    //set the buffer to the proper one
    mem_rf_rx_data_tortoise_union* buffer=(mem_rf_rx_data_tortoise_union*)buffer_rx;
    //---------------------------------------
    c_amount=sprintf(c, RX_DCS_STARTSTRING);//to indicate new line
    strncat(str,c,c_amount);
    //date_time_gps_UTC,reception_time,ID_num,bat_percent,animal_activity,activity_time,temp_MCU,temp_IMU,lat,lon,hdop,ID,device_type,RSSI
    //-----------------
    //date_time_gps_UTC,reception_time
    c_amount=date_time_uint32_to_string(c,buffer->m.d.gps_date_time_compressed);//ToD0
    strncat(str,c,c_amount);
    c_amount=sprintf(c, ",");strncat(str,c,c_amount);
    c_amount=day_time_in_seconds_to_string(c,(((uint32_t)buffer->m.d.day_time_2_sec_res)<<1));
    strncat(str,c,c_amount);
    //get MAC ADDRESS as string
    device_id_to_str(c_mac,buffer->m.id_tx.device_id);
    //ID_num,bat_percent
    i_aux=get_asigned_num_from_id_str(c_mac);
    c_amount=sprintf(c, ",%i,%i,",(int)i_aux,(int) buffer->m.d.bat_level_percent);
    strncat(str,c,c_amount);
    //animal_activity, animal activity time
    c_amount=get_animal_activity_str(c,buffer->m.d.animal_activity);
    strncat(str,c,c_amount);
    c_amount=sprintf(c, ",%i",(int) buffer->m.d.animal_activity_time);
    strncat(str,c,c_amount);
    //temp_MCU,temp_IMU
    c_amount=sprintf(c,",%i,%i",buffer->m.d.temperature_c,buffer->m.d.temperature_c_imu);
    strncat(str,c,c_amount);
    //gps lat, lon, hdop
   if (buffer->m.id_tx.type_of_device==DEVICE_TYPE_RF_MD_TORTO){
    c_amount=sprintf(c, ",%.4f,%.4f,%.1f",buffer->m.d.gps_lat,buffer->m.d.gps_lon,((float)buffer->m.d.gps_hdop_2dec/100));
   }
   else{c_amount=sprintf(c, ",%.4f,%.4f,%.1f",0.00,0.00,0.99);}//use hdop=0.99 to allow the point in the map to be seen
    strncat(str,c,c_amount);
    //ID,device_type,RSSI
    c_amount=sprintf(c,",");strncat(str,c,c_amount);
    strncat(str,c_mac,24); //adding mac address
    c_amount=sprintf(c,",");strncat(str,c,c_amount);
    c_amount=get_type_of_device(c,buffer->m.id_tx.type_of_device);
    strncat(str,c,c_amount);
    c_amount=sprintf(c,",%i",buffer->m.id_rx.rssi);
    strncat(str,c,c_amount);
    //End message to indicate the end of parsing
    c_amount=sprintf(c,RX_DCS_ENDSTRING);
    strncat(str,c,c_amount);
}
//declared to maintain consistency.//commented to remove warnings
#endif
//-----------------------------------
/*static void rfsub1ghz_tx_callback(EasyLink_Status status){
    semaphore_release(&sem_rfsub1ghz_api);
}*/
static void rfsub1ghz_rx_callback(EasyLink_RxPacket * prxPacket, EasyLink_Status status){
    rx_status=status; //store status of reception
    rx_rssi=prxPacket->rssi;
    rx_time=localtime_uint32();//get time at Rx
    memcpy(&(rxPacket),prxPacket,sizeof(rxPacket));
    semaphore_release(&sem_rfsub1ghz_api);
}

static void radiosub1ghz_setup(TaskData* ptask_data){

    if(semaphore_trytotake(&sem_rfsub1GHz)){
        #ifdef RF_TX_DATA_CCA_MODE
         //init TRNG enfinew
         TRNG_init();
         TRNG_Params_init(&trngParams);
         trngParams.returnBehavior = TRNG_RETURN_BEHAVIOR_POLLING;
         trngHandle = TRNG_open(CONFIG_TRNG_EASYLINK, &trngParams);
         if(NULL == trngHandle) {printuf("ERROR TASK RF TX DATA:Failed to init TRNG driver");}
         EasyLink_RfParams_TX_DATA.EasyLink_params.pGrnFxn = (EasyLink_GetRandomNumber)getRandomNumber;
         #endif
         EasyLink_init_phys(&RF_RX_RADIO_SETUP,&EasyLink_RfParams_TX_DATA);//!= EasyLink_Status_Success) //{while (1);};
         EasyLink_switch_radio(&EasyLink_RfParams_TX_DATA);
         EasyLink_setFrequency(RF_RX_DATA_FREQ);//to change the frequenncy
         EasyLink_setRfPower(RF_TX_DATA_POWER);
         ///----------close after setup
         EasyLink_close(&EasyLink_RfParams_TX_DATA);
         #ifdef RF_TX_DATA_CCA_MODE
         TRNG_close(trngHandle);
         #endif
      #ifdef RTOS_PRINTU
       printuf("\r\n%s:TX Data at Freq:%.3f MHz, Tx Power:%i dBm",ptask_data->name,
                (((float)RF_RX_DATA_FREQ)/1000000),RF_TX_DATA_POWER );
     #ifdef RF_TX_DATA_CCA_MODE
     printuf((const char*)"\r\nRF TX Data Carrier-Sense Multiple Access MODE ENABLE");
    #else
    printuf((const char*)"\r\nRF TX Data Carrier-Sense Multiple Access MODE OFF");
     #endif
    #endif
       semaphore_release(&sem_rfsub1GHz);}
}

extern "C"	{
 void task_rfsub1ghz_DCS_rx_data(void* ptask_data);
}
//---------------------------------------------
 void task_rfsub1ghz_DCS_rx_data(void* ptask_data){
	EasyLink_Status local_rx_status;//declared here to avoid issues with the intraction
	uint32_t local_rx_time;//declared here to avoid issues with the intraction
	int8_t local_rx_rssi;
	//TickType_t clocktick_start,clocktick_end;//uncomment if you want to meassure times
    //set uart for allowing the comand-transfer between devices
    //--Note: Reception is not used in this case
    semaphore_init(&sem_uart_callback,SEC_TO(1));//INIT SEMAPHORE FOR THIS API
    semaphore_trytotake(&sem_uart_callback);
    uart_set(&uart0_interface,UART0_TD_INTERFACE_PORT_NUMBER,UART0_TD_INTERFACE_BAUD_RATE,UART0_TD_INTERFACE_MODE,NULL);

    state_machine_wait_for_ADQ_DATA_state();
    //-----------
	 vTaskDelay(SEC_TO(1));//wait for parameters update
	 semaphore_init(&sem_rfsub1ghz_api,SEC_TO(SEM_RFSUB1GHZ_API_TIMEOUT));//INIT SEMAPHORE FOR THIS API
	 semaphore_trytotake(&sem_rfsub1ghz_api);//this must be taken after creation for proper work
	 printuf("\r\nStarting DCS radio set up");
	 radiosub1ghz_setup((TaskData*) ptask_data);// Initialize the radio
	 //--proceed to recieve. sem_rfsub1GHz will never be released beacuse only this thread will use the radio!
	 semaphore_trytotake(&sem_rfsub1GHz);//take and never release
     EasyLink_switch_radio(&EasyLink_RfParams_TX_DATA);// start the radio. This is keep to maintain code consistency
	 EasyLink_receiveAsync(rfsub1ghz_rx_callback, 0);//trys to recieve another packet
	 printuf("\r\nDCS Radio ok, awaiting messages... ");
	while(1){
                 if(semaphore_trytotake(&sem_rfsub1ghz_api)){
                     //saves the result in a local variable
                    local_rx_status=rx_status;
                    local_rx_time=rx_time;
                    local_rx_rssi=rx_rssi;
                    //allow reception again while system process the content
                    set_store_struct(&buffer_data_rx,&rxPacket, rx_time);//keep outside the callback function to classify packets
                    EasyLink_receiveAsync(rfsub1ghz_rx_callback, 0);//trys to recieve another packet
                   //check if the RX was ok
                   if (local_rx_status==EasyLink_Status_Success){
                    //-----NEW packet A=rives, copy content to adecuate buffer based on type of message//data to plot
                    #ifdef RF_SEND_DATA_TO_DCS
                     if (buffer_data_rx.m.id_tx.type_of_message==RF_MSSG_DATA){//if it is a meesage with data, send to plotter
                         data_to_message_str_to_plotter(&buffer_data_rx,data_message_str);//generate data to plot with python app
                         printuf(data_message_str); }//send data to uart port
                    #endif
                    //---------make decitions based on the type of message------------------------------------
                     if (buffer_data_rx.m.id_tx.type_of_device==DEVICE_TYPE_RF_MD_TORTO){
                         data_tortoise_to_message_str((mem_rf_rx_data_tortoise_union*)&buffer_data_rx,data_message_str);}
                     if (buffer_data_rx.m.id_tx.type_of_device==DEVICE_TYPE_RF_MD_LIZARD){
                         data_lizard_to_message_str((mem_rf_rx_data_lizard_union*) &buffer_data_rx,data_message_str);}
                     printuf(data_message_str);
                     memcpy(&(data_message_str[350]),(char*)(&(buffer_data_rx.b[0])),sizeof(buffer_data_rx.m));
                    #ifdef SD_STORAGE_ON
                     memory_store_str((void*)&data_message_str,sizeof(data_message_str));
                    #endif
                    }
                    #ifdef RF_ERROR_PRINT
                    else{print_error_message(local_rx_time,local_rx_rssi, local_rx_status);}
                    #endif
                 }else{EasyLink_receiveAsync(rfsub1ghz_rx_callback, 0);}//trys to recieve another packet}
                //--------exit task based on state machine state
                 if(state_machine_is_lowpower()){break;}//exit if extremelow
                 if(state_machine_is_shutdown()){break;}//exit if low power
                //-------------Task Delay until next period----------
   }
    while(1){vTaskDelay(TASK_SLEEP_FOREVER);
 }
}
#endif
