/*
 *
 */

#include "micro_sd_fat.h"

/* update sd time based on MCU RTC*/
//THIS FUNCTION MUST BE DEFINED BUT NOT DECLARED OUTSIDE
   /* update sd time based on MCU RTC*/
#ifndef SD_WRITE_TIME_TO_FILE
int32_t fatfs_getFatTime(void){
return(0)
}//must be defined even if not used
#else
void fatfs_set_date_Time(microsd_struct* microsd_p){
/* Initialize real-time clock for saving the time in your file if it was not already init*/
    time_t seconds;
    struct tm *pTime;
    seconds = time(NULL);
    pTime = localtime(&seconds);//getb local time
   //if you wish an autosetup
   if ((pTime->tm_year)==69){//equals 69  (in TI standardad, this means that time was never seted and will be
     microsd_p->driver.ts.tv_sec=SD_START_TIME;
     clock_settime(CLOCK_REALTIME, &(microsd_p->driver.ts));//this is used to assign a creation data
        }
}
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
{//init the driver, must be called only once
   SDFatFS_init();
   microsd_p->file_subdir1[0]=0;//init
   microsd_p->file_subdir2[0]=0;//init
   microsd_p->file_subdir3[0]=0;//init
/* add_device() should be called once and is used for all media types */
   add_device(microsd_p->fatfs_prefix, _MSA, ffcio_open, ffcio_close, ffcio_read,
    ffcio_write, ffcio_lseek, ffcio_unlink, ffcio_rename);
#ifdef SD_WRITE_TIME_TO_FILE
   /* Initialize real-time clock for saving the time in your file*/
   fatfs_set_date_Time(microsd_p);
#endif
   //Display_printf(display, 0, 0, "complete file name inside {}:{%s}",microsd_p->file_name_all );
   //At this point, you can write and close files
}
void microsd_update_file_all(microsd_struct* microsd_p){
	(microsd_p->file_name_all)[0]=	0;//clean up the file name to clean "strncat" first character
	strncat(microsd_p->file_name_all, microsd_p->fatfs_prefix,sizeof(microsd_p->fatfs_prefix));
	strncat(microsd_p->file_name_all, microsd_p->mount_num_c,sizeof(microsd_p->mount_num_c));
	if (microsd_p->file_subdir1[0]>0){
		strncat(microsd_p->file_name_all, "/",1);
		strncat(microsd_p->file_name_all, microsd_p->file_subdir1, sizeof(microsd_p->file_subdir1));
		strncat(microsd_p->file_name_all, "/",1);
		if (microsd_p->file_subdir2[0]>0){
			strncat(microsd_p->file_name_all, microsd_p->file_subdir2,sizeof(microsd_p->file_subdir2));
			strncat(microsd_p->file_name_all, "/",1);}
	}
	strncat(microsd_p->file_name_all, microsd_p->file_name,sizeof(microsd_p->file_name));
	strncat(microsd_p->file_name_all, microsd_p->file_extention,sizeof(microsd_p->file_extention));
}
void microsd_update_file_all_name_ext(microsd_struct* microsd_p,char* name_all,uint8_t name_size ){
	(microsd_p->file_name_all)[0]=	0;//clean up the file name to clean "strncat" first character
	strncat(microsd_p->file_name_all, microsd_p->fatfs_prefix,sizeof(microsd_p->fatfs_prefix));
	strncat(microsd_p->file_name_all, microsd_p->mount_num_c,sizeof(microsd_p->mount_num_c));
	if (microsd_p->file_subdir1[0]>0){
		strncat(microsd_p->file_name_all, "/",1);
		strncat(microsd_p->file_name_all, microsd_p->file_subdir1,sizeof(microsd_p->file_subdir1));
		strncat(microsd_p->file_name_all, "/",1);
		if (microsd_p->file_subdir2[0]>0){
			strncat(microsd_p->file_name_all, microsd_p->file_subdir2,sizeof(microsd_p->file_subdir2));
			strncat(microsd_p->file_name_all, "/",1);}
	}

	strncat(microsd_p->file_name_all, name_all, name_size);
}

