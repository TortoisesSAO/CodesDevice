/*

 */
#ifndef DEVICE_SYSTEM_CONFIG_H_
#define DEVICE_SYSTEM_CONFIG_H_
//this header allows you to select the setup of your device
//ANIMAL LIST                                  $add an unique number to each of this fields
#define ANIMAL_TORTOISE_SAO                   48488448468 //must be a unique number in order to allow a  MACRO comparison
#define ANIMAL_LIZARD                         48986556565 //must be a unique number in order to allow a MACRO comparison
//DEVICE KIND LIST
#define DEVICE_TYPE_MD                        488685535 //MOnitorting device //must be a unique number in order to allow a  MACRO comparison
#define DEVICE_TYPE_TD                        89198568  ///Tracking device //must be a unique number in order to allow a  MACRO comparison
#define DEVICE_TYPE_DCS                       48488848  //Data Collector Station //must be a unique number in order to allow a  MACRO comparison
//*----------------------------------------------------------------
//*-------------------Select your kind of device--------------------
//*----------------------------------------------------------------
#define DEVICE_TYPE_KIND                      DEVICE_TYPE_MD
#define DEVICE_TYPE_ANIMAL                    ANIMAL_TORTOISE_SAO
//--->>>this settings will impact over "device_system_setup.h" and"device_system_information.h" and
//select the max stack based on the type of device to optimize code footprint
// struct for fixex stack size.
///system parameters
#define RTOS_PRINTU  //alows the use of uart print functions. You can disable this to reduce code footprint
//define the max amount of data that can be stored into a sensor
//-------------------------------------------------
//for MICRO allow  SD storage
#define SD_STORAGE_ON
//----------------------------------------------------------------------------
//#define DEVELOPMENT_BOARD_EMPTY      //uncomment to run the code with a CC1312R development board. Most of the functions will be bypased
//----------------------------------------------------------------------------
// Leds setup
#define LED_PERIOD_ADQ_START_TIME       0          //300 //seconds:
#define LED_PERIOD_ADQ                  1          //in seconds.period for led blink once system is in ACQ state and  LED_PERIOD_ACQ_START_TIME has elapsed
#define LED_TIMEON_ADQ                  0.001      //in seconds.time the led will be on  once system is in ACQ state and  LED_PERIOD_ACQ_START_TIME has elapsed
//IMU sensors setup
#define IMU_ACCEL_GYRO_SAMPLE_RATE_HZ      15   // 15 or 60 is setted up
//**************************************************
//mag sensor setup
#define IMU_MAG_SAMPLE_RATE_HZ             10   //5 OR 10 is setted up
// to indicate you are using an "empty" hardware (Like developments board) with not perhiperifcal
//for animal SAO, you can use the animal detection engnie
#if((DEVICE_TYPE_KIND==DEVICE_TYPE_MD)&(DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO)) //ONLY FOR MD and TORTOISE
#define IMU_ANIMAL_ACTIVITY_ENABLED        //enabled the detection and inclution of animal activity
//#define IMU_ANIMAL_ACTIVITY_CHANGE_SENSORS_ENABLED_IMU  //enable to check if should turn off the imu
//#define IMU_ANIMAL_ACTIVITY_CHANGE_SENSORS_TURN_OFFIMU         //enable to turn off the imu after chack
//--------parameters for animal actrivity detection
#define IMU0_ACCEL_TRESHOLD_TORTOISE        1200 //1600=0.1g. accelerometer scale 0.000061=1g, 0,00059841=1 m/s
#define IMU0_ACCEL_AVERAGE_SAMPLES_TORTOISE 10   //amount of samples to average
#define IMU_STILL_TIMEOUT                   20   // if animal stays still for more than IMU_STILL_TIMEOUT, it is considered that the gyroscope should be turn off
#endif
//---------------GPS setup
#define STATE_MACHINE_GPS_DISABLED    //uncomment to disable the GPS check of the state machine and disable the module
//#define GPS_SAVEALL_NOPARSE          300// enable this to run make the GPS collect and sotore allmessages during the time specified in seconds
#define GPS_ACQ_PERIOD                 600  //in seconds: Time between system tries to get GPS samples
#define GPS_ACQ_MAX_TIME               60   //in seconds: Max time to adquire the GPS samples
#define GPS_GPS_NUMBER_OF_MESSAGES     3    //amount of messages types to be adquired. 3 is the default value
#define GPS_MAX_RESOLUTION_ERROR       10   //GPS Resolution in meters. Data will be adquired only
//#define GPS_ADQUIRE_IF_ANIMAL_NOT_STILL     //enable to only turn on GPS only if animal moved since last position adquisition
#ifdef GPS_ADQUIRE_IF_ANIMAL_NOT_STILL
#define IMU_ANIMAL_ACTIVITY_ENABLED          //enable dependensies
#endif
//max RF power selection: WARNING: MUST SET .syscfg->Device Configuration ->Force VDDR
#define CCFG_FORCE_VDDR_HH_IS_DEFINED////must also be seted also inside .syscfg->Device Configuration ->Force VDDR
#ifdef  CCFG_FORCE_VDDR_HH_IS_DEFINED
#define RF_MAX_POWER  14              //14 dB mode increase MCU general power consumption by a 15%.
#else
#define RF_MAX_POWER  13              //normal mode
#endif
//RF KA setup
#define RF_KA_PACKET_LENGHT                 1 // in bytes.must be 1 at least for packet Tx. all bytes value is one
#define RF_KA_TX_POWER                      13 //in dBm use 13 to not drawn too much current
#define RF_KA_FREQ_DEFAULT                  148000000 //in Hz
#define RF_KA_PERIOD                        2.5 //in seconds
//RF TX_DATA setup
#define RF_TX_DATA_POWER                    RF_MAX_POWER //14 means that will attempt to tx at max power if config allows it
#define RF_TX_DATA_FREQ                     149950000 //in Hz
#define RF_TX_DATA_PERIOD                   4 //in seconds
//#define RF_TX_DATA_CCA_MODE               //uncomment (!NOt tested) to enable CSMA mode for TX_DATA. 2023/06: NOt tested
//#define RF_SEND_DATA_TO_DCS                //allow to send data to DCS. Disabled for debugging
#define RF_ERROR_PRINT                     //Print when a bad Rx happens.
//----------------------------------------Batery related parameters
#define SYSTEM_STORE_BATTERY_LEVEL              //define if you want to store battery levels
//select the battery for your proyect
#define BAT_LIPO_3700mV_600mAh       3700600   //asig number to compare
#define BAT_NOBAT_3300mV             330000000 //as ig number to compare

