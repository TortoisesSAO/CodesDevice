
#include <device_system_setup.h>
#include "device_system_information.h"
#if (DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO)
#if  (DEVICE_BATTERY==BAT_LIPO_3700mV_600mAh)
#include <unistd.h>
#define BAT_LEVEL_MAX_mV 4200
#define BAT_LEVEL_MIN_mV 2700
typedef struct
{
    uint16_t bat_level_mv;
    uint8_t  bat_level_percent;
} __attribute__((packed)) bat_level_percent_table_Entry;

extern uint8_t bat_level_percent_table_find_value( uint16_t bat_level_mv);

//----------------------------c file comptabile------------------------------------------
// table construct. Must contain all the possible listed devices
/*table for BAT_LIPO_3700mV_600mAh*/
const bat_level_percent_table_Entry bat_level_table[] ={
//TORTOMETER HARDWARE V2.X (MCU CC131X) LISTED WORKING DEVICES
  //ID (MAC ADDRESS FOR CC1312R)|ASIGNED NUMBER OF DEVICE
        {BAT_LEVEL_MAX_mV,100},
        {4177,99},
        {4154,98},
        {4130,97},
        {4107,96},
        {4084,95},
        {4061,94},
        {4045,93},
        {4035,92},
        {4025,91},
        {4015,90},
        {4006,89},
        {3996,88},
        {3986,87},
        {3976,86},
        {3966,85},
        {3956,84},
        {3946,83},
        {3936,82},
        {3926,81},
        {3916,80},
        {3906,79},
        {3896,78},
        {3886,77},
        {3878,76},
        {3869,75},
        {3860,74},
        {3850,73},
        {3840,72},
        {3830,71},
        {3820,70},
        {3810,69},
        {3800,68},
        {3792,67},
        {3785,66},
        {3777,65},
        {3770,64},
        {3763,63},
        {3756,62},
        {3748,61},
        {3740,60},
        {3731,59},
        {3722,58},
        {3713,57},
        {3704,56},
        {3695,55},
        {3689,54},
        {3683,53},
        {3676,52},
        {3670,51},
        {3664,50},
        {3658,49},
        {3652,48},
        {3648,47},
        {3643,46},
        {3638,45},
        {3634,44},
        {3629,43},
        {3624,42},
        {3621,41},
        {3617,40},
        {3614,39},
        {3610,38},
        {3606,37},
        {3603,36},
        {3599,35},
        {3596,34},
        {3592,33},
        {3589,32},
        {3585,31},
        {3581,30},
        {3578,29},
        {3572,28},
        {3565,27},
        {3559,26},
        {3553,25},
        {3547,24},
        {3541,23},
        {3534,22},
        {3528,21},
        {3522,20},
        {3516,19},
        {3510,18},
        {3503,17},
        {3497,16},
        {3489,15},
        {3481,14},
        {3472,13},
        {3464,12},
        {3456,11},
        {3448,10},
        {3436,9},
        {3424,8},
        {3411,7},
        {3398,6}, //below this point is time to start sleep
        {3332,5},//calculated manually
        {3266,4},//calculated manually
        {3200,3},//calculated manually
        {3030,2},//calculated manually
        {2870,1},//calculated manually
        {BAT_LEVEL_MIN_mV,  0},//critical point
};

extern uint8_t bat_level_percent_table_find_value(uint16_t bat_level_mv){
    // Local variable stores an invalid value. //
    //Iterate through the power table. We do not verify against nullptr. ////
    uint8_t i;
          for (i=0; i<255; i++){ //because we are using % with a resolution
            if (bat_level_table[i].bat_level_mv < bat_level_mv) {
                break; //the lower-or-equal criterion toward the input argument. //
               }
            else {
                if (bat_level_table[i].bat_level_mv==BAT_LEVEL_MIN_mV){break; }//reach end of table
            }
        }
        return(bat_level_table[i].bat_level_percent);//return the lowest close value
 }
#endif
#endif

// for lizzards
#if (DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD)

#include <unistd.h>
#define BAT_LEVEL_MAX_mV 1 //for testing
#define BAT_LEVEL_MIN_mV 0 //for testing
typedef struct
{
    uint16_t bat_level_mv;
    uint8_t  bat_level_percent;
} __attribute__((packed)) bat_level_percent_table_Entry;

extern uint8_t bat_level_percent_table_find_value( uint16_t bat_level_mv);

//----------------------------c file------------------------------------------
// table construct. Must be built according to your selected battery
const bat_level_percent_table_Entry bat_level_table[] ={
        {3300,100},
        {3200, 90},
        {0,  0},
};
extern uint8_t bat_level_percent_table_find_value(uint16_t bat_level_mv){
    // Local variable stores an invalid value. //
    uint8_t bat_level=bat_level_table[0].bat_level_percent;//added to remove warning
    bat_level=(uint8_t)((bat_level_mv/33));//to get an equivalent of percet due to the fact there is no tabnle
    //Iterate through the power table. We do not verify against nullptr. ////
        return(bat_level);//return the linear percent for a 3.3V source
 }

#endif
