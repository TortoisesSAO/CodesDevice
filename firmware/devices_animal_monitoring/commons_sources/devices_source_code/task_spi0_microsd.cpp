
#include "device_system_information.h"
#ifdef SD_STORAGE_ON
//system general  libraries
#include "rtos_tasks.h"          //allow the use of an operative system function
//for printing services
#ifdef RTOS_PRINTU
#include "task_uart_printu.h"
#endif
//---------specific libraries
//storage unit library
#include <memory_structures/memory_struct_system.h> //for storage unit
//specific libraries of this task
#include "task_spi0_microsd.h"         //defines
#include <micro_sd_fat/micro_sd_fat.h>//micro sd main libary
#include <TI_CC13XX_id/device_id_TI_CC13X2.h>// main library_to get device ID
//for interacting with  RF
#include "task_rfsub1ghz_tx_data.h"      //.h to interact with send "RF data" Task
//-----------------------------------------------------
//task specific defines
#define MICROSD_FOLDER_STR_SIZE 8 //DO NOT CHANGE
#define MICROSD_MAC_STR_SIZE 24   //DO NOT CHANGE
static char microsd_date_str[MICROSD_FOLDER_STR_SIZE+1]="NODATEAV";
static char device_folder_str[MICROSD_FOLDER_STR_SIZE];
char device_id_str[MICROSD_MAC_STR_SIZE];
static char battery_level_str[3]="NA";
static char device_number_str[4]="NON";
static device_ID_data device_id_data;
static uint64_t device_id_num=0;
static int  microsd_check_ret=1;//test of microsd.
//----------for data store
static const char MEM_BUFFER_OVERFLOW_MESSAGE[]="\r\nSD ERROR: Attempting to store a buffer with Bigger size than SD. Max allowed "
        "Buffer wrote below this message will be limited to size of \"buffer_data\" ";
static const char MEM_BUFFER_ZERO_MESSAGE[]="\r\nSD ERROR: Attempting to store a buffer with size 0. No store will be performed";
static microsd_struct microsd;
static bool memory_write_task_on=false;
static sempaphore_struct sem_sd_write;              //for internal use
#define SEM_SD_WRITE_TIMEOUT SEM_WAIT_FORVER;
sempaphore_struct* psem_spi0=NULL;


void memory_store_give_shared_spi_semaphore(sempaphore_struct* psem_spi){
    psem_spi0=psem_spi;
}


//-------------------
typedef mem_system_data_union mem_struct;
static mem_struct buffer_data={
      {//.m
        {MEM_STR_ID_HEAD,SYSTEM_STR_ID}, //head
        {{0}},                             //data
      }
      };

void microsd_set_battery_level(uint16_t bat_level_mv, uint8_t bat_level_percent){
 buffer_data.m.data.batery_level_in_mv=bat_level_mv;
 if (bat_level_percent>0){//only copy the value if requested
	  if (bat_level_percent>99){battery_level_str[0]='1';}                      else{battery_level_str[0]='0';}
	   if (bat_level_percent>9){battery_level_str[1]=bat_level_percent/10+'0';} else{battery_level_str[1]='0';}
	   	   	   	   	   	   	   	battery_level_str[2]=bat_level_percent%10+'0';
    }
 }
//-------------------------------------------------------------------------------------
//--------This elements shall be copy with each character
unsigned char microsd_write_buffer[SD_BLOCK_UNIT_SIZE];
static uint16_t bytes_to_write; //verfied
//-------------------------------
//semaphore utilities
void microsd_start(microsd_struct* microsd_p){
    //set variables relevant to the use of the system
	//get device id to create file name
    device_id_num=get_device_id();
    get_device_id_str(device_id_str);//first we get the device ID
    device_id_data=device_ID_table_find_value(device_id_str);
    device_folder_str[0]=0;//init the stucture
     if (device_id_data.device_number>0){
              //convert to number
              device_number_str[0]=device_id_data.device_number/100 +48;
              device_number_str[1]=device_id_data.device_number/10-(device_id_data.device_number/100)*10+48; //if (device_number<100){(device_number/10  +48);}
              device_number_str[2]=device_id_data.device_number%10  +48;
              strncat(device_folder_str,DEVICE_FOLDERNAME_5_LETERS,5);//DO NOT REMOVE 5 IF USING SDFAT LIBRARY!!!
              strncat(device_folder_str,device_number_str,3);         //DO NOT REMOVE 3 IF USING SDFAT LIBRARY!!!
              }
     else{MAC_ADDRESS_get_last8_chars(device_id_str,device_folder_str);}
    //--------------------------------------------------------------------------
	microsd_set_mount(microsd_p,SD_SPI_DRIVER_NUMBER, SD_MOUNT_DRIVE_NUM,( char*) SD_FATFS_PREFIX);
    microsd_set_file_properties(microsd_p,(char*) device_folder_str,(char*) SD_FILE_EXTENTION,(char*) SD_OPEN_MODE);
	microsd_init(microsd_p);
}
//

