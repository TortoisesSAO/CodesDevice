#ifndef TIME_AL_H_
#define TIME_AL_H_

#define TI_CC13x2
//-----------------header--------------------------------------------------
#define TIME_YEAR_OFFSET         80  //substracs 80 years to make FAT-SD format compatible and increase the amoun of years to store
                                  //This was made in order to be compatible with FAT library
#define TIME_YEARS_SINCE         1900 //Depending on the compiler, the elapsed seconds are calculated since 1900 for TI/GNU or 1970 by other tools
#define DATE_TIME_STRING_LENGTH  8
#define TIME_STRING_LENGTH       20
#define SECONDS_IN_A_DAY 86400  // 3600 seconds*24 hours=86400
#include <time.h>

//#ifdef __cplusplus
//extern  "C" {
//#endif
     /* typedef struct{
              uint8_t          tm_sec;
              uint8_t          tm_min;
              uint8_t          tm_hour;
              uint8_t          tm_mday;
              uint8_t          tm_wday;
              uint8_t          tm_mon;
              uint16_t         tm_year;
              uint16_t         tm_yday;
              int16_t          tm_isdst;
          }tm;*/
    /*allows you to convert a uint32_t compressed time into an actual tm standard format*/
    tm              convertlocaltime_uint32_to_tm(uint32_t time_compressed);
    /*allows you to convert a actual tm standard format into a uint32_t compressed time */
    uint32_t        convertlocaltime_tm_to_uint32( tm Time);
    /*get a tm structure from time_parameters*/
    tm              date_time_to_tm(uint16_t year,uint8_t month,uint8_t day,uint8_t hours,uint8_t minutes, uint8_t seconds);
    /*set the curremt date and time for the microcontroller unit*/
    void            localtime_set_date_time(uint16_t year,uint8_t month,uint8_t day,uint8_t hours,uint8_t minutes, uint8_t seconds);
    /*set the curremt date and time for the microcontroller unit by using the total elapsed seconds*/
    void            localtime_set_elapsed_seconds(uint32_t seconds_elapsed);

    /*returns current day and time compressed in uint32 format */
    uint32_t        localtime_uint32(void);
    //strftime(buffer, 20,"\r\n  %Y-%m-%d %X UTC",timeinfo);// to print . Reminder: tm* timeinfo;
    /*return amounts of seconds elapsed in a day*/
    uint32_t        get_elapsed_seconds_in_current_day(void);
    /*convert elapsed seconds in a day to 00:00:00 string */
    uint8_t day_time_in_seconds_to_string(char* str,uint32_t time);
    uint8_t get_date_as_8long_string(char* str);
    /*convert compressed day and time to to 2023-00-00 00:00:00 string */
    uint8_t date_time_uint32_to_string(char* str,uint32_t time_compressed);
    uint8_t date_time_tm_to_string(char* str,tm *ltm);

//#ifdef __cplusplus
//  }
// #endif

#endif

