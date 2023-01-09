#ifndef TASK_SPI0_MICROSD_H_
#define TASK_SPI0_MICROSD_H_

#include <unistd.h>//for uint def
#include <string.h>//for memcpy ussage
const char gpskml_begin[]= "\r\n<?xml version=\"1.0\" encoding=\"utf-8\" ?> <kml xmlns=\"http://www.opengis.net/kml/2.2\"> <Document id=\"root_doc\"><Folder><name>Tracks</name> </Folder><Folder><name>Layer0</name><Placemark><name>Path</name><Style><LineStyle><color>ff0000ff</color></LineStyle><PolyStyle><fill>0</fill></PolyStyle></Style> <LineString><coordinates> ";
const char gpskml_end[]="</coordinates> </LineString> </Placemark> </Folder> </Document></kml";

#define SD_BLOCK_UNIT_SIZE         4096
/* MCU SPI DRIVER ASIGNED, FIXED */
#define SD_SPI_DRIVER_NUMBER       spi_sd//CONFIG_SDFatFS_0 is the default name

/* Drive number used for FatFs */
#define SD_MOUNT_DRIVE_NUM      0
#define SD_FATFS_PREFIX         "fat32"
#define SD_FILE_PATH            "DIRECTOR"
#define SD_FILE_NAME            "12345678"//
#define SD_FILE_EXTENTION       ".dat"
#define SD_OPEN_MODE            "a "//operating mode=appends


#ifdef __cplusplus
   }
 #endif
#endif