void microsd_set_mount(microsd_struct* microsd_p,uint8_t driver_num,uint8_t mount_num, char* fat_prefix)
{char number[1];
microsd_p->driver.driver_num=driver_num;
if (mount_num>9){mount_num=0;}
microsd_p->driver.mount_num=mount_num;
microsd_p->mount_num_c[0]=0;//clean up mount_num_c to "strncat" first character
number[0]=48+mount_num;//add 48 to unsigned int to convert to 1 digit number char
strncat(microsd_p->mount_num_c, ":", 1);
strncat(microsd_p->mount_num_c,number, 1);
strncat(microsd_p->mount_num_c, ":", 1);
memcpy(microsd_p->fatfs_prefix, fat_prefix, sizeof(microsd_p->fatfs_prefix));//sizeof(microsd.fatfs_prefix)
microsd_update_file_all(microsd_p);//updates the complete file name
}


int microsd_mount_sd(microsd_struct* microsd_p)
{	microsd_p->driver.handle = NULL;
	microsd_p->driver.handle = SDFatFS_open(microsd_p->driver.driver_num, microsd_p->driver.mount_num);
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
microsd_p->driver.file_src=NULL;
microsd_p->driver.file_src = fopen(microsd_p->file_name_all,microsd_p->open_mode);//THis is operation that consumes most of the time when doing it at power up

if ((microsd_p->driver.file_src)==NULL)//error is null
    {microsd_p->state_error_code=1;
	 microsd_p->state_code=SD_FILE_OPEN_ERROR;
	 microsd_p->state=1;}
else{microsd_p->state_code=SD_FILE_OPEN_OK;
	microsd_p->state_error_code=0;//not write error code
	microsd_p->state=0;
}
return(microsd_p->state_error_code);// 0 if succesfull, 1 if error
}
int microsd_close_file(microsd_struct* microsd_p)
{//close the file after writting
microsd_p->state_error_code=fclose(microsd_p->driver.file_src);
if ((microsd_p->state_error_code)==0)//O==SUCCESFULLY CLOSED
    {microsd_p->state_code=SD_FILE_CLOSE_OK;
    microsd_p->state=0;}
else{microsd_p->state_code=SD_FILE_CLOSE_ERROR;
     microsd_p->state=1;}

return(microsd_p->state_error_code);

}

void microsd_set_file_properties(microsd_struct* microsd_p,char* name, char* extention,char* open_mode){
memcpy(microsd_p->file_name, name, sizeof(microsd_p->file_name));
memcpy(microsd_p->file_extention, extention, sizeof(microsd_p->file_extention));
memcpy(microsd_p->open_mode, open_mode, sizeof(microsd_p->open_mode));
microsd_update_file_all(microsd_p);
}
//for internal use only. Updates char structs parameters. Must be called each time a folder, name or extention is changed

