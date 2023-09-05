
/* Standard C Libraries */
#include <stdlib.h>
#include "device_system_information.h"
#if (DEVICE_TYPE_KIND==DEVICE_TYPE_TD)
#include "memory_structures/memory_struct_rf_data_tortoise.h"
/* BIOS Header files */
#include "rtos_tasks.h"
#include "state_machine.h"
#include "task_spi0_microsd.h"
#include <TI_CC13XX_id/device_id_TI_CC13X2.h>//micro sd main libary
//----------------------------------
#include <time_AL/time_AL.h>//time and date managment in standard time.c
/* Application Header files */
#include "easylink_custom/EasyLink.h"
/* Data struct file*/
#include <drivers_AL/uart_AL.h>//driver uart

#ifdef RTOS_PRINTU
#include "task_uart_printu.h"
#else
#define  printuf(...)      ; //replace the function for a ;;
#endif


#define UART0_TD_INTERFACE_PORT_NUMBER                    uart_mu0
#define UART0_TD_INTERFACE_BAUD_RATE                      115200
#define UART0_TD_INTERFACE_MODE                           UART_CALLBACK//UART_BLOCKING//UART_NONBLOCKING

static  sempaphore_struct sem_rfsub1ghz_api;
static  sempaphore_struct sem_uart_callback;
#define SEM_RFSUB1GHZ_API_TIMEOUT            30
#define RF_RX_RADIO_SETUP                    EasyLink_KA_Phys
#define RF_RX_DATA_FREQ                      RF_KA_FREQ_DEFAULT //central tx freq


static EasyLink_rf_Params EasyLink_RfParams;
static EasyLink_RxPacket rxPacket = {0, 0, 0, 0, 0, {0}};
//static EasyLink_TxPacket txPacket = {0, 0, 0, {0}};//commented to remove warking, defined to maintain consistency
static EasyLink_Status   rx_status; //result of reception
static uint32_t          rx_time;    //result of reception
static int8_t            rx_rssi;    //result of reception
static uart_struct uart0_interface;


static char data_message_str[200];
//---------day counter system--------------
static uint32_t rfsub1ghz_freq=RF_RX_DATA_FREQ;
//----------------------------------------------------------
typedef mem_rf_rx_data_animal_TD_union mem_struct;
//-------------------------------------------------------------
static mem_struct buffer_data={
      {//.m
        {MEM_STR_ID_HEAD,RFDATA_STR_ID,RFDATA_BLOCK_UNIT_SIZE,1,0,0}, //head. Note://.m.head.data_written=1 in this case.
        {0},                                                                    //conf
        0,
        0
      }
      };
