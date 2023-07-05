
#include <device_system_setup.h>
#include "rtos_tasks.h"          //allow the use of an operative system function
#include "state_machine.h"       //allow the use of an state machine
#ifdef RTOS_PRINTU
#include "task_uart_printu.h"   //for printing services
#endif
//---------specific libraries
#include <stdlib.h>
#include "easylink_custom/EasyLink.h"         //for RF driver
#include <TI_CC13XX_id/device_id_TI_CC13X2.h>// main library_to get device ID


//easy link exclusive parametyers
//------for asyncronic reception
static  sempaphore_struct sem_rfsub1ghz_ka;
#define SEM_RFSUB1GHZ_KA_TIMEOUT 0.5

static void rfsub1ghz_ka_tx_callback(EasyLink_Status status){
	semaphore_release(&sem_rfsub1ghz_ka);
}
/*static void rfsub1ghz_ka_rx_callback(EasyLink_RxPacket * rxPacket, EasyLink_Status status){
	semaphore_release(&sem_rfsub1ghz_ka);
}*/

static uint32_t ka_frequency_asigned;
static EasyLink_TxPacket txPacket = {0, 0, 0, {0}};
// Initialize the EasyLink parameters to their default values
static EasyLink_rf_Params EasyLink_RfParams_KA;

/*peforms a transmition of data*/
static inline void init_rf_sub1ghz_ka(TaskData* ptask_data){
    uint8_t i;
    if(semaphore_trytotake(&sem_rfsub1GHz)){
        //EasyLink_Params_init(&(EasyLink_RfParams_KA.EasyLink_params));
        if(EasyLink_init_phys(&EasyLink_KA_Phys,&EasyLink_RfParams_KA)!= EasyLink_Status_Success) {
            while (1);};
         semaphore_init(&sem_rfsub1ghz_ka,SEC_TO(SEM_RFSUB1GHZ_KA_TIMEOUT));//INIT SEMAPHORE FOR THIS API
         semaphore_trytotake(&sem_rfsub1ghz_ka);//this must be taken after creation for proper work of Async engine
         for (i = 0; i < RF_KA_PACKET_LENGHT; i++)  {txPacket.payload[i] = 255;}//write all ones in binary
         txPacket.len = RF_KA_PACKET_LENGHT;//Load the packet
         EasyLink_switch_radio(&EasyLink_RfParams_KA);
         EasyLink_setFrequency(ka_frequency_asigned);//to change the frequenncy
         EasyLink_setRfPower(RF_KA_TX_POWER);
         //------------close the driver after turn on
         EasyLink_close(&EasyLink_RfParams_KA);
#ifdef RTOS_PRINTU
 vTaskDelay(SEC_TO(0.1));//0.2 crahsWAIT TO ENSURE A PROPER SET UP OF printuf task
 printuf("\r\n%s:KA pulse at Freq:%.3f MHz, Tx Power:%i dBm, TX Packet Payload Lenght:%i bytes ",ptask_data->name,
        ((float)ka_frequency_asigned)/1000000,RF_KA_TX_POWER, RF_KA_PACKET_LENGHT );
#endif
         semaphore_release(&sem_rfsub1GHz);
    }
}
/*peforms a transmition of data*/
static inline void rf_sub1ghz_ka_transmit(void){
    if(semaphore_trytotake(&sem_rfsub1GHz)){
        //SET THE PARAMETERS OF THE TX
        EasyLink_switch_radio(&EasyLink_RfParams_KA);
        /* Create packet with incrementing sequence number and random payload */
        EasyLink_transmitAsync(&txPacket, rfsub1ghz_ka_tx_callback);
        //EasyLink_transmitCcaAsync(&txPacket, rfsub1ghz_ka_tx_callback);
        semaphore_trytotake(&sem_rfsub1ghz_ka);//will take the sem after tx if finish
        vTaskDelay(SEC_TO(EASYLINK_DELAY_TO_CLOSE_SECS));//waits 10 msecs to ensure other radio operators ends. seeEASYLINK_IDLE_TIMEOUT_MS
        EasyLink_close(&EasyLink_RfParams_KA);
        semaphore_release(&sem_rfsub1GHz);
    }
}

extern "C"	{
 void task_rfsub1ghz_ka(void* ptask_data);
}
//---------------------------------------------
 void task_rfsub1ghz_ka(void* ptask_data)
{   //settings main parameters
	TickType_t xLastWakeTime = 0;// Initialise the xLastWakeTime variable with the current time.TickType_t xFrequency = task_data_p[task_index]->t;
	TickType_t xFrequency = ((TaskData*) ptask_data)->t;
	//configuring devices to use
    //TickType_t clocktick_start,clocktick_end;//uncomment if you want to meassure times
    ka_frequency_asigned=(device_ID_get_ka_frequency());//150010000;
    // Initialize the EasyLink parameters to their default values
    init_rf_sub1ghz_ka((TaskData*) ptask_data);
    vTaskDelay(SEC_TO(0.2));//WAIT FOR PROPER SETUP
    while(1){
	          rf_sub1ghz_ka_transmit();
		      //CHECK IF BAT IS CRITICAL
		      if(state_machine_is_shutdown()){break;}//exit process
		      //-------------task kP delay
	 	      vTaskDelayUntil(&xLastWakeTime, xFrequency);//delays until xFrequency has passed in the RTOS
      }
   while(1){vTaskDelay(TASK_SLEEP_FOREVER);}
}

