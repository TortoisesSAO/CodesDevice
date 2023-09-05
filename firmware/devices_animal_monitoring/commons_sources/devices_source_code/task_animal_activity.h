
#ifndef _TASK_ANIMAL_ACTIVITY_H
#define _TASK_ANIMAL_ACTIVITY_H
#include <lsm9ds1/LSM9DS1_Types.h>    //for IMU structures
#ifdef __cplusplus
  //extern "C" {
#endif
typedef enum
  {  //here you should define any mode of operation
	ANIMAL_,                   //default value: no behaivour has been detected
  	ANIMAL_STILL,             //animal is still and no moving. ex: sleep
	ANIMAL_MOVING,            //animal is moving
	ANIMAL_EATING,            //animal is eating
	ANIMAL_NESTING,           //animal is putting eggss
	ANIMAL_COPULATING,         //animal is eating
  }animal_activity_enum_tortoise;
//#define IMU0_ACCEL_TRESHOLD_TORTOISE        1200 //1600=0.1g. accelerometer scale 0.000061=1g, 0,00059841=1 m/s
//#define IMU0_ACCEL_AVERAGE_SAMPLES_TORTOISE 10 //1600=0.1g for

  /*stimates anival acvtivity using a non blocking technique*/
typedef animal_activity_enum_tortoise animal_activity_enum;
typedef struct
    {  //here you should define any mode of operation
	   animal_activity_enum state;               //current state
	   uint32_t             state_elapsed_time;  //elapsed time in this state as rtos-ticks
    }animal_activity_struct;

#define IMU_ACCEL_TRESHOLD IMU0_ACCEL_TRESHOLD_TORTOISE
//#define IMU_STILL_TIMEOUT  20 //defined in "device_system_config"
    uint8_t get_animal_activity_str(char* c,uint8_t num);
#ifdef IMU_ANIMAL_ACTIVITY_ENABLED
    animal_activity_struct get_animal_activity(void);
void estimate_animal_activity_from_accel(FIFO_buffer_struct* imuLSM9DS1_data, uint8_t gyro_enable, uint16_t amount_of_data );

#ifdef __cplusplus
  // }
 #endif
#endif
#endif

