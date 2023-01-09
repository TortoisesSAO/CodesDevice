/*
 *
 */

#include "micro_sd_fat.h"

/* update sd time based on MCU RTC*/
//THIS FUNCTION MUST BE DEFINED BUT NOT DECLARED OUTSIDE
   /* update sd time based on MCU RTC*/
#ifndef SD_WRITE_TIME_TO_FILE
void fatfs_getFatTime(void){}//must be defined even if not used
#else
int32_t fatfs_getFatTime(void)
{
    time_t seconds;
    uint32_t fatTime;
    struct tm *pTime;
    /*
     *  TI time() returns seconds elapsed since 1900, while other tools
     *  return seconds from 1970.  However, both TI and GNU localtime()
     *  sets tm tm_year to number of years since 1900.
     */
    seconds = time(NULL);
    pTime = localtime(&seconds);
    /*
     *  localtime() sets pTime->tm_year to number of years
     *  since 1900, so subtract 80 from tm_year to get FAT time
     *  offset from 1980.
     */
    fatTime = ((uint32_t)(pTime->tm_year - 80) << 25) |
        ((uint32_t)(pTime->tm_mon) << 21) |
        ((uint32_t)(pTime->tm_mday) << 16) |
        ((uint32_t)(pTime->tm_hour) << 11) |
        ((uint32_t)(pTime->tm_min) << 5) |
        ((uint32_t)(pTime->tm_sec) >> 1);

    return ((int32_t)fatTime);
}
#endif
/* Driver configuration */

void microsd_init(microsd_struct* microsd_p)
{//The following functions must be called before for correct settings:
       //microsd_set_mount
       //microsd_set_file_properties
	//set the default values
   SDFatFS_init();
/* add_device() should be called once and is used for all media types */
   add_device(microsd_p->fatfs_prefix, _MSA, ffcio_open, ffcio_close, ffcio_read,
    ffcio_write, ffcio_lseek, ffcio_unlink, ffcio_rename);
#ifdef SD_WRITE_TIME_TO_FILE
   /* Initialize real-time clock for saving the time in your file*/
   microsd_p->driver.ts.tv_sec=SD_START_TIME;
   clock_settime(CLOCK_REALTIME, &(microsd_p->driver.ts));//this is used to assign a creation data
#endif
   //Display_printf(display, 0, 0, "complete file name inside {}:{%s}",microsd_p->file_name_all );
   microsd_mount_sd(microsd_p);//mounts the sd memory
   //At this point, you can write and close files
}
void microsd_set_mount(microsd_struct* microsd_p,uint8_t driver_num,uint8_t mount_num, char* fat_prefix)
{char number[1];
microsd_p->driver.driver_num=driver_num;
if (mount_num>9){mount_num=0;}
microsd_p->driver.mount_num=mount_num;
microsd_p->mount_num_c[0]=	NULL;//clean up mount_num_c to "strncat" first character
number[0]=48+mount_num;//add 480 to unsigned int to convert to 1 digit number char
strncat(microsd_p->mount_num_c, ":", 1);
strncat(microsd_p->mount_num_c,number, 1);
strncat(microsd_p->mount_num_c, ":", 1);
//If mount_num==0->> microsd.mount_num_c=":0:" after this
memcpy(microsd_p->fatfs_prefix, fat_prefix, sizeof(microsd_p->fatfs_prefix));//sizeof(microsd.fatfs_prefix)
microsd_update_file_all(microsd_p);//updates the complete file name
}


int microsd_mount_sd(microsd_struct* microsd_p)
{	microsd_p->driver.handle = SDFatFS_open(microsd_p->driver.driver_num, microsd_p->driver.mount_num);
if (microsd_p->driver.handle == NULL) {
	//Display_printf(display, 0, 0, "Error starting the SD card\n");
	microsd_p->state_code=SD_MOUNT_ERROR;
	microsd_p->state=1;}
else {microsd_p->state_code=SD_MOUNT_OK;
      microsd_p->state=0;}
return(microsd_p->state);//0 0 if succesfull, 1 if error
}
void microsd_unmount_sd(microsd_struct* microsd_p)
{	/* Stopping the SDCard */
	SDFatFS_close(microsd_p->driver.handle);
     }
