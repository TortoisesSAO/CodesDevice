/*
 * freertos_custom_defines.h
 *
 *  Created on: 7 dic. 2021
 *      Author: Andres Oliva Trevisan
 */

#ifndef RTOS_TASK_PARAMS_H_
#define RTOS_TASK_PARAMS_H_

#include "device_system_information.h"
//default stack size
//TIMES ARE EXPRESSED IN seconds
#define TASKP_LEDS_NAME             "Task PWM Leds"
#define TASKP_LEDS_TIME             LED_TIMEON_ADQ      //(seconds)
#define TASKP_LEDS_PERIOD           LED_PERIOD_ADQ          //(seconds)
#define TASKP_LEDS_DEADLINE         LED_PERIOD_ADQ*2         //(seconds)
#define TASKP_LEDS_PRIORITY         configMAX_PRIORITIES-1 //max priority
#define TASKP_LEDS_TYPE             PERIODIC_TASK
#define TASKP_LEDS_STACKSIZE        512
// TEMP
#define TASKP_TEMP_NAME             "Task Temperature"
#define TASKP_TEMP_TIME             0.002      //(seconds)
#define TASKP_TEMP_PERIOD           30.         //(seconds)
#define TASKP_TEMP_DEADLINE         TASKP_TEMP_PERIOD*2         //(seconds)
#define TASKP_TEMP_PRIORITY         configMAX_PRIORITIES-9
#define TASKP_TEMP_TYPE             PERIODIC_TASK
#define TASKP_TEMP_STACKSIZE        1024
// SYSMONITOR
#define TASKP_SYSMONITOR_NAME       "Task System monitor"
#define TASKP_SYSMONITOR_TIME       0.01      //(seconds)
#define TASKP_SYSMONITOR_PERIOD     60.         //(seconds)
#define TASKP_SYSMONITOR_DEADLINE   TASKP_SYSMONITOR_PERIOD*1.5         //(seconds)
#define TASKP_SYSMONITOR_PRIORITY   configMAX_PRIORITIES-8
#define TASKP_SYSMONITOR_TYPE       PERIODIC_TASK
#define TASKP_SYSMONITOR_STACKSIZE  3048
// IMU ACC/GYRO TASK
#define TASKP_IMUAG_NAME            "Task IMU ACC/GYRO"
#define TASKP_IMUAG_TIME            0.010        //(seconds)
#define TASKP_IMUAG_PERIOD          IMU_ACCEL_GYRO_PERIOD_TO_READ        //(seconds) 1.067 def
#define TASKP_IMUAG_DEADLINE        TASKP_IMUAG_PERIOD*2        //(seconds)
#define TASKP_IMUAG_PRIORITY        configMAX_PRIORITIES-3
#define TASKP_IMUAG_TYPE            PERIODIC_TASK
#define TASKP_IMUAG_STACKSIZE       2048
// IMU ACC/GYRO TASK
#define TASKP_IMUM_NAME             "Task IMU MAG"
#define TASKP_IMUM_TIME             0.004 //4 mseconds
#define TASKP_IMUM_PERIOD           IMU_MAG_PERIOD_TO_READ   //0.1 def
#define TASKP_IMUM_DEADLINE         TASKP_IMUM_PERIOD*2
#define TASKP_IMUM_PRIORITY         configMAX_PRIORITIES-3
#define TASKP_IMUM_TYPE             PERIODIC_TASK
#define TASKP_IMUM_STACKSIZE        1024
//TASKP GPS
//This task handles the
#define TASKP_GPS_NAME           	"Task GPS"
#define TASKP_GPS_PERIOD            GPS_ACQ_PERIOD    //300 sec=5 min
#define TASKP_GPS_TIME              GPS_ACQ_MAX_TIME
#define TASKP_GPS_DEADLINE       	TASKP_GPS_TIME*2
#define TASKP_GPS_PRIORITY       	configMAX_PRIORITIES-4  //4
#define TASKP_GPS_TYPE           	PERIODIC_TASK
#define TASKP_GPS_STACKSIZE      	2048
//TASKP RF TX DATA
#define TASKP_RFTX_NAME           	"Task RF TX DATA"
#define TASKP_RFTX_TIME           	0.90       //2023:06 meassure
#define TASKP_RFTX_PERIOD           RF_TX_DATA_PERIOD   //300 sec=5 min
#define TASKP_RFTX_DEADLINE       	TASKP_RFTX_PERIOD*2
#define TASKP_RFTX_PRIORITY       	configMAX_PRIORITIES-6  //5
#define TASKP_RFTX_TYPE           	PERIODIC_TASK
#define TASKP_RFTX_STACKSIZE      	2048
//TASKP RF KEEP ALIVE (SUb 1 GHz radio driver)
//This taks ensure the periodic sending of a keep alive
#define TASKP_RFKA_NAME           	"Task RF KA"
#define TASKP_RFKA_TIME           	0.019                  //tx ONLY 1 byte of ayloafd
#define TASKP_RFKA_PERIOD         	RF_KA_PERIOD                    //2.5
#define TASKP_RFKA_DEADLINE       	TASKP_RFKA_PERIOD*1.5  //
#define TASKP_RFKA_PRIORITY       	configMAX_PRIORITIES-5 //6
#define TASKP_RFKA_TYPE           	PERIODIC_TASK
#define TASKP_RFKA_STACKSIZE      	2048