static void set_store_struct(mem_struct* pmem,EasyLink_RxPacket * prxPacket)
{
memcpy(&(pmem->m.id_rx.rssi),&(prxPacket->rssi),1);
memcpy(&(pmem->m.sec_to_next_packet_tx),(prxPacket->payload),1);
//reset packet after copy the elements. This is not necesary, just done for debug purposes
}
static void update_store_struct(mem_struct * pPacket){
    pPacket->m.freq=rfsub1ghz_freq;
    pPacket->m.id_rx.date_time_compressed=localtime_uint32();
}
//---------------------------------------------------
//commented to remove warking, defined to maintain consistency
/*static void rfsub1ghz_tx_callback(EasyLink_Status status){
    semaphore_release(&sem_rfsub1ghz_api);
}*/
static void rfsub1ghz_rx_callback(EasyLink_RxPacket * prxPacket, EasyLink_Status status){
    rx_status=status;
    memcpy(&(rxPacket.rssi),&(prxPacket->rssi),1);
    memcpy(&(rxPacket.absTime),&(prxPacket->absTime),sizeof(rxPacket.absTime));
    memcpy(&(rxPacket.dstAddr),&(prxPacket->dstAddr),sizeof(rxPacket.dstAddr));
    memcpy(&(rxPacket.rxTimeout),&(prxPacket->rxTimeout),sizeof(rxPacket.rxTimeout));
    memcpy(&(rxPacket.len),&(prxPacket->len),1);
    memcpy(&(rxPacket.payload),(&(rxPacket.payload)),prxPacket->len);
    semaphore_release(&sem_rfsub1ghz_api);

}
static void print_error_message(uint32_t time_32t,int8_t rssi, EasyLink_Status status){
char str_time[30];
date_time_uint32_to_string(str_time, time_32t);
printuf("\r\nTD RX failed at %s UTC.RSSI: %i, error code:%i,",str_time,(int)rssi,(int)status);
}
static void data_to_message_str(char* str){
    char c[50];
    int c_amount;
    str[0]=0;//to init the string to store
    //---------------------------------------
    c_amount=sprintf(c, "\r\n");//to indicate new line
    strncat(str,c,c_amount);
    c_amount=sprintf(c, "TD Rx Date: ");//ccontent example"2023/05/21 00:00:00 "
    strncat(str,c,c_amount);
    c_amount=date_time_uint32_to_string(c, buffer_data.m.id_rx.date_time_compressed);
    strncat(str,c,c_amount);
    c_amount=sprintf(c, " UTC, RSSI:%i, at frequency: %.3f MHz.",
    buffer_data.m.id_rx.rssi,((float)buffer_data.m.freq/1000000));//ccontent example"2023/05/21 00:00:00 "
    strncat(str,c,c_amount);
}
static void radiosub1ghz_setup(void){
    if(semaphore_trytotake(&sem_rfsub1GHz)){
         semaphore_init(&sem_rfsub1ghz_api,SEC_TO(SEM_RFSUB1GHZ_API_TIMEOUT));//INIT SEMAPHORE FOR THIS API
         semaphore_trytotake(&sem_rfsub1ghz_api);//this must be taken after creation for proper work
         //----init KA phys layer
         EasyLink_init_phys(&EasyLink_KA_Phys,&EasyLink_RfParams);//
         EasyLink_switch_radio(&EasyLink_RfParams);
         EasyLink_setFrequency(RF_RX_DATA_FREQ);//to change the frequenncy
         EasyLink_setRfPower(RF_TX_DATA_POWER);
         semaphore_release(&sem_rfsub1GHz);//release the resource
    }
}
void callback_uart0_td(UART2_Handle handle, void *buffer, size_t count, void *userArg, int_fast16_t status);
static void uart_rx(void);
extern "C"	{
 void task_rfsub1ghz_TD_rx_data(void* ptask_data);
}
//---------------------------------------------
 void task_rfsub1ghz_TD_rx_data(void* ptask_data){
     EasyLink_Status local_rx_status;//declared here to avoid issues with the intraction
     uint32_t local_rx_time;//declared here to avoid issues with the intraction
     int8_t local_rx_rssi;
     state_machine_wait_for_state(state_machine.SM_ADQ_DATA);
     vTaskDelay(SEC_TO(1));//wait for parameters update
     printuf("\r\nStarting TD radio set up");
     radiosub1ghz_setup();// Initialize the radio
     //set uart for allowing the comand-transfer between devices
     semaphore_init(&sem_uart_callback,SEC_TO(10));//INIT SEMAPHORE FOR THIS API
     semaphore_trytotake(&sem_uart_callback);
    uart_set(&uart0_interface,UART0_TD_INTERFACE_PORT_NUMBER,UART0_TD_INTERFACE_BAUD_RATE,UART0_TD_INTERFACE_MODE,callback_uart0_td);
    #ifdef RTOS_PRINTU
    printuf("\r\nPlease enter the number asigned to the frequency:");
    #endif
    uart_rx();//for to update radio
    //--proceed to recieve. sem_rfsub1GHz will never be released beacuse only this thread will use the radio!
    semaphore_trytotake(&sem_rfsub1GHz);//8
    EasyLink_receiveAsync(rfsub1ghz_rx_callback, 0);//trys to recieve another packet
    #ifdef RTOS_PRINTU
    printuf("\r\nTD Radio ok, awaiting messages... ");
    #endif
    while(1){
	       if(semaphore_trytotake(&sem_rfsub1ghz_api)){ //will succed when new packet arrives
                //-----NEW packet Arrives, copy content to main buffer
                local_rx_status=rx_status;
                local_rx_time=rx_time;
                local_rx_rssi=rx_rssi;
                set_store_struct(&buffer_data,&rxPacket);
                //allow reception again while system process the content
                EasyLink_receiveAsync(rfsub1ghz_rx_callback, 0);//trys to recieve another packet
                //check if the RX was ok
                if (local_rx_status==EasyLink_Status_Success){
                //---------send the data in legible language to the uart port for user--------------
                   update_store_struct(&buffer_data);
                   data_to_message_str(data_message_str);
                   printuf(data_message_str);
                   //---------store data with timestamp-------------------------------------
                   memcpy(&(data_message_str[70]),(char*)&(buffer_data.b[0]),sizeof(buffer_data.m));
                   #ifdef SD_STORAGE_ON
                   memory_store_str((void*)&data_message_str,sizeof(data_message_str));
                   #endif
                  }
                    #ifdef RF_ERROR_PRINT
                    else{print_error_message(local_rx_time,local_rx_rssi, local_rx_status);}
                    #endif
                  }else{EasyLink_receiveAsync(rfsub1ghz_rx_callback, 0);}//trys to
		    //--------exit task based on state machine state
             if(state_machine_is_lowpower()){break;}//exit if extremelow
             if(state_machine_is_shutdown()){break;}//exit if low power
          }
 while(1){vTaskDelay(TASK_SLEEP_FOREVER);}
}

 /* allows the user to change frequency to one in the table*/
