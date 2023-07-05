/*

 */
#ifndef DEVICE_SYSTEM_INFORMATION_H_
#define DEVICE_SYSTEM_INFORMATION_H_
#include <device_system_setup.h>
//code asociated to type of device
//RF code to detect kind of device
#define DEVICE_TYPE_RF_MD_TORTO               1
#define DEVICE_TYPE_MD_TORTOISE                "MD_TORTOISE"
#define DEVICE_TYPE_RF_TD_TORTO               2
#define DEVICE_TYPE_TD_TORTOISE               "TD_TORTOISE"
#define DEVICE_TYPE_RF_DCS_TORTO              3
#define DEVICE_TYPE_DCS_TORTOISE              "DCS_TORTOISE"
#define DEVICE_TYPE_RF_MD_LIZARD              4
#define DEVICE_TYPE_MD_LIZARD                 "MD_LIZARD"
#define DEVICE_TYPE_RF_TD_LIZARD              5
#define DEVICE_TYPE_TD_LIZARD                 "TD_LIZARD"
#define DEVICE_TYPE_RF_DCS_LIZARD             6
#define DEVICE_TYPE_DCS_LIZARD                "DCS_LIZARD"
//Generate DEVICE TYPES
#if ((DEVICE_TYPE_KIND==DEVICE_TYPE_MD)&(DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO))
#define MD_TORTOISE
#define DEVICE_TYPE                     DEVICE_TYPE_MD_TORTOISE
#define DEVICE_TYPE_RF                  DEVICE_TYPE_RF_MD_TORTO
#endif
#if ((DEVICE_TYPE_KIND==DEVICE_TYPE_TD)&(DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO))
#define TD_TORTOISE
#define DEVICE_TYPE                     DEVICE_TYPE_TD_TORTOISE
#define DEVICE_TYPE_RF                  DEVICE_TYPE_RF_TD_TORTO
#endif
#if ((DEVICE_TYPE_KIND==DEVICE_TYPE_DCS)&(DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO))
#define DCS_TORTOISE
#define DEVICE_TYPE                     DEVICE_TYPE_DCS_TORTOISE
#define DEVICE_TYPE_RF                  DEVICE_TYPE_RF_DCS_TORTO
#endif
#if ((DEVICE_TYPE_KIND==DEVICE_TYPE_MD)&(DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD))
#define MD_LIZARD
#define DEVICE_TYPE                          DEVICE_TYPE_MD_LIZARD
#define DEVICE_TYPE_RF                       DEVICE_TYPE_RF_MD_LIZARD
#endif
#if ((DEVICE_TYPE_KIND==DEVICE_TYPE_TD)&(DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD))
#define TD_LIZARD
#define DEVICE_TYPE                         DEVICE_TYPE_TD_LIZARD
#define DEVICE_TYPE_RF                      DEVICE_TYPE_RF_TD_LIZARD
#endif
#if ((DEVICE_TYPE_KIND==DEVICE_TYPE_DCS)&(DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD))
#define DCS_LIZARD
#define DEVICE_TYPE                         DEVICE_TYPE_DCS_LIZARD
#define DEVICE_TYPE_RF                      DEVICE_TYPE_RF_DCS_LIZARD
#endif

//--------------SAO Tortoises definitions----------------------------
#ifdef MD_TORTOISE
#define FILE_NAME                       "TORTOISE"
#endif
#ifdef TD_TORTOISE
#define FILE_NAME                       "TD_LOG"
#endif
#ifdef DCS_TORTOISE
#define FILE_NAME                       "DCS_LOG"
#endif
//------------- LIZZARD definitions----------------------------
#ifdef MD_LIZARD
#define FILE_NAME                       "LIZARD"
#endif
#ifdef TD_LIZARD
#define FILE_NAME                       "TD_LOG"
#endif
#ifdef DCS_LIZARD
#define FILE_NAME                       "DCS_LOG"
#endif
//------------------------------------------------------------
//-----------DEVICE FOLDERS (5 LETERS MAX) NAME
#if (DEVICE_TYPE_ANIMAL==ANIMAL_TORTOISE_SAO)
#define STORAGE_FOLDER_NAME             "TORTO"    //must be 5 leters due to file name limit of sd storate
#endif
//-----------DEVICE FOLDERS (5 LETERS MAX) NAME
#if (DEVICE_TYPE_ANIMAL==ANIMAL_LIZARD)
#define STORAGE_FOLDER_NAME             "LIZAR"    //must be 5 leters due to file name limit of sd storate
#endif
//-----------DEVICES VERSIONS ANIMAL_TORTOISE_SAO
#define MD_TORTOISE_HARD_VR1          		 "2.0"
#define MD_TORTOISE_FIRM_VR1                 "1.0"
#define MD_TORTOISE_FIRM_VR11                 "1.1"
#define TD_TORTOISE_HARD_VR1                 "2.0"
#define TD_TORTOISE_FIRM_VR1                 "1.0"
#define TD_TORTOISE_FIRM_VR11                 "1.1"
#define DCS_TORTOISE_HARD_VR1                "2.0"
#define DCS_TORTOISE_FIRM_VR1                "1.0"
#define DCS_TORTOISE_FIRM_VR11                "1.1"