//Tasks Aperiodics (TASKA)
//Task MICRO SD

#define TASKA_SD_NAME               "Task SD store"
#define TASKA_SD_TIME               0.08      //ARROUND 80 ms for 4096 bytes
#define TASKA_SD_PERIOD             0         //aperiodic, so 0.
#define TASKA_SD_PRIORITY           configMAX_PRIORITIES-2
#define TASKA_SD_TYPE               APERIODIC_TASK
#define TASKP_SD_STACKSIZE          2072


#define TASKA_ANIMALACT_NAME        "Task animal act."
#define TASKA_ANIMALACT_TIME        0.004                   //ARROUND 4 ms
#define TASKA_ANIMALACT_PERIOD      0                       //aperiodic, so 0.
#define TASKA_ANIMALACT_PRIORITY    configMAX_PRIORITIES-9
#define TASKA_ANIMALACT_TYPE        APERIODIC_TASK
#define TASKP_ANIMALACT_STACKSIZE   1048

//Tasks Aperiodics (TASKA)
//Task RF RECIEVE PARAMETERS

//----------------------TD
#define TASKA_RF_TD_RX_NAME             "Task RF TD Rx"
#define TASKA_RF_TD_RX_PERIOD           0
#define TASKA_RF_TD_RX_TIME             10     //ARROUND 80 ms
#define TASKA_RF_TD_RX_PRIORITY         configMAX_PRIORITIES-2
#define TASKA_RF_TD_RX_TYPE             APERIODIC_TASK
#define TASKA_RF_TD_RX_STACKSIZE        2072
//----------------------DSC
#define TASKA_RF_DSC_RX_NAME            "Task RF DSC Rx"
#define TASKA_RF_DSC_RX_PERIOD          0
#define TASKA_RF_DSC_RX_TIME            10     //ARROUND 80 ms
#define TASKA_RF_DSC_RX_PRIORITY        configMAX_PRIORITIES-2
#define TASKA_RF_DSC_RX_TYPE            APERIODIC_TASK
#define TASKA_RF_DSC_RX_STACKSIZE       2072

//----------------------------------------------------------------------------
//------------PRINTU SERVICE-------------------------------------
#ifdef RTOS_PRINTU
#define TASKA_PRINTU_NAME           "Task PRINTU"
#define TASKA_PRINTU_TIME           0.1       //0.1 is an stimation, one really cannot know due to the service nature
#define TASKA_PRINTU_PERIOD         0         //aperiodic, so 0.
#define TASKA_PRINTU_PRIORITY       1         //priority is 1, almost like iddle task
#define TASKA_PRINTU_TYPE           APERIODIC_TASK
#define TASKP_PRINTU_STACKSIZE      2048
#endif



#endif /* INC_FREERTOS_CUSTOM_DEFINES_H_ */