extern "C"	{
void task_spi0_microsd_write_block(void* ptask_data);
void task_spi0_microsd( void* ptask_data);
}

//-------------------------------------//periodic calls
/* Copy the amounts " bytes_to_write" of the  microsd_write_buffer to the micro sd card */
void task_spi0_microsd_write_block(void* ptask_data){
//proceed to perform a write
microsd_open_write_close(&microsd,microsd_write_buffer,bytes_to_write);
((mem_data_union*) microsd_write_buffer)->m.head.block_size=0;//cleans this to ensure data has been written
semaphore_release(psem_spi0);//release the resource
}


void microsd_write_file_into_device_folder(unsigned char* data_p, uint16_t data_size, char* name_all,uint8_t name_size)
{    char mode[]=SD_OPEN_MODE;//"a "=append data to file mode
    if (semaphore_trytotake(psem_spi0)){
        microsd_open_write_close_name(&microsd,data_p, data_size, name_all, name_size ,mode);
        semaphore_release(psem_spi0);//release the resource
    }
}
int memory_store(void* pMem_to_store){
int ret=1; //1==error: could not take the semaphore
#ifndef DEVELOPMENT_BOARD_EMPTY
if (semaphore_trytotake(psem_spi0)){
    ret=0;//could take the semaphore. other errors will be raised if the following operations fails
	bytes_to_write=((mem_data_union*) pMem_to_store)->m.head.block_size;//do not change
	//protections against a bad call function call (such as store 0 bytes or more than the buffer size)
	if (bytes_to_write>sizeof(microsd_write_buffer)){
	    microsd_open_write_close(&microsd,(unsigned char *)MEM_BUFFER_OVERFLOW_MESSAGE,sizeof(MEM_BUFFER_OVERFLOW_MESSAGE));
	                      bytes_to_write=sizeof(microsd_write_buffer);
	    ret=2;}//size too big
	if (bytes_to_write>0){
	  //now we have the size of the buffer, so we proceed to perform a complete copy after writing
	   memcpy(microsd_write_buffer, pMem_to_store,bytes_to_write);//copy from pMem_to_store to microsd_write_buffer
	     ret=0;
	   }
	else{microsd_open_write_close(&microsd,(unsigned char *)MEM_BUFFER_ZERO_MESSAGE,sizeof(MEM_BUFFER_ZERO_MESSAGE));
	    ret=3;//bytes to write equal 0}}//store error
	   }
    semaphore_release(&sem_sd_write);//release the resource
   }
#else
ret=0;//succesfull (because is a simulatiom)
#endif
return(ret);
}
void memory_store_str(void* pMem_to_store,uint16_t str_size)
{	//str_size is the amounts of characters you want to be written into file
#ifndef DEVELOPMENT_BOARD_EMPTY
  if (semaphore_trytotake(psem_spi0)){
    if(memory_write_task_on==false){
      microsd_open_write_close(&microsd,(unsigned char*)pMem_to_store,str_size);//
      semaphore_release(psem_spi0);//release the resource
    }
    else{
      bytes_to_write=str_size;
      //protections against a bad call function call (such as store 0 bytes or more than the buffer size)
      if (bytes_to_write>sizeof(microsd_write_buffer)){
          bytes_to_write=sizeof(microsd_write_buffer);}
      if (bytes_to_write==0){bytes_to_write=1;}//to ensure no fails in the engine
      //now we have the size of the buffer, so we proceed to perform a complete copy after writing
	  memcpy(microsd_write_buffer, pMem_to_store,bytes_to_write);//copy from pMem_to_store to microsd_write_buffer
	  semaphore_release(&sem_sd_write);//release the resource
    }
    }
#endif
}

//-------------------------only called once

