
#define configSS_ASSERT_EQUAL( x, y ) if( ( x ) != ( y ) ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

#define configSS_ASSERT_GREATHER_OR_EQUAL( x, y ) if( ( x ) < ( y ) ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

/* ========================================================================= */
/* Required for integrating the SsTCB into the task TCB. */
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 1

/* Add functionality to be added to FreeRTOS's tasks.c source file. */
#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H 1

/* Executes vSlackSchedulerSetup() at scheduler initialization. */
#define FREERTOS_TASKS_C_ADDITIONS_INIT() vSlackSchedulerSetup()

/* Required for identify the IDLE task in slacks methods and deadline check. */
#define INCLUDE_xTaskGetIdleTaskHandle  1

/*
 * Slack methods available:
 * 0 = Fixed
 * 1 = Davis
 */
#define configUSE_SLACK_STEALING        1 /* 1: Use slack stealing methods, 0: No slack. */
#define configUSE_SLACK_METHOD          0 /* Slack method to use */
#define configUSE_SLACK_K               0 /* Only calculate slack at the scheduler start */
#define configMAX_SLACK_PRIO            1 /* priority levels that are used for slack. */
#define configMIN_SLACK_SD              1 /* Minimum amount of available slack. */
/* ========================================================================= */
