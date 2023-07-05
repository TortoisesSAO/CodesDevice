
#include <device_system_setup.h>
#ifdef RTOS_PRINTU
//for rtos utilities
#include <RTOS_AL/rtos_AL.h>
#include "task_uart_printu.h"    //for PRINTu structures
#include <stdarg.h>  //for variable arguments
#include <string.h>//for memcpy ussage
#include <stdio.h>//to allow sprintf

typedef struct {
	char 				 c[PRINT_MAX_CHARS_PER_QUEUE];
	uint16_t             c_amount; //amount of characters to print
}string_struct;
typedef struct {
    string_struct 				s[PRINT_QUEUE_LIST_SIZE];   //string to print
	uint8_t			    queue_pos;//               //amount of data to write into sd file
}print_array;


static sempaphore_struct sem_print_queue_add;    //semaphore for adding element to list to print
#define SEM_PRINT_QUEUE_ADD_TIMEOUT 1

static sempaphore_struct* psem_uart0=NULL;     //for interacting with external sempaphores
static sempaphore_struct sem_uart0;
#define SEM_PRINT_TIMEOUT 1
static sempaphore_struct sem_print_wait;      //for internal use

//--------------------------
static string_struct to_print_next;
static char          str_new[500]; //for mem protection, declare in this order
static print_array   print_list;
static int      amount_of_chars;
static va_list       vl;
//*-------------------------
const char           overflow_message[]="!PRINT OVERFLOW!";
//-------------
uart_struct uart_print;
uart_struct* puart_print;
//-------------------
extern "C"	{
void task_uart0_printu(void* ptask_data);
void task_uart0_printu_print( void* ptask_data);

}
void printu_give_shared_uart_semaphore(sempaphore_struct* psem_uart){
    psem_uart0=psem_uart;
}
void task_uart0_printu_print(void* ptask_data){
 int i;
   while(1){
            if (semaphore_trytotake(&sem_print_queue_add)){//copy value to print buffer
               if (print_list.queue_pos>0){
                  to_print_next=print_list.s[0];//copy element to print list
                   //to_print_next.c_amount=print_list.s[0].c_amount;
                  // memcpy(to_print_next.c, print_list.s[0].c,to_print_next.c_amount);//copy from qlist to print buffer
                 --print_list.queue_pos;//decrement position to tell that the order to print has been raised
                 //if there are elements to print, move all elemements 1 position below the array
                 if (print_list.queue_pos>0){ for(i=0;i<print_list.queue_pos;i++){print_list.s[i]=print_list.s[i+1];}}
                 //release the resource
                 semaphore_release(&sem_print_queue_add);
                 //print using uart port
                 if(semaphore_trytotake(psem_uart0)){
                    uart_init(puart_print);//init uart if not init
                    uart_send(puart_print,to_print_next.c,to_print_next.c_amount);
                    uart_close(puart_print);//closes uart after printing all the elements
                    semaphore_release(psem_uart0);
                    }
                }
                else{//no elements to print, release the queue  reosurce and exit loop
                 semaphore_release(&sem_print_queue_add);
                 break;}
            }
        }
}
void task_uart0_printu(void* ptask_data){\
//init internal sempahore for quee add to list
semaphore_init(&sem_print_queue_add,SEM_PRINT_QUEUE_ADD_TIMEOUT);
print_list.queue_pos=0;//init the print list
//init uart module (but without turning it ON)
puart_print=&uart_print;
uart_set(puart_print,UART0_PRINT_PORT_NUMBER,UART0_PRINT_BAUD_RATE,UART0_PRINT_MODE,NULL);
if (psem_uart0==NULL){//If you did not provide the sem pointer, it will init itself
    semaphore_init(&sem_uart0,SEM_PRINT_TIMEOUT);
    psem_uart0=&sem_uart0;
}
//init internal sempaphore to print each quee element
semaphore_init(&sem_print_wait,SEM_WAIT_FOREVER);
semaphore_trytotake(&sem_print_wait);//take the resource to avoid issuies with the task the resource
printuf_nortos((const char*)"\r\nPrintu task succesfully started. Awating messages to print...");
printuf_nortos((const char*)"\r\nIMPORTANT: \"printuf(..)       \" function print up to %i characters ONLY", PRINT_MAX_CHARS_PER_QUEUE);
printuf_nortos((const char*)"\r\n --->WHile \"printuf_nortos(..)\" function print up to %i characters, but it must NEVER be called after THIS point (which is once RTOS was started)", sizeof(str_new));
printuf_nortos((const char*)"\r\n*******************************************************************", sizeof(str_new));
printuf_nortos((const char*)"\r\n*******************************************************************", sizeof(str_new));
while(1){
         semaphore_trytotake(&sem_print_wait);//awaits a resource release to write
         task_uart0_printu_print((TaskData*) ptask_data);
}
}
/*this functions carries out the print instruction when no RTOS is enabled*/
void printu_exec(int amount_of_chars) {
puart_print=&uart_print;
uart_set(puart_print,UART0_PRINT_PORT_NUMBER,UART0_PRINT_BAUD_RATE,UART0_PRINT_MODE,NULL);
uart_init(puart_print);//init uart if not init
uart_send (puart_print,str_new,amount_of_chars);
uart_close(puart_print);//closes uart after print
}
/*NEVER CALL THIS ONCE RTOS IS ON print function to uart port to be called by the user*/
void printuf_nortos(const char *str,...){
    int      amount_of_chars;
    va_list       vl;
    va_start( vl, str );//str_new
    amount_of_chars= vsprintf (str_new, str,  vl ); //to test if sprintf does not
    va_end(vl);
    if (amount_of_chars>sizeof(str_new)){
        amount_of_chars=sizeof(str_new);
        memcpy(&str_new[sizeof(str_new)-sizeof(overflow_message)-2],overflow_message,sizeof(overflow_message));//copy from pMem_to_store to microsd_write_buffer
        }
    printu_exec(amount_of_chars);
}
/*print function to uart port to be called by the user*/
extern void printuf(const char *str,...){
    //check if task for nonblocking printing was created, otherwise, print as default
    if (semaphore_trytotake(&sem_print_queue_add)){
        //parse the function //if this is part is putted inside sempahore, chrash happens anyway
        va_start( vl, str );//str_new
        amount_of_chars= vsprintf (str_new, str,  vl ); //to test if sprintf does not
        va_end(vl);
        if (amount_of_chars>PRINT_MAX_CHARS_PER_QUEUE){amount_of_chars=PRINT_MAX_CHARS_PER_QUEUE;} //ensure non buffer overlap
        //proceed to add the element to the quee
	    if (print_list.queue_pos<(PRINT_QUEUE_LIST_SIZE)){//check if there is space in the list to append elements
            print_list.s[(print_list.queue_pos)].c_amount= amount_of_chars;
		    memcpy(print_list.s[print_list.queue_pos].c, str_new,print_list.s[print_list.queue_pos].c_amount);//copy from qlist to print buffer
            ++(print_list.queue_pos);
            if (print_list.queue_pos>0){//there are elements to print-->call print functiopn
                semaphore_release(&sem_print_queue_add);//release the resource before allowing to print
                semaphore_release(&sem_print_wait);//release the resource to allow printing
            }else{ semaphore_release(&sem_print_queue_add);} //release the resource
	    }else{ semaphore_release(&sem_print_queue_add);} //release the resource
	  }
}
#endif
