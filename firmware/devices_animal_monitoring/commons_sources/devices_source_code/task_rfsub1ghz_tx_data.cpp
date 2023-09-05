
/* Standard C Libraries */
#include <device_system_setup.h>
#if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD)
#include "rtos_tasks.h"          //allow the use of an operative system function
#include "state_machine.h"       //allow the use of an state machine
#ifdef RTOS_PRINTU
#include "task_uart_printu.h"   //for printing services
#endif
//---------specific libraries
#include <stdlib.h>
#include "task_rfsub1ghz_tx_data.h"
#include "easylink_custom/EasyLink.h"         //for RF driver
#include <TI_CC13XX_id/device_id_TI_CC13X2.h>// main library_to get device ID
//-----------------------------proyect dependant
#if(DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO)
#include "memory_structures/memory_struct_rf_data_tortoise.h"
typedef mem_rf_tx_data_tortoise_union mem_struct_rf;
#endif
#if(DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD)
#include "memory_structures/memory_struct_rf_data_lizard.h"
typedef mem_rf_tx_data_lizard_union mem_struct_rf;
#endif
///-------------------------------------------
static  sempaphore_struct sem_rfsub1ghz_api;
#define SEM_RFSUB1GHZ_API_TIMEOUT 0.5

static void rfsub1ghz_tx_callback(EasyLink_Status status){
	semaphore_release(&sem_rfsub1ghz_api);
}
/*static void rfsub1ghz_rx_callback(EasyLink_RxPacket * rxPacket, EasyLink_Status status){
	semaphore_release(&sem_rfsub1ghz_api);
}*/


#define RF_TX_DATA_DATA_FREQ                  RF_TX_DATA_FREQ //central tx freq
// Initialize the EasyLink parameters to their default values
static EasyLink_rf_Params EasyLink_RfParams_TX_DATA;
//static EasyLink_RxPacket rxPacket = {0, 0, 0, 0, 0, {0}};
static EasyLink_TxPacket txPacket = {0, 0, 0, {0}};
#if(DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO)
#include "memory_structures/memory_struct_rf_data_tortoise.h"
typedef mem_rf_tx_data_tortoise_union mem_struct_rf;
#endif
//-------------------------------------------------------------
static mem_struct_rf buffer_data;
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
/*this are functions are called by this task tasks in order to update TX packet buffer content*/
 void set_tx_packet_id(uint8_t type_of_device){
     uint64_t device_id;
     device_id=get_device_id();
     if(semaphore_trytotake(&sem_rfsub1GHz_packetTx)){
     buffer_data.m.id.type_of_device = type_of_device;
     buffer_data.m.id.device_id      = device_id;
     semaphore_release(&sem_rfsub1GHz_packetTx);}
 }
 /*update the type of packet to send in a thread-protected way*/
 void update_tx_packet_message_type(uint8_t type_of_device){
   if(semaphore_trytotake(&sem_rfsub1GHz_packetTx)){
     buffer_data.m.id.type_of_message = type_of_device;
     semaphore_release(&sem_rfsub1GHz_packetTx);}
 }
