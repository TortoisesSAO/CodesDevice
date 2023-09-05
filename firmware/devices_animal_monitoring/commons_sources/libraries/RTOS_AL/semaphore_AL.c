/*
 *  ======== sempahore_mutex.c ========
 */

#include <RTOS_AL/semaphore_AL.h>

//Functions
bool semaphore_trytotake_time(sempaphore_struct* semaphore, uint32_t timeout)
{
#ifdef RTOS_TIRTOS
return(Semaphore_pend(semaphore->driver.handle,timeout));//Could not take the sempahore

#endif
#ifdef RTOS_POSIX
int sem_val;
struct timespec abstime;
abstime.tv_sec=0;//seconds
abstime.tv_nsec= (long) (timeout * SEMPAHORE_TIME_SCALE);// convert us to nanosecojnds
sem_val=sem_timedwait(&(semaphore->driver.sem_struct),&(abstime));
if (sem_val==-1)//semaphore was taken
     {return(true);}//Could take the sempahore
return(false);//Could not take the sempahore
#endif
}

bool semaphore_trytotake(sempaphore_struct* semaphore){
//uint32_t timeout=semaphore->time_out;
//Semaphore_Handle handle= semaphore->driver.handle;
bool ret=false;
#ifdef RTOS_FREERTOS
	xSemaphoreTake(semaphore_shared, MUTEX_TIMEOUT );}//gives permision to other app to use semaphore
#endif
#ifdef RTOS_TIRTOS
ret=Semaphore_pend(semaphore->driver.handle,semaphore->time_out);//Could not take the sempahore
return(ret);
}
#endif
#ifdef RTOS_POSIX
int sem_val;
clock_gettime(CLOCK_REALTIME, &(semaphore->driver.abstime));
sem_val=sem_timedwait(&(semaphore->driver.sem_struct),&(semaphore->driver.abstime));
if (sem_val==-1)//semaphore was taken
     {return(true);}//Could take the sempahore
return(false);//Could not take the sempahore
}
#endif

void semaphore_release(sempaphore_struct* semaphore)
{
#ifdef RTOS_FREERTOS
    xSemaphoreGive( xMutex );}//gives permision to other app to use semaphore
#endif
#ifdef RTOS_TIRTOS
Semaphore_post(semaphore->driver.handle);
#endif
#ifdef RTOS_POSIX
sem_post(&(semaphore->driver.sem_struct));//gives permision to other app to use semaphore
#endif

}

void semaphore_init(sempaphore_struct* semaphore,uint32_t time_out)
{
	semaphore->time_out=time_out;
#ifdef RTOS_FREERTOS
	static SemaphoreHandle_t semaphore_shared;
	semaphore_shared = xSemaphoreCreateBinary();
#endif
#ifdef RTOS_TIRTOS
	//creates the semaphore and set up
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&(semaphore->driver.sem_struct), 1, &semParams);
    semaphore->driver.handle = Semaphore_handle(&(semaphore->driver.sem_struct));
#endif
#ifdef RTOS_POSIX
    semaphore->driver.abstime.tv_sec=0;/* seconds */
    semaphore->driver.abstime.tv_nsec= (long) ((time_out)*SEMPAHORE_TIME_SCALE);/* convert ms to nanosecojnds */
    if (sem_init(&(semaphore->driver.sem_struct), 0, 0) != 0) { ; } /* Error creating semaphore */
#endif
//sets the timeout
    semaphore->time_out= time_out;
}