void microsd_write_block(microsd_struct* microsd_p,unsigned char* data_p, uint16_t data_size)
{if ((microsd_p->driver.file_src)!=NULL)//check the file was created succesfully
{
  if (data_size>0){
	 fwrite(data_p, 1, data_size, microsd_p->driver.file_src);
     fflush(microsd_p->driver.file_src);//resets micro sdwrite buffer
     rewind(microsd_p->driver.file_src); /* Reset the internal file pointer */
    }
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
int microsd_create_subdir(microsd_struct*microsd_p,char* pname,uint8_t name_size){
int result=-1;
char dir_path[SD_MAX_PATH_SIZE];//must be constant
//error protetction
uint8_t name_size_ok;
if (name_size>SD_MAX_NAME_OR_DIR_SIZE){name_size_ok=SD_MAX_NAME_OR_DIR_SIZE;}else{name_size_ok=name_size;}
dir_path[0]=0;
microsd_mount_sd(microsd_p);//mounts the sd memory
if (microsd_p->state==0){
	if (microsd_p->file_subdir1[0]==0){ //path
		strncat(dir_path, pname,name_size_ok);
	    result=f_mkdir(dir_path);
	    if ( (result==0)|(result==FR_EXIST) ){ //0: operation succesfull, 8: folder already exist
	    	memcpy(microsd_p->file_subdir1, pname, name_size_ok);}
	  }
	  else{ if ( microsd_p->file_subdir2[0]==0){
		    	  strncat(dir_path, microsd_p->file_subdir1,sizeof(microsd_p->file_subdir1));
		    	  strncat(dir_path, "/",1);
		    	  strncat(dir_path, pname,name_size_ok);
		    	  result=f_mkdir(dir_path);
		  	      if ( (result==0)|(result==FR_EXIST) ){ //0: operation succesfull, 8: folder already exist
		  	    	    memcpy(microsd_p->file_subdir2, pname, name_size_ok);}
	   	      	}
        }
    microsd_update_file_all(microsd_p);
  }
microsd_unmount_sd(microsd_p);//mounts the sd memory
return(result);
}
int microsd_delete_file_name(microsd_struct* microsd_p, char* ppath_name,uint8_t name_size){
  int result=-1; //working
  char path_to_delete[SD_FILE_NAME_SIZE];
	memcpy(path_to_delete, ppath_name, name_size);
	//---------------mount sd and delete file
	microsd_mount_sd(microsd_p);//mounts the sd memory
	if (microsd_p->state==0){
		result=f_unlink(path_to_delete);//not working any of those optiobns
	}
	microsd_unmount_sd(microsd_p);//mounts the sd memory
return(result);
}

//open a file with another name
int microsd_open_write_close_name(microsd_struct* microsd_p,unsigned char* data_p, uint16_t data_size, char* name_all,uint8_t name_size , char* open_mode){
  int result;
  char name_backup[SD_FILE_NAME_SIZE];
  char openmode[SD_OPEN_MODE_SIZE];
/*
*/
memcpy(name_backup, microsd_p->file_name_all, sizeof(microsd_p->file_name_all));
memcpy(openmode, microsd_p->open_mode, sizeof(microsd_p->open_mode));
//---------------update the parameter
microsd_update_file_all_name_ext(microsd_p,name_all, name_size );
//---------------open the file
result=microsd_open_write_close(microsd_p,data_p,data_size);
//---------------return the original parameters
memcpy(microsd_p->file_name_all, name_backup, sizeof(microsd_p->file_name_all));
memcpy(microsd_p->open_mode, openmode, sizeof(microsd_p->open_mode));

return (result);
}

int microsd_open_write_close(microsd_struct* microsd_p,unsigned char* data_p, uint16_t data_size ){
#ifdef SD_WRITE_TIME_TO_FILE
	fatfs_getFatTime();/* update sd time based on MICRO RTS*/
#endif
microsd_mount_sd(microsd_p);//mounts the sd memory
if (microsd_p->state==0){
	microsd_open_file(microsd_p);//open a file and saves its name
	if (microsd_p->state==0){
		microsd_write_block(microsd_p,data_p,data_size );//writes to the last opened file
		microsd_close_file(microsd_p);//closes the last opened file
		}
  }
microsd_unmount_sd(microsd_p);//unmount the micro sd memory
if(microsd_p->state==0){return 0;}
    else {return 1;}
}

/*creates , writes and deletes a file in order to check if the connected micro sd is working properly*/
int check_micro_sd_connected(microsd_struct* microsd_p)
{ int result;
  unsigned char c_aux[2]="t";
  const unsigned char file_name[]="TEST.TTT";//file to create
result=microsd_open_write_close_name(microsd_p, c_aux, 1 , ( char*) file_name, sizeof(file_name),"w ");
microsd_delete_file_name(microsd_p,( char*)file_name, sizeof(file_name));
return (result);//0: opening success/ 1:No file opened, error!
}
