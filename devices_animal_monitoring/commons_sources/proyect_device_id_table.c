
#include <device_system_setup.h>
#include "device_system_information.h"
#if (DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO)
#include "TI_CC13XX_id/device_id_TI_CC13X2.h"
#define DEVICE_ID_NONLISTED "00:00:00:00:00:00:00:00"
#define DEVICE_NUM_NONLISTED 0 //DO NOT CHANGE!!
const device_ID_table_Entry device_ID_table[] ={
//MD_TORTORISE HARDWARE V2.X (MCU CC131X) LISTED WORKING DEVICES
  //ID (MAC ADDRESS FOR CC1312R)|ASIGNED NUMBER OF DEVICE
      {"00:12:4B:00:1C:AA:49:4E",{1,148100000}},
      {"00:12:4B:00:1C:AA:42:E4",{2,148200000}},
      {"00:12:4B:00:1C:AA:4B:CB",{3,148300000}},
      {"00:12:4B:00:1C:AA:44:65",{4,148400000}},
      {"00:12:4B:00:1C:AA:44:2F",{5,148500000}},
      {"00:12:4B:00:1C:AA:46:53",{6,148600000}},
      {"00:12:4B:00:1C:AA:44:7D",{7,148700000}},
      {DEVICE_ID_NONLISTED      ,{DEVICE_NUM_NONLISTED,RF_KA_FREQ_DEFAULT}}//for non listed device, uses this value of freq
};
#endif
#if (DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD)
#include "TI_CC13XX_id/device_id_TI_CC13X2.h"
#define DEVICE_ID_NONLISTED "00:00:00:00:00:00:00:00"
#define DEVICE_NUM_NONLISTED 0
const device_ID_table_Entry device_ID_table[] ={
//MD_LIZARD HARDWARE V1.X (MCU CC131X) LISTED WORKING DEVICES
  //ID (MAC ADDRESS FOR CC1312R)|ASIGNED NUMBER OF DEVICE
      {"00:12:4B:00:2B:EB:14:FB",{1,148100000}},
      {DEVICE_ID_NONLISTED      ,{DEVICE_NUM_NONLISTED,RF_KA_FREQ_DEFAULT}}//for non listed device, uses this value of freq
};
#endif

