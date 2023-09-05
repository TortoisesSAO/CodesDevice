/*
 * freertos_custom_defines.h
 *
 *  Created on: 7 dic. 2021
 *      Author: Andres Oliva Trevisan
 */

#ifndef RTOS_AL_CONFIG_H_
#define RTOS_AL_CONFIG_H_

//MACROS USED
//#define     RTOS_FREERTOS        1 //using FREE-RTOS
#define       RTOS_TIRTOS          1 //using TI-RTOS
//#define     RTOS_POSIX           1 //using posix layer
//#define       RTOS_POSIX_AND_OTHER 1 //using posix layer+RTOS to optimize some of the functions
//PRE COMPILER WARNING, /*IF YOU ADD NEW RTOS, UPDATE THIS*/
#if defined(RTOS_FREERTOS)+defined(RTOS_TIRTOS)+defined(RTOS_POSIX)>1
      !!!COMPILE ERRROR: YOU CAN ONLY 1 REAL TIME OPERATIVE SYSTEM AT THE SAME TIME
#endif

//**************************************************************************************
//***GENERAL RTOS DEBUGGING UTILITIES
//#define RTOS_PRINT_DEBUG  //allow to use debug printing library
//***GENERAL FREERTOS & ECLIPSE BASED IDE DEBUGGING UTILITIES
//#define RTOS_VIEWER       //allow to use RTOS VIEWER plugging
//#define RTOS_TRACEALYZER  //allow to use RTOS to use Percepcio Trazealazer plugging
//***GENERAL TIRTOS/POSIX & CODE COMPOSER STUDIO 11 BASED IDE DEBUGGING UTILITIES
//GENERAL UTILITIEXS
//#define RTOS_EAT_CPU //allow the use of eat cpu function for simulation purposes
//**********************************************************
//****ONLY DEFINE ONE SCHEDULLER AT THE SAME TIME****
//NOT IMPEMENTED#define RTOS_PS 1//Allow the use of Polling Server as Scheduler.
//NOT IMPEMENTED#define RTOS_DS 1//Allow the use of Deferal Server as Scheduler.
//NOT IMPEMENTED#define RTOS_DP 1//Allow the use of Dual Priority as Scheduler.
//NOT IMPEMENTED#define RTOS_KI 1//Allow the use of K-Planificability as Scheduler
//              #define RTOS_SS 1//Allow the use of Slack Stealing as Scheduler
//****ONLY DEFINE ONE SCHEDULLER AT THE SAME TIME****
//----->SCHEDULLERS DEPENDECIES ENABLES
	               #ifdef RTOS_SS
                      #define RTOS_SSS //Slack Stealing library related
                  //#define RTOS_SS_DEBUG  //For slack stealing debugging printing *ensure RTOS_PRINT_DEBUG is defined*
                   #endif
//**************************************************************************************
///------------PRE COMPILER WARNINGS

#if defined(RTOS_PS)+defined(RTOS_DS)+defined(RTOS_DP)+defined(RTOS_KI)+defined(RTOS_SS)>1
      !!!COMPILE ERRROR: YOU CAN ONLY HAVE ONE SCHEDULLER AT THE SAME TIME
#endif
#if defined(RTOS_SS_DEBUG)&& !defined(RTOS_PRINT_DEBUG)
      !!!COMPILE ERRROR: DEFINE "RTOS_PRINT_DEBUG" FOR USE "RTOS_SS_DEBUG"
#endif
#endif /* RTOS_AL_CONFIG_H_ */