void rx_uart_update_freq(uint8_t input){
    uint32_t freq_ka_rx;
    freq_ka_rx=get_ka_frequency_from_num(input);//For integer cast
    if  (freq_ka_rx>0){
        rfsub1ghz_freq=freq_ka_rx;
        EasyLink_abort();
        EasyLink_setFrequency(rfsub1ghz_freq);//to change the frequenncy
        }
    else{
    rfsub1ghz_freq=RF_RX_DATA_FREQ;
    EasyLink_abort();
    EasyLink_setFrequency(rfsub1ghz_freq);//to change the frequenncy7EasyLink_getFrequency()
    }
 }

/*
static void uart_rx(void){
    char c[10];
    c[0]='\0';//init
    if(semaphore_trytotake(&sem_uart0)){
         uart_init(&uart0_interface);
         uart_recieve_reset(&uart0_interface);
         uart_recieve(&uart0_interface,c,sizeof(10));
         uart_close(&uart0_interface);
         semaphore_release(&sem_uart0);//release the resource
     }
    rx_uart_update_freq((uint8_t)c[9]-48);
   #ifdef RTOS_PRINTU
    printuf(c);
    printuf("\r\nRX UART:%s",c);
    printuf(&(c[1]));
    printuf("\r\nRx KA frequency set: %.3f MHz",((float)EasyLink_getFrequency())/1000000);
   #endif
}
*/
 static char c_callback;
void callback_uart0_td(UART2_Handle handle, void *buffer, size_t count, void *userArg, int_fast16_t status){
    memcpy(&c_callback,buffer,1);
    semaphore_release(&sem_uart_callback);
 }
static void uart_rx(void){
    char c=0;
    if(semaphore_trytotake(&sem_uart0)){
         uart_init(&uart0_interface);
         c_callback=0;
         uart_recieve(&uart0_interface,&c,1);
         semaphore_trytotake(&sem_uart_callback);
         c=c_callback;
         uart_close(&uart0_interface);
         semaphore_release(&sem_uart0);//release the resource
    }
    rx_uart_update_freq(((uint8_t)c-48));
   #ifdef RTOS_PRINTU
    printuf("\r\nRX UART:%s",c);
    printuf("\r\n Rx KA frequency set: %.3f MHz",((float)EasyLink_getFrequency())/1000000);
   #endif
}




#endif
