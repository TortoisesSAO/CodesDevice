#ifndef SEMAPHORE_AL_H_
#define SEMAPHORE_AL_H_

#include "rtos_AL_defines.h" //for defines
#include "rtos_AL_config.h"
#include <stdbool.h> //for bool use

#define SEM_WAIT_FOREVER       TASK_SLEEP_FOREVER
/* Driver Header files */
#ifdef RTOS_TIRTOS
//#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>//for bios wait for ever
#include <ti/sysbios/knl/Clock.h>//for Clock_tickPeriod

#define SEMPAHORE_TIME_SCALE             1
//structure used
typedef struct{
Semaphore_Struct sem_struct;//original structure must be defined inside in order to make this wotrk
Semaphore_Handle handle; //handler
}
sempaphore_driver;
#endif
//
#ifdef RTOS_POSIX
//#include <pthread.h>
#include <semaphore.h>//for posix semaphore
#include <time.h>//for posix semaphore
#include <stdbool.h> //for using sleep and nanosleep
#define SEMPAHORE_TIME_SCALE             1000
//structure used
typedef struct{
sem_t sem_struct;//original structure must be defined inside in order to make this wotrk
struct timespec abstime;
}
sempaphore_driver;
#endif
//COMMON STRUCTURE
typedef struct{
sempaphore_driver driver;  //driver values, like handle, ect
uint32_t time_out;// wait time in us
}sempaphore_struct;
//functions
//this function will init the sempahore

#ifdef __cplusplus
  extern "C" {
#endif
void semaphore_init(sempaphore_struct* semaphore,uint32_t time_out);
//This function will attempt to take the semaphore after the default timeout or return fail
bool semaphore_trytotake(sempaphore_struct* semaphore);
//This function will attempt to take the semaphore after the passed timeout or return fail
bool semaphore_trytotake_time(sempaphore_struct* semaphore, uint32_t timeout);
//This function will release the sempahore
void semaphore_release(sempaphore_struct* semaphore);

#ifdef __cplusplus
   }
 #endif
#endif /*  */
