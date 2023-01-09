/*
 * uart_driver.c
 *
 *  Created on: 4 jul. 2022
 *      Author: Nicro
 */
#include "uart_printu.h"  //

#ifdef PRINTU_ENABLED

 #include <stdio.h>   //for printing
 #include <stdarg.h>  //for printing
 #include <string.h>  //for printing

static uart_struct* uart_p //print debug uart pointer
int printu_start(uart_struct* uart_pointer){
	uart_p=uart_pointer;
}
int printu(const char * str, ...){
//BEGIN OF STRING/CHAR ARRAY PARSER
	va_list vl;
		int i = 0, j=0;
			char buff[100]={0}, tmp[20];
			va_start( vl, str );
			while (str && str[i])
			{
			  	if(str[i] == '%')
			  	{
	 		    i++;
	 		    switch (str[i])
	 		    {
		 		    case 'c':
		 		    {
		 		        buff[j] = (char)va_arg( vl, int );
		 		        j++;
		 		        break;
		 		    }
		 		    case 'd':
		 		    {
		 		        itoa(va_arg( vl, int ), tmp, 10);
		 		        strcpy(&buff[j], tmp);
		 		        j += strlen(tmp);
			           break;
			        }
			        case 'x':
			        {
			           itoa(va_arg( vl, int ), tmp, 16);
			           strcpy(&buff[j], tmp);
			           j += strlen(tmp);
			           break;
			        }
	        	}
	     	}
	     	else
		    {
		       	buff[j] =str[i];
		       	j++;
		    }
		    i++;
		}
		va_end(vl);
		//END OF STRING/CHAR ARRAY PARSER
		//--------------------------------
		//HERE GOES YOUR PRINT "buff" FUNCTION
		//UART send characters function
        #ifdef PRINTU_UART
		uart_send(uart_p,buff,sizeof(buff));//uart_print MUST BE DEFINED
        #endif
		//Default print, select to turn on
        #ifdef PRINTU_DEFAULT
		printf(buff);
        #endif
		//HERE GOES YOUR PRINT "buff" FUNCTION
	    return j;//RETURN THE J
}

#endif /*  */