static void set_store_struct(mem_struct_rf* pmem){
    uint32_t day_time=get_elapsed_seconds_in_current_day();
    if(semaphore_trytotake(&sem_rfsub1GHz_packetTx)){
        pmem->m.id.type_of_message=1;
        pmem->m.d.day_time_2_sec_res=(day_time>>1);
        semaphore_release(&sem_rfsub1GHz_packetTx);
    }

}
/*peforms a transmition of data*/
//static inline tx_data();
/*peforms a transmition of data*/
static inline void init_rf_sub1ghz_tx_data(TaskData* ptask_data){
    vTaskDelay(SEC_TO(1));//wait for parameters update. do not change
    semaphore_init(&sem_rfsub1ghz_api,SEC_TO(SEM_RFSUB1GHZ_API_TIMEOUT));//INIT SEMAPHORE FOR THIS API
    semaphore_trytotake(&sem_rfsub1ghz_api);//this must be taken after creation for proper work
    if(semaphore_trytotake(&sem_rfsub1GHz)){//takes the radio
        #ifdef RF_TX_DATA_CCA_MODE
         //init TRNG enfinew
         TRNG_init();
         TRNG_Params_init(&trngParams);
         trngParams.returnBehavior = TRNG_RETURN_BEHAVIOR_POLLING;
         trngHandle = TRNG_open(CONFIG_TRNG_EASYLINK, &trngParams);
         if(NULL == trngHandle) {printuf("ERROR TASK RF TX DATA:Failed to init TRNG driver");}
         EasyLink_RfParams_TX_DATA.EasyLink_params.pGrnFxn = (EasyLink_GetRandomNumber)getRandomNumber;
         #endif
         EasyLink_init_phys(&EasyLink_TX_DATA_Phys,&EasyLink_RfParams_TX_DATA);//!= EasyLink_Status_Success) //{while (1);};
         EasyLink_switch_radio(&EasyLink_RfParams_TX_DATA);
         EasyLink_setFrequency(RF_TX_DATA_DATA_FREQ);//to change the frequenncy
         EasyLink_setRfPower(RF_TX_DATA_POWER);
         ///----------close after setup
         EasyLink_close(&EasyLink_RfParams_TX_DATA);
         #ifdef RF_TX_DATA_CCA_MODE
         TRNG_close(trngHandle);
         #endif
#ifdef RTOS_PRINTU
printuf("\r\n%s:TX Data at Freq:%.3f MHz, Tx Power:%i dBm, TX Packet Payload Lenght:%i bytes. ",ptask_data->name,
        (((float)RF_TX_DATA_DATA_FREQ)/1000000),RF_TX_DATA_POWER, (sizeof(mem_struct_rf)+sizeof(EasyLink_TxPacket)-EASYLINK_MAX_DATA_LENGTH));
#ifdef RF_TX_DATA_CCA_MODE
printuf((const char*)"\r\nRF TX Data Carrier-Sense Multiple Access MODE ENABLE");
#else
printuf((const char*)"\r\nRF TX Data Carrier-Sense Multiple Access MODE OFF");
#endif
#endif
semaphore_release(&sem_rfsub1GHz);}
}
/*peforms a transmition of data*/
static inline void rf_sub1ghz_txdata_load_data(EasyLink_TxPacket* ptxPacket,mem_struct_rf* pbuffer_data){
    set_store_struct(pbuffer_data);
    if(semaphore_trytotake(&sem_rfsub1GHz_packetTx)){
        ptxPacket->len=(uint8_t)(sizeof(pbuffer_data->m));    //message content size
        memcpy(&(ptxPacket->payload),&(pbuffer_data->b[0]),ptxPacket->len); //copy the message
        semaphore_release(&sem_rfsub1GHz_packetTx);
      }
}
/*peforms a transmition of data*/
static inline void rf_sub1ghz_txdata_transmit(EasyLink_TxPacket* ptxPacket ){
    if(semaphore_trytotake(&sem_rfsub1GHz)){
        /* Create packet with incrementing sequence number and random payload */
        #ifndef RF_TX_DATA_CCA_MODE
        EasyLink_switch_radio(&EasyLink_RfParams_TX_DATA);
        EasyLink_transmitAsync(ptxPacket, rfsub1ghz_tx_callback);
        #else
        trngHandle = TRNG_open(CONFIG_TRNG_EASYLINK, &trngParams);
        EasyLink_switch_radio(&EasyLink_RfParams_TX_DATA);
        EasyLink_transmitCcaAsync(ptxPacket, rfsub1ghz_tx_callback);
        #endif
        semaphore_trytotake(&sem_rfsub1ghz_api);//will take the sem after tx if finish
        vTaskDelay(SEC_TO(EASYLINK_DELAY_TO_CLOSE_SECS));//waits at least 5 msecs to ensure the radio power downs itself due to inactivity
        EasyLink_close(&EasyLink_RfParams_TX_DATA); //after the radio powered down, we can shut dowwn the driver and release the resource
        #ifdef RF_TX_DATA_CCA_MODE
        TRNG_close(trngHandle);
        #endif
        semaphore_release(&sem_rfsub1GHz);
      }
}
extern "C"	{
 void task_rfsub1ghz_tx_data(void* ptask_data);
}
//---------------------------------------------
 void task_rfsub1ghz_tx_data(void* ptask_data){
	//settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = ((TaskData*) ptask_data)->t;
    //if(state_machine_is_state_active(state_machine.SM_BAT_LOW)){task_detach_AL((TaskData*)ptask_data);}
	//TickType_t clocktick_start,clocktick_end;//uncomment if you want to meassure times
    //--------------set up device  radio
    init_rf_sub1ghz_tx_data((TaskData*) ptask_data);
    //-----------update rf packet with a default value
    time_t rawtime;
    tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);//verfied: work ok
    update_tx_packet_gps(0,0,0.99,*timeinfo); //verified, work ok. Hdop= 0.99 for allow plotting
    state_machine_wait_for_ADQ_DATA_state();///------wait for adq state to begin
    //before sen packets, set the kind of device and their id
    set_tx_packet_id(DEVICE_TYPE_RF);
	while(1){
	          update_tx_packet_message_type((uint8_t)RF_MSSG_DATA);
	          //load the tx packet with current bufferstore parameters
	          rf_sub1ghz_txdata_load_data(&txPacket,&buffer_data);
		      //tx the data using the radio engine
	          rf_sub1ghz_txdata_transmit(&txPacket);
		  	  //--------exit task based on state machine state
	          if(state_machine_is_lowpower()){break;}//exit process
		 	  //-------------Task Delay until next period----------
		 	  vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
           }
 while(1){vTaskDelay(TASK_SLEEP_FOREVER);}

}
 void update_tx_packet_gps(float lat, float lon, float hdop,tm Time){
  if(semaphore_trytotake(&sem_rfsub1GHz_packetTx)){
#if(DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO)
      buffer_data.m.d.gps_date_time_compressed=convertlocaltime_tm_to_uint32(Time);
      buffer_data.m.d.gps_lat=lat;
      buffer_data.m.d.gps_lon=lon;
      buffer_data.m.d.gps_hdop_2dec= (uint16_t)(hdop*100);//ex: if hdop=1.56, value stored will be 156
#endif
#if(DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD)
      buffer_data.m.d.gps_date_time_compressed=convertlocaltime_tm_to_uint32(Time);
      buffer_data.m.d.gps_lat=lat;
      buffer_data.m.d.gps_lon=lon;
      buffer_data.m.d.gps_hdop_2dec= (uint16_t)(hdop*100);//ex: if hdop=1.56, value stored will be 156
#endif
      semaphore_release(&sem_rfsub1GHz_packetTx);}
 }
void update_tx_packet_temperature(int8_t temperature_degrees,int8_t temperature_degrees_imu){
 if(semaphore_trytotake(&sem_rfsub1GHz_packetTx)){
     buffer_data.m.d.temperature_c=temperature_degrees;
     buffer_data.m.d.temperature_c_imu=  temperature_degrees_imu;
     semaphore_release(&sem_rfsub1GHz_packetTx);}
}

void update_tx_packet_battery_level(uint8_t battery_percent){
  if(semaphore_trytotake(&sem_rfsub1GHz_packetTx)){
      buffer_data.m.d.bat_level_percent=battery_percent;
      semaphore_release(&sem_rfsub1GHz_packetTx);}
}

void update_tx_packet_animal_activity(uint8_t animal_activity,uint32_t elapsed_time ){
if(semaphore_trytotake(&sem_rfsub1GHz_packetTx)){
    buffer_data.m.d.animal_activity=animal_activity;
    buffer_data.m.d.animal_activity_time=elapsed_time;//in seconds
    semaphore_release(&sem_rfsub1GHz_packetTx);}
 }
#endif
