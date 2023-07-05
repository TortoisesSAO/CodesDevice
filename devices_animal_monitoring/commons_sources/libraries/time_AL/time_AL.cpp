/*/*
 * Author: Andres Oliva Trevisan
 * Supervised: Eng. Nicolas Catalano
 * Research Group:PhD Laila, Karina Laneri
 *
 */
// This library allows the user to compress tm standard

#include <stdint.h>
//#include <RTOS_AL/rtos_AL.h>
#include "time_AL.h"
/*allows you to convert a compressed time into an actual tm standard format*/
tm  convertlocaltime_uint32_to_tm(uint32_t time_compressed){
        struct tm Time_r ;
         struct tm *Time=&Time_r;
        /* time_compressed is the amount of years since 1900+ TIME_YEAR_COMPRESSION
         * while  pTime->tm_year corresponds to number of years since 1900,
         * so add TIME_YEAR_COMPRESSION to get the current value of the
        *  standard format
        *///max years to store:
        Time->tm_year =(((uint16_t)(time_compressed>>26)&255) + TIME_YEAR_OFFSET);//apply binary operations to obtain the requested bytes
        Time->tm_mon = ((uint8_t) (time_compressed>>22)&15);//apply binary operations to obtain the requested bytes
        Time->tm_mday= ((uint8_t) (time_compressed>> 17)&31);//apply binary operations to obtain the requested bytes
        Time->tm_hour= ((uint8_t) (time_compressed>> 12)&31);//apply binary operations to obtain the requested bytes
        Time->tm_min = ((uint8_t) (time_compressed>> 6)&63);//apply binary operations to obtain the requested bytes
        Time->tm_sec = ((uint8_t) (time_compressed<< 0)&63);//apply binary operations to obtain the requested bytes
       // if(((time_compressed)&1)>1){++Time->tm_sec;}//added for testing, does not change behaivour
        Time_r= *Time;
        return(Time_r);
 }
/*allows you to convert a actual tm standard format into a uint32_t compressed time */
uint32_t convertlocaltime_tm_to_uint32( tm Time){
   /*
    *  localtime() sets pTime->tm_year to number of years
    *  since 1900, so subtract TIME_YEAR_COMPRESSION to increase
    *  the amounts of years than can be stored in uint32_t date+year format
    */
    struct tm* pTime= &Time;
    uint32_t time_uint32 = ((uint32_t)(pTime->tm_year -TIME_YEAR_OFFSET) << 26)  |
       ((uint32_t)(pTime->tm_mon) << 22) |
       ((uint32_t)(pTime->tm_mday) << 17) |
       ((uint32_t)(pTime->tm_hour) << 12) |
       ((uint32_t)(pTime->tm_min) << 6) |
       ((uint32_t)(pTime->tm_sec) >> 0);
   return ((uint32_t)time_uint32);
}
/*return current date and time compressed in uint32_t format*/
uint32_t localtime_uint32(void){
        time_t seconds;
        uint32_t time_uint32;
        struct tm Time_r;
        struct tm *pTime;
        pTime=&Time_r;
        /*
         *  TI time() returns seconds elapsed since 1900, while other tools
         *  return seconds from 1970.  However, both TI and GNU localtime()
         *  sets tm tm_year to number of years since 1900.
         */
         seconds = time(NULL);
         pTime = localtime(&seconds);
         Time_r=*pTime;
        /*
         *  localtime() sets pTime->tm_year to number of years
         *  since 1900, so subtract TIME_YEAR_COMPRESSION to increase
         *  the amounts of years than can be stored in uint32_t date+year format
         */
         time_uint32=convertlocaltime_tm_to_uint32(Time_r);
          return ((uint32_t)time_uint32);

    }

tm  date_time_to_tm(uint16_t year,uint8_t month,uint8_t day,uint8_t hours,uint8_t minutes, uint8_t seconds){
    struct tm Time_r ;
    struct tm* Time=&Time_r;
    Time->tm_year =year-TIME_YEARS_SINCE;//1900 must be substracted due to time.h format
    Time->tm_mon = month- 1; //1 month must be subsctracted due to format.
    Time->tm_mday= day;
    Time->tm_hour= hours;
    Time->tm_min = minutes;
    Time->tm_sec = seconds;//apply binary operations to obtain the requested bytes
    Time_r=*Time;
    return(Time_r);
}

void localtime_set_date_time(uint16_t year,uint8_t month,uint8_t day,uint8_t hours,uint8_t minutes, uint8_t seconds){
    struct tm Time_r ;
    struct tm* Time=&Time_r;
    Time->tm_year =year-TIME_YEARS_SINCE;//1900 must be substracted due to time.h format
    Time->tm_mon = month- 1; //1 month must be subsctracted due to format.
    Time->tm_mday= day;
    Time->tm_hour= hours;
    Time->tm_min = minutes;
    Time->tm_sec = seconds;//apply binary operations to obtain the requested bytes
    Time_r=*Time;
    //now get the seconds elapsed by invoking the mktime function and use our custom function to set time
    localtime_set_elapsed_seconds(mktime(Time));//added for testing, does not change behaivour
}