//select the battery for your proyect
#if (DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO)
#define DEVICE_BATTERY BAT_LIPO_3700mV_600mAh
#endif
#if (DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD)
#define DEVICE_BATTERY BAT_NOBAT_3300mV      //battery NOT mapped
#endif


#if DEVICE_BATTERY==BAT_LIPO_3700mV_600mAh//scales are defined in:
//------------------------------------------------------------------------------------
#define USB_LEVEL_CONNECTED          4500       //mV
#define BATTERY_LEVEL_FULL           4200       //mVFor LiPO Battery 4.2V
#define BATTERY_LEVEL_FULL_charged   4100       //mV For LiPO Battery 4.2V, we use 4100 to avoid a bad meassure fue to resistor bridge mismatch
#define CHARGE_CURRENT_charged       50         //mA For LiPO Battery 4.2V 600 mAH. below 50 mA of current charge means bat is almost full
#define BATTERY_LEVEL_LOW            3300//3400       //For LiPO Battery 4.2V
#define BATTERY_LEVEL_CRITICAL       3050//3050       //For LiPO Battery 4.2V, this value is 3V
#define BATTERY_LEVEL_LOW_PERCENT    6
#endif
//-------------------Lizards
#if DEVICE_BATTERY==BAT_NOBAT_3300mV//scales are defined in:
//------------------------------------------------------------------------------------
#define USB_LEVEL_CONNECTED          4500       //keep this value above your batery level
#define BATTERY_LEVEL_FULL           4200       //non used, but must be declared
#define BATTERY_LEVEL_FULL_charged   4100       //m
#define CHARGE_CURRENT_charged       50         //mA
#define BATTERY_LEVEL_LOW            10         //
#define BATTERY_LEVEL_CRITICAL       0         // define with your battery
#define BATTERY_LEVEL_LOW_PERCENT    0         // define with your battery
#endif
//----------------------------------------
//----------------------------test modes
//#define GPS_SAVEALL_NOPARSE_RF_TXDATA_QUICK
#endif //


