/*
 * freertos_custom_defines.h
 *
 *  Created on: 7 dic. 2021
 *      Author: Andres Oliva Trevisan
 */

#ifndef RTOS_TASK_PARAMS_H_
#define RTOS_TASK_PARAMS_H_
//default stack size
//TIMES ARE EXPRESSED IN seconds
// TEMP
#define TASKP_TEMP_NAME           "Task Temp"
#define TASKP_TEMP_TIME           0.002      //(seconds)
#define TASKP_TEMP_PERIOD         30.         //(seconds)
#define TASKP_TEMP_DEADLINE       60.         //(seconds)
#define TASKP_TEMP_PRIORITY       configMAX_PRIORITIES-10
#define TASKP_TEMP_TYPE           PERIODIC_TASK
// IMU ACC/GYRO TASK
#define TASKP_IMU_NAME           "Task IMU ACC/GYRO"
#define TASKP_IMU_TIME           0.002        //(seconds)
#define TASKP_IMU_PERIOD         1.067        //(seconds)
#define TASKP_IMU_DEADLINE       2.134        //(seconds)
#define TASKP_IMU_PRIORITY       configMAX_PRIORITIES-3
#define TASKP_IMU_TYPE           PERIODIC_TASK
// IMU ACC/GYRO TASK
#define TASKP_IMUM_NAME           "Task IMU MAG"
#define TASKP_IMUM_TIME           0.001 //(seconds)
#define TASKP_IMUM_PERIOD         0.1
#define TASKP_IMUM_DEADLINE       0.2
#define TASKP_IMUM_PRIORITY       configMAX_PRIORITIES-2
#define TASKP_IMUM_TYPE           PERIODIC_TASK
//TASKP GPS
//This task handles the
#define TASKP_GPS_NAME           "Task GPS"
#define TASKP_GPS_TIME           59.99
#define TASKP_GPS_PERIOD         60    //300 sec=5 min
#define TASKP_GPS_DEADLINE       150
#define TASKP_GPS_PRIORITY       configMAX_PRIORITIES-4
#define TASKP_GPS_TYPE           PERIODIC_TASK

//TASKP RF TX DATA
#define TASKP_RFTX_NAME           "Task RF TX"
#define TASKP_RFTX_TIME           0.1       //tx+rf
#define TASKP_RFTX_PERIOD         300    //300 sec=5 min
#define TASKP_RFTX_DEADLINE       600
#define TASKP_RFTX_PRIORITY       configMAX_PRIORITIES-5
#define TASKP_RFTX_TYPE           PERIODIC_TASK
//TASKP RF KEEP ALIVE (SUb 1 GHz radio driver)
//This taks ensure the periodic sending of a keep alive
//send a 7 bytes pulse at 4800 kbps *needs to be measured
#define TASKP_RFKA_NAME           "Task RF KA"
#define TASKP_RFKA_TIME           0.012        //tx ONLY
#define TASKP_RFKA_PERIOD         2.5
#define TASKP_RFKA_DEADLINE       4
#define TASKP_RFKA_PRIORITY       configMAX_PRIORITIES-6
#define TASKP_RFKA_TYPE           PERIODIC_TASK
//Tasks Aperiodics (TASKA)
//Task MICRO SD

#define TASKA_SD_NAME              "Task SD"
#define TASKA_SD_TIME              0.08      //ARROUND 80 ms
#define TASKA_SD_PERIOD            0
#define TASKA_SD_PRIORITY          configMAX_PRIORITIES-4
#define TASKA_SD_TYPE              APERIODIC_TASK

//Tasks Aperiodics (TASKA)
//Task RF RECIEVE PARAMETERS

#define TASKA_RFRP_NAME              "Task RF CONFIG"
#define TASKA_RFRP_TIME              100     //ARROUND 80 ms
#define TASKA_RFRP_PRIORITY          configMAX_PRIORITIES-4
#define TASKP_IMU_TYPE               PERIODIC_TASK



#endif /* INC_FREERTOS_CUSTOM_DEFINES_H_ */
