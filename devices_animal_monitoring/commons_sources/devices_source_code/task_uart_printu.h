
#ifndef _TASK_PRINTU_H_
#define _TASK_PRINTU_H_
#include <device_system_setup.h>
#ifdef RTOS_PRINTU
#include <stdarg.h>// for multiple arguments passing]
// /https://stackoverflow.com/questions/3530771/passing-variable-arguments-to-another-function-that-accepts-a-variable-argument
#include "drivers_AL/uart_AL.h"
#include <RTOS_AL/semaphore_AL.h> //allow the use of an operative system

#define UART0_PRINT_PORT_NUMBER                    uart_mu0
#define UART0_PRINT_BAUD_RATE                      115200
#define UART0_PRINT_MODE                           UART_BLOCKING
#ifdef __cplusplus
  extern "C" {
#endif

void printu_give_shared_uart_semaphore(sempaphore_struct* psem_uart);
extern void printuf(const char *str, ...);
void printuf_nortos(const char *str, ...);
#ifdef __cplusplus
   }
 #endif
#endif
#endif