void microsd_set_date_and_create_firstfile(void){
	  char* wmessage;
	  char time_str[20];
	  uint8_t c_amount=0;
	  //first get tickclocks and date
	  time_t rawtime;
	  struct tm timeinfo;
	  struct tm* ptime;
	  time(&rawtime);
	  ptime = localtime(&rawtime);
	  timeinfo=*ptime;
	  buffer_data.m.data.localtime=timeinfo;
	  buffer_data.m.data.system_day_time_rtos=xTaskGetTickCount();//get current time ticks at this moment for syncro
	  //if the folder has a valid date (above 2020,change value). This is to avoid issues at debugging
	  if ((ptime->tm_year+TIME_YEARS_SINCE)>2020){get_date_as_8long_string(microsd_date_str);}
	  //create folder with acq date
      microsd_create_subdir(&microsd,microsd_date_str,sizeof(microsd_date_str));
      //perform a first store
      buffer_data.m.data.device_id=device_id_num;
      buffer_data.m.data.device_number=device_id_data.device_number;
      buffer_data.m.data.rtos_clock_tick_period_hz=config_tick_rate_HZ;
      memcpy(&(buffer_data.m.data.device_type),DEVICE_TYPE,sizeof(DEVICE_TYPE));
      memcpy(&(buffer_data.m.data.hardwareVersion),DEVICE_HARDWARE_VERSION,sizeof(DEVICE_HARDWARE_VERSION));
      memcpy(&(buffer_data.m.data.firmwareVersion),DEVICE_FIRMWARE_VERSION,sizeof(DEVICE_FIRMWARE_VERSION));
      //gerenate a message to show to te user
	  buffer_data.m.data.presentation_message[0]=0;//init pointer
	  wmessage=&(buffer_data.m.data.presentation_message[0]);
	  strcat(wmessage,":Device Type:");
	  strcat(wmessage,DEVICE_TYPE);
	  strcat(wmessage,",HardV:");
	  strcat(wmessage,DEVICE_HARDWARE_VERSION);
	  strcat(wmessage,",FirmV:");
	  strcat(wmessage,DEVICE_FIRMWARE_VERSION);
	  strcat(wmessage,", Device ID \"0");//do not change
	  strncat(wmessage,&(device_id_str[0]),1);//do not change to work with TICLANG compiler
	  strncat(wmessage,&(device_id_str[1]),sizeof(device_id_str)-1);//do not change to work with TICLANG compiler
	  strcat(wmessage,"\",Number:");
	  strcat(wmessage, device_number_str);
	  strcat(wmessage,"| Start adquisition date:");
	  c_amount=date_time_tm_to_string(time_str,ptime);
	  strncat(wmessage,time_str,c_amount);
	  strcat(wmessage," UTC");
	  strcat(wmessage,", Batery Level:");
	  strncat(wmessage, battery_level_str,3);//leave 3 as fixed
	  strcat(wmessage,"\%");
     //writes first message with all the requested data
	  microsd_open_write_close(&microsd , (unsigned char*) (buffer_data.b), sizeof(buffer_data) );
}

int memory_check_sd_connected_ok(void){
    int ret=1;//1:failed
    if (semaphore_trytotake(psem_spi0)){
          if (microsd_check_ret>0){//not tested yet
          //if ok, get id of device and set main folder
              ret=check_micro_sd_connected(&microsd);//run this first to improve speed
             if (ret==0){
               microsd_create_subdir(&microsd,device_folder_str,sizeof(device_folder_str));
               microsd_check_ret=0; }//CREATE FIRST FOLDER
          }else{ret=0;}
        semaphore_release(psem_spi0);
    }
return (ret);
}
/*call this function after ACQ state has been reached*/
void memory_create_first_file(void){
   if (semaphore_trytotake_time(psem_spi0,SEC_TO(10))){
    microsd_set_date_and_create_firstfile();
    semaphore_release(psem_spi0);}
}

void task_spi0_microsd(void* ptask_data){
   //turn on micro sd
   int ret;
   if (semaphore_trytotake(psem_spi0)){//takes the resource first to avoid the use of the resource before system is ready
      microsd_start(&microsd);
      memory_write_task_on=true;
//------------------------------------------------------------------------------------
      semaphore_init(&sem_sd_write,SEM_WAIT_FOREVER);
      semaphore_trytotake(&sem_sd_write);//release the resource
      semaphore_release(psem_spi0);//release the resource
}
   //perform SD check and tell user:
 ret=memory_check_sd_connected_ok();
#ifdef RTOS_PRINTU
   vTaskDelay(SEC_TO(0.1));//WAIT TO ENSURE A PROPER SET UP OF printuf task
   if (ret==0){printuf("\r\n%s:MicroSD check SUCCESFULL. Ready to write!",((TaskData*)ptask_data)->name);}
          else{printuf("\r\n%s:MicroSD check FAILED. Could not write into MicroSD ",((TaskData*)ptask_data)->name);}
 #endif
while(1){
         semaphore_trytotake(&sem_sd_write);//awaits a resource release to write
         task_spi0_microsd_write_block((TaskData*) ptask_data);//will releease sem_spi0
}

}
#endif
