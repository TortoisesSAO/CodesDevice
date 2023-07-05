/*

 */
#ifndef DEVICE_SYSTEM_SETUP_H_
#define DEVICE_SYSTEM_SETUP_H_
#include <device_system_config.h>
//--->>>this settings will impact over "device_system_information.h"
//select the max stack based on the type of device to optimize code footprint
// struct for fixex stack size. can be changed for each type of proyect
#if (DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO) //set up for min footprint
 #if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD) //set up for min footprint
 #define MAX_RTOS_ALLTASKS_STACK_SIZE 19000
 #else
 #define MAX_RTOS_ALLTASKS_STACK_SIZE 10000
 #endif
#endif
#if (DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD) //set up for min footprint
 #if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD) //set up for min footprint
 #define MAX_RTOS_ALLTASKS_STACK_SIZE 19000
 #else
 #define MAX_RTOS_ALLTASKS_STACK_SIZE 10000
 #endif
#endif
//LED DEFINED FOR LOW POWER INDICATOR. Hardwar dependandt
#if (DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO) //set up for min footprint
#define GPIO_LED_STATUS_LOW_POWER_LED    18
#endif
#if (DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD) //set up for min footprint
#define GPIO_LED_STATUS_LOW_POWER_LED    17
#endif
///system parameters: PRintu
#ifdef RTOS_PRINTU
#if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD) //set up for min footprint
  #define PRINT_QUEUE_LIST_SIZE     10
  #define PRINT_MAX_CHARS_PER_QUEUE 127
#else
  #define PRINT_QUEUE_LIST_SIZE     10
  #define PRINT_MAX_CHARS_PER_QUEUE 300 //
#endif
#endif
//define the max amount of data that can be stored into a sensor
#if (DEVICE_TYPE_KIND==DEVICE_TYPE_MD) //
#define MAX_SENSOR_BUFFER_AMOUNT       4096  //(BYTES)//max possible size of a sensor buffer.
#else
#define MAX_SENSOR_BUFFER_AMOUNT       1024  //(BYTES) //TO REDUCE FOOTPRINT
#endif
//for MICRO SD storage
#ifdef SD_STORAGE_ON
#define SD_MAX_STORE_AMOUNT MAX_SENSOR_BUFFER_AMOUNT //(BYTES) //max amounts of bytes that can be stored in a SD unit. must be equal to MAX_SENSOR_BUFFER_AMOUNT
#endif
//for lizards MD, remove sd storage option
#if((DEVICE_TYPE_KIND==DEVICE_TYPE_MD)&(DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD))
    #ifndef SD_STORAGE_ON                        //DISABLES THIS CHECK
         #undef  SD_STORAGE_ON
    #endif
#endif
//----------------- DEVELOPMENT_BOARD_EMPTY code:
#ifdef DEVELOPMENT_BOARD_EMPTY
    #ifndef STATE_MACHINE_BAT_CHECK_DISABLED      //DISABLES THIS CHECK
         #define STATE_MACHINE_BAT_CHECK_DISABLED
    #endif
    #ifndef STATE_MACHINE_GPS_DISABLED            //DISABLES THIS CHECK
         #define STATE_MACHINE_GPS_DISABLED
    #endif
    #ifndef SD_STORAGE_ON                        //DISABLES THIS CHECK
         #undef  SD_STORAGE_ON
    #endif
