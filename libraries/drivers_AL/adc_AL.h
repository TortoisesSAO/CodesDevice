#ifndef ADC_AL_H_
#define ADC_AL_H_


/* Driver Header files */
#include <ti/drivers/ADC.h> //to acces the TI driver
/* TI driver chacteristics */
#define ADC_BITS_AMOUNT            4095   //=2^12-1not used, just to remind the hardware setup
#define ADC_REF_VOLTAGE            1.49  //not used, just to remind the hardware setup
/*Used macros*/

/* Max unit to store the result after a conversion.
 * This driver is set up for a maximum input of 64.365 V considering scaling liker dividers
 * If you have bigger inputs, change to uint32_t*/
#define adc_uint_t uint16_t
/* Macro to convert input values from float to unsigned integer, and applies an scale factor
 * This factor is applied in order to perform a conversion from bits to mV*/
#define TO_UINT(f)                (adc_uint_t)((f)*(1000))
/*--------------------------------------------------------------
 *Structures
 *--------------------------------------------------------------*/
typedef struct{
ADC_Handle      handle;//handle of the driver channel
uint8_t         pinout;//channel selected of the ADC
}
adc_driver;
//----------------------------------------------------------------
typedef struct{
adc_driver      driver;  //driver values, like handle, ect
float      input_scale;//scales the input in order to
							//This is addiontal scale external to the MCU (example:a divider resistor bridge)
uint16_t        result_bits;//result in bytes of the adc conversion
adc_uint_t      result_mV;
}
adc_struct;
/*--------------------------------------------------------------
 *Functions
 *--------------------------------------------------------------*/
#ifdef __cplusplus
  extern "C" {
#endif
/* Performs an ADC acquisition, storing the result in the structure
 * returns the result of the conversion (fail/succes) */
int32_t adc_get(adc_struct* adc);
/* Converts a value of an specific channel from bits to mV */
void adc_to_mv(adc_struct* adc);
/* Initialize an ADC driver & an specific ADC channel */
void adc_init(adc_struct* adc_p);
/* Initialize an ADC driver & an a group of ADC channels */
void adc_init_array(adc_struct* adc_array[], uint8_t adc_array_amount);
/* Initialize an ADC driver */
// void adc_init_driver(void); //for internal use, leave this commentes
#ifdef __cplusplus
   }
 #endif
#endif