void localtime_set_elapsed_seconds(uint32_t seconds_elapsed){
    struct timespec ts;           // Set this to the current UNIX time in seconds
    ts.tv_sec=seconds_elapsed;   //
    clock_settime(CLOCK_REALTIME, &(ts));//this is used to assign a creation data
}

/*converts astandard tm date and time into a string in format:1984-04-29 23:56:52 *///time_compressed=142458650
uint8_t date_time_tm_to_string(char* str,tm *ltm){
strftime(str, TIME_STRING_LENGTH,"%Y-%m-%d %X",ltm);
return((uint8_t)TIME_STRING_LENGTH);
}
/*converts a compressed date and time into a string in format:1984-04-29 23:56:52 *///time_compressed=142458650
uint8_t date_time_uint32_to_string(char* str,uint32_t time_compressed){
struct tm Time_r=convertlocaltime_uint32_to_tm(time_compressed);
struct tm *ltm = &Time_r;
strftime(str, TIME_STRING_LENGTH,"%Y-%m-%d %X",(const struct tm*)ltm);
return((uint8_t)TIME_STRING_LENGTH);
}

/*convert elapsed seconds in a day to 00:00:00 string */
uint8_t day_time_in_seconds_to_string(char* str,uint32_t time){
        uint8_t seconds, hours, minutes;
        hours = (time/3600);
        str[0]=hours/10+'0';str[1]=hours%10+'0';str[2]=':';
        minutes = (time -(3600*hours))/60;
        str[3]=minutes/10+'0';str[4]=minutes%10+'0';str[5]=':';
        seconds = (time -(3600*hours)-(minutes*60));
        str[6]=seconds/10+'0';str[7]=seconds%10+'0';
        str[DATE_TIME_STRING_LENGTH]='\0';//for compatibility
 return(DATE_TIME_STRING_LENGTH);
}
uint32_t get_elapsed_seconds_in_current_day(void){
    uint32_t elapsed_seconds;
    time_t rawtime;
    struct tm* ptime;
    time(&rawtime);
    ptime = localtime(&rawtime);
    elapsed_seconds=((ptime->tm_hour)*3600+(ptime->tm_min)*60+(ptime->tm_sec));
    return(elapsed_seconds);
}
uint8_t get_date_as_8long_string(char* str){
    time_t rawtime;
    struct tm* ptime;
    time(&rawtime);
    ptime = localtime(&rawtime);
    uint8_t  year = (ptime->tm_year+TIME_YEARS_SINCE-2000);
    uint8_t  month= ptime->tm_mon+1;
    uint8_t  day  =  ptime->tm_mday ;
    str[0]='2';//the date is 20 for the following 77 years
    str[1]='0';
    str[2]=(year / 10  +48);
    str[3]=(year % 10  +48);
    str[4]=(month / 10 +48);
    str[5]=(month % 10 +48);
    str[6]=(day / 10   +48);
    str[7]=(day % 10   +48);
return(8);
}
//strftime(buffer, 80,"\r\n  %Y/%m/%d %X UTC",timeinfo); to print

//-----------------------------------c file--------------------------------------------------
//example to run into online compiler
/*
 *******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

/*#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <iostream>
using namespace std;
#define TIME_YEAR_COMPRESSION 100
  void bin(uint32_t n)
  {
    uint32_t i;
    cout << "0";
    for (i = 1 << 30; i > 0; i = i / 2)
    {
      if((n & i) != 0)
      {
        cout << "1";
      }
      else
      {
        cout << "0";
      }
    }
  }

//time_t as an integral value representing the number of seconds elapsed since 00:00 hours, Jan 1, 1970 UTC (i.e., a unix timestamp). Although libraries may implement this type using alternative time representations.
int main()
{
    time_t rawtime;
    struct tm* timeinfo;
    struct tm time_uncromp;
    uint32_t time_cromp;
    // Used to store the time
    // returned by localetime() function
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 80,"\r\n  %Y/%m/%d %X UTC",timeinfo);

    // strftime() function stores the
    // current time as Hours : Minutes
    //%I %M and %p-> format specifier
    // of Hours minutes and am/pm respectively

    // prints the formatted time
    puts(buffer);
    time_cromp=localtime_uint32();
    time_uncromp=time_uint32_to_tm(time_cromp);
    printf("ok");
    printf("time is %i \r\n",(int32_t)time_cromp);
    strftime(buffer, 80," %Y/%m/%d %X UTC",&time_uncromp);
    puts(buffer);
    return 0;
}

*/