#endif
//IMU0 SENSOR SETUP
//---------------------------------------
#if IMU_ACCEL_GYRO_SAMPLE_RATE_HZ==15
#define IMU_ACCEL_GYRO_SAMPLE_RATE      1// 1 = 14.9 Hz,2 = 59.5 , 3 = 119  4 = 238  5 = 476 6= 952 Hz ACCEL/GYRO
#define IMU_ACCEL_GYRO_PERIOD_TO_READ   1.067 //set the period as 16/sample rate: ex; 16/14.9 Hz=1.067
#endif
#if IMU_ACCEL_GYRO_SAMPLE_RATE_HZ==60
#define IMU_ACCEL_GYRO_SAMPLE_RATE      2// 1 = 14.9 Hz,2 = 59.5 , 3 = 119  4 = 238  5 = 476 6= 952 Hz ACCEL/GYRO
#define IMU_ACCEL_GYRO_PERIOD_TO_READ   0.267 //set the period as 16/sample rate: ex; 16/59.5 Hz=0.267
#endif
//**************************************************
////MAG0 SENSOR SETUP
#if IMU_MAG_SAMPLE_RATE_HZ==5   //5 Hz
#define IMU_MAG_SAMPLE_RATE             3     //3=Hz
#define IMU_MAG_PERIOD_TO_READ          0.2   ///set the period as 1/sample rate: ex; 1/5 Hz=0.2
#endif
#if IMU_MAG_SAMPLE_RATE_HZ==10            //10 Hz
#define IMU_MAG_SAMPLE_RATE             4    //4=10Hz
#define IMU_MAG_PERIOD_TO_READ          0.1   ///set the period as 1/sample rate: ex; 1/10 Hz=0.1
#endif
// to indicate you are using an "empty" hardware (Like developments board) with not perhiperifcal
//for animal SAO, you can use the animal detection engnie
//dependencies related to IMU_ANIMAL_DETECTION
//----------------enable dependencies
#if((DEVICE_TYPE_KIND==DEVICE_TYPE_MD)&(DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO))
#ifdef IMU_ANIMAL_ACTIVITY_CHANGE_SENSORS_TURN_OFFIMU
      #ifndef IMU_ANIMAL_ACTIVITY_CHANGE_SENSORS_ENABLED_IMU
        #define IMU_ANIMAL_ACTIVITY_CHANGE_SENSORS_ENABLED_IMU
    #endif
#endif
#ifdef IMU_ANIMAL_ACTIVITY_CHANGE_SENSORS_ENABLED_IMU
      #ifndef IMU_ANIMAL_ACTIVITY_ENABLED
        #define IMU_ANIMAL_ACTIVITY_ENABLED
    #endif
#endif
#endif
//GPS DEPENDENSIES
#if((DEVICE_TYPE_KIND==DEVICE_TYPE_MD)&(DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD))
#define STATE_MACHINE_GPS_DISABLED
#endif
//RF definitions
//for parsing the data recieded with DCS using a python script (DO NOT CHANGE)
#define RX_DCS_STARTSTRING "\r\nRxbyDCS:"
#define RX_DCS_ENDSTRING    "RxEND"
//ADC channels and battery checks setup
#if(DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO)
#define ADC_BATTERY_LEVEL_SCALE    16      //1500k resistor,100k load or 1000k resistor 67k load
#define ADC_CHARGER_CURRENT_SCALE  4.4     //16k resistor,  4.7k load
#define ADC_USB_LEVEL_SCALE        22.27   //100k resistor, 4.7k load
#endif
#if(DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD)
#define ADC_BATTERY_LEVEL_SCALE    1      //1 non scale, direct meassure
#define ADC_CHARGER_CURRENT_SCALE  0      //0 To null value
#define ADC_USB_LEVEL_SCALE        0      //0 to null value
#define STATE_MACHINE_BAT_CHECK_DISABLED  //avoid the battery check for testing
#define STATE_MACHINE_USB_CHECK_DISABLED  //avoid the battery check for testing
#endif

//----------------------------------------Batery related parameters
//NON USED. defined inside "device_sytem_config"
//----------------------------------------
//----------------------------test modes
//#define GPS_SAVEALL_NOPARSE_RF_TXDATA_QUICK

#ifdef GPS_SAVEALL_NOPARSE_RF_TXDATA_QUICK
   RF_TX_DATA_PERIOD                   2.5
  #ifndef GPS_SAVEALL_NOPARSE
    #define GPS_SAVEALL_NOPARSE       300
  #endif

#endif


#endif //


