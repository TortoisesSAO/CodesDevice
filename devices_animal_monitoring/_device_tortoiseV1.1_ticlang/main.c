

/*
 *  ======== main_tirtos.c ========
 */
#include <stdint.h>
/* RTOS header files */
#include <ti/drivers/Board.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>
/* Stack size in bytes */

/*
 *  ======== main ========
 */
void rtos_task_creation(void);

int main(void)
{
    Board_init();
    /* Initialize the attributes structure with default values */
    //init the drivers
    GPIO_init();
    I2C_init();
    //create tasks
    rtos_task_creation();
    //this point of the program should never be reached
    return (0);
}