//-----------DEVICES VERSIONS LIZARD_XX
#define MD_LIZARD_HARD_VR1                 "1.0"
#define MD_LIZARD_FIRM_VR1                 "1.0"
#define TD_LIZARD_HARD_VR1                 "1.0"
#define TD_LIZARD_FIRM_VR1                 "1.0"
#define DCS_LIZARD_HARD_VR1                "1.0"
#define DCS_LIZARD_FIRM_VR1                "1.0"
#endif // _

///****************************************************************
///****************************************************************
///****************************************************************
//generate the system information need

#ifdef MD_TORTOISE
    #define DEVICE_HARDWARE_VERSION            MD_TORTOISE_HARD_VR1                //MAX 5 CHARS
    #define DEVICE_FIRMWARE_VERSION            MD_TORTOISE_FIRM_VR11                //MAX 5 CHARS
    #define DEVICE_FOLDERNAME_5_LETERS         STORAGE_FOLDER_NAME
#endif
#ifdef TD_TORTOISE
    #define DEVICE_HARDWARE_VERSION            TD_TORTOISE_HARD_VR1                //MAX 5 CHARS
    #define DEVICE_FIRMWARE_VERSION            TD_TORTOISE_FIRM_VR11                //MAX 5 CHARS
    #define DEVICE_FOLDERNAME_5_LETERS         STORAGE_FOLDER_NAME
    #define DEVICE_FILENAME_8_LETERS           FILE_NAME
#endif
#ifdef DCS_TORTOISE
    #define DEVICE_HARDWARE_VERSION            DCS_TORTOISE_HARD_VR1                //MAX 5 CHARS
    #define DEVICE_FIRMWARE_VERSION            DCS_TORTOISE_FIRM_VR11                //MAX 5 CHARS
    #define DEVICE_FOLDERNAME_5_LETERS         STORAGE_FOLDER_NAME
    #define DEVICE_FILENAME_8_LETERS           FILE_NAME
#endif
//generate the system information need

#ifdef MD_LIZARD
    #define DEVICE_HARDWARE_VERSION            MD_LIZARD_HARD_VR1                //MAX 5 CHARS
    #define DEVICE_FIRMWARE_VERSION            MD_LIZARD_FIRM_VR1                //MAX 5 CHARS
    #define DEVICE_FOLDERNAME_5_LETERS         STORAGE_FOLDER_NAME
#endif
#ifdef TD_LIZARD
    #define DEVICE_HARDWARE_VERSION            TD_LIZARD_HARD_VR1                //MAX 5 CHARS
    #define DEVICE_FIRMWARE_VERSION            TD_LIZARD_FIRM_VR1                //MAX 5 CHARS
    #define DEVICE_FOLDERNAME_5_LETERS         STORAGE_FOLDER_NAME
    #define DEVICE_FILENAME_8_LETERS           FILE_NAME
#endif
#ifdef DCS_LIZARD
    #define DEVICE_HARDWARE_VERSION            DCS_LIZARD_HARD_VR1                //MAX 5 CHARS
    #define DEVICE_FIRMWARE_VERSION            DCS_LIZARD_FIRM_VR1                //MAX 5 CHARS
    #define DEVICE_FOLDERNAME_5_LETERS         STORAGE_FOLDER_NAME
    #define DEVICE_FILENAME_8_LETERS           FILE_NAME
#endif