int microsd_open_file(microsd_struct* microsd_p)
{//after opening the SDCARD, we proceed to do write operation in append mode
microsd_p->driver.file_src = fopen(microsd_p->file_name_all,microsd_p->open_mode);//THis is operation that consumes most of the time when doing it at power up

if ((microsd_p->driver.file_src)==NULL)
    {microsd_p->state_error_code=1;
	 microsd_p->state_code=SD_FILE_OPEN_ERROR;
	 microsd_p->state=0;}
else{microsd_p->state_code=SD_FILE_OPEN_OK;
microsd_p->state_error_code=0;//not write error code
microsd_p->state=1;
}
return(microsd_p->state_error_code);// 0 if succesfull, 1 if error
}
int microsd_close_file(microsd_struct* microsd_p)
{//close the file after writting
microsd_p->state_error_code=fclose(microsd_p->driver.file_src);
if ((microsd_p->state_error_code)==0)//O==SUCCESFULLY CLOSED
    {microsd_p->state_code=SD_FILE_CLOSE_OK;
    microsd_p->state=1;}
else{microsd_p->state_code=SD_FILE_CLOSE_ERROR;
     microsd_p->state=0;}

return(microsd_p->state_error_code);

}
void microsd_set_file_properties(microsd_struct* microsd_p,char* open_mode,char* name, char* extention)
{
memcpy(microsd_p->open_mode, open_mode, sizeof(microsd_p->open_mode));
microsd_p->file_name[0]=NULL;//clean up the file name to clean "strncat" first character
memcpy(microsd_p->file_name, name, sizeof(microsd_p->file_name));
microsd_p->file_extention[0]=NULL;//clean up the file extention
memcpy(microsd_p->file_extention, extention, sizeof(microsd_p->file_extention));
microsd_update_file_all(microsd_p);
}
void microsd_update_file_all(microsd_struct* microsd_p)
{
	microsd_p->file_name_all[0]=	NULL;//clean up the file name to clean "strncat" first character
	strncat(microsd_p->file_name_all, microsd_p->fatfs_prefix,sizeof(microsd_p->fatfs_prefix));
	strncat(microsd_p->file_name_all, microsd_p->mount_num_c,sizeof(microsd_p->mount_num_c));
	strncat(microsd_p->file_name_all, microsd_p->file_name,sizeof(microsd_p->file_name));
	strncat(microsd_p->file_name_all, microsd_p->file_extention,sizeof(microsd_p->file_extention));

}

void microsd_write_block(microsd_struct* microsd_p,unsigned char* data_p, uint16_t data_size)
{if ((microsd_p->driver.file_src)!=NULL)//check the file was created succesfully
{
	fwrite(data_p, 1, data_size, microsd_p->driver.file_src);
    fflush(microsd_p->driver.file_src);//resets micro sdwrite buffer
    rewind(microsd_p->driver.file_src); /* Reset the internal file pointer */
    //Display_printf(display, 0, 0, "done\n");
}
/*
else {Display_printf(display, 0, 0,"Error: \"%s\" could not be created.\nPlease check the "
    //    "Board.html if additional jumpers are necessary.\n",
	//	microsd_p->file_name_all);
//Display_printf(display, 0, 0, "Aborting...\n");
 while (1);}
 */
}
int microsd_open_write_close(microsd_struct* microsd_p,unsigned char* data_p, uint16_t data_size )
{int result;
#ifdef SD_WRITE_TIME_TO_FILE
	fatfs_getFatTime();/* update sd time based on MICRO RTS*/
#endif
//microsd_mount_sd(microsd_p);//mounts the sd memory
result=microsd_open_file(microsd_p);//open a file and saves its name
if (result==0){//resul=0 means no error
microsd_write_block(microsd_p,data_p,data_size );//writes to the last opened file
microsd_close_file(microsd_p);//closes the last opened file
}
//microsd_unmount_sd(microsd_p);//unmount the micro sd memory
if ((microsd_p->driver.file_src)!=NULL)
   {microsd_p->state=1;
	return (1);}//file opened, SUCCESS!
else{microsd_p->state=0;
	return (0);}//No file opened, error!
}

int check_micro_sd_connected(microsd_struct* microsd_p)
{
microsd_open_file(microsd_p);//open a file and saves its name
microsd_close_file(microsd_p);//closes the last opened file
if ((microsd_p->driver.file_src)!=NULL)
   {microsd_p->state=0;}//file opened, SUCCESS!
else{microsd_p->state=1;}
//returns the value
return (microsd_p->state);//No file opened, error!
}
