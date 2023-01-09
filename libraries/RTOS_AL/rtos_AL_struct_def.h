/*
 * freertos_custom_defines.h
 *
 *  Created on: 7 dic. 2021
 *      Author: Andres Oliva Trevisan
 */

#ifndef RTOS_AL_STRUCT_DEF_H_
#define RTOS_AL_STRUCT_DEF_H_

#include "rtos_AL_defines.h"

//structures
#ifndef TaskType_t
typedef enum
{
	PERIODIC_TASK,
	APERIODIC_TASK
}TaskType_t;
#endif
typedef struct{
	TaskHandle_t handle;      //task handler(pointer)
	uint8_t* stack_p;         //memory stack_pointer (65,536 bytes max)
	uint32_t stack_size;      //stacksize in bytes (65,536 bytes max)
	uint16_t i_tk;             //task fixed index inside main array
	TaskFunction_t pxTaskCode;//pointer to function of task
	void* pxStruct;           //pointer to custom structure
	char name[25];            //task name
	uint16_t type;     //type of task
	TickType_t p;     //Priority
	float c_s;        //Time to execute task (seconds)
	float t_s;        //Time to execute task (seconds)
	float d_s;        //Time to execute task (seconds)
	TickType_t c;     //Time (Worst-case execution time)/* Minimum inter-arrival for aperiodic */
	TickType_t t;     //Period
	TickType_t d;     //Deadline
    //***********************************************
	TickType_t r;     //Worst-case Response Time
    //here start the parameters related to schedulling algortimgs
	TickType_t cp;    //Polling server   (PS) capacity for Polling Server
	TickType_t cd;    //Deferal server   (DS) capacity for Deferal Server
	TickType_t y;     //Dual Priority    (DP) promotion time= d(Deadline)-t (time)
	TickType_t y_c;   //Dual Priority    (DP) promotion time counter
	BoolType_t y_b;   //Dual Priority    (DP) blocked state flag
	BoolType_t y_p_f; //Dual Priority    (DP) DP High Priotiry status:TRUE=is in high band, false is in low band
    uint8_t    y_p;   //Dual Priority    (DP) DP Promotion priority task high band
    TickType_t ki;    //k-planifcability (KI) index
    TickType_t ki_c;  //k-planifcability (KI) index counter
    TickType_t s;     //Slack Stealling  (SS) Counter
    TickType_t s_p;   //Slack Stealling  (SS) priority
}TaskData;

#endif /* INC_FREERTOS_CUSTOM_DEFINES_H_ */
