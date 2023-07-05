/*

 */

#include "adc_AL.h"
#include <stdio.h>//to include null
//--------------------------
bool flag_driver_init=false;
void adc_init_driver(void){
    if (flag_driver_init==false){
    ADC_init();
    flag_driver_init=true;
    }
}
void adc_start(adc_struct* adc_p){
       if (adc_p->driver.handle==NULL)
       {
       adc_p->driver.handle = ADC_open((adc_p->driver).pinout, &(adc_p->driver.params));
       }
}
void adc_stop(adc_struct* adc_p){
   if (adc_p->driver.handle!=NULL)
   {
	ADC_close((adc_p->driver).handle);
	(adc_p->driver).handle=NULL;
   }
}
/* Initialize an ADC driver & an specific ADC channel */
void adc_init(adc_struct* adc_p){
/* ADC conversion  variables */
//Inits all the ADCs
adc_init_driver();                 //first we init the app
	ADC_Params_init(&(adc_p->driver.params));//then we init the parameters
	(adc_p->driver).handle = ADC_open((adc_p->driver).pinout, &(adc_p->driver.params));
     #ifdef DEBUG_MODE
        if ((adc_p->driver).handle== NULL) {while (1); } /* CONFIG_ADC did not open */
     #endif
}
/* Initialize an ADC driver & an a group of ADC channels */
void adc_init_array(adc_struct* adc_array_p[], uint8_t adc_array_amount){
uint8_t i=0;//Start to iterate over all the adc drivers
//Inits all the ADCs
    adc_init_driver();                 //first we init the app
	ADC_Params_init(&(adc_array_p[i]->driver.params));//then we init the parameters
    for (i=0; i < adc_array_amount; i++) {
	    (adc_array_p[i]->driver).handle = ADC_open((adc_array_p[i]->driver).pinout, &(adc_array_p[i]->driver.params));
     #ifdef DEBUG_MODE
        if ((adc_array_p[i]->driver).handle== NULL) {while (1); } /* CONFIG_ADC did not open */
     #endif
    }
}
/* Performs an ADC acquisition, storing the result in the structure
 * returns the result of the conversion (fail/succes) */
int32_t adc_get(adc_struct* adc){
	return(ADC_convert(((adc->driver).handle), &(adc->result_bits)));
}
/* Converts a value of an specific channel from bits to mV */
void adc_to_mv(adc_struct* adc){
uint32_t adc_result_uV;
 //first we perform the scalem using the library of the adc
adc_result_uV = ADC_convertRawToMicroVolts((adc->driver).handle, adc->result_bits);//)adc->result_bits);
 // then we perform the scale and conversion to mV.
//IMPORTANT: adc->input_scale IS DEFINED *1000 IN ORDER TO ALLOW THE CONVERSION FROM mV TO UV
adc->result_mV=( adc_uint_t )(((adc->input_scale)*adc_result_uV/1000));

}
