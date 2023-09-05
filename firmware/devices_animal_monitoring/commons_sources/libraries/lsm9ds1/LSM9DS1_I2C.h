#ifndef LSM9DS1_I2C_H_
#define LSM9DS1_I2C_H_
#include "drivers_AL/i2c_AL.h"
#include "ti_drivers_config.h"
//Define print uart only for debugging pourpose
//#define PRINT_UART
//this
#ifndef CONFIG_I2C_0
  #ifdef CONFIG_I2C_OPT3001
  #define CONFIG_I2C_0 CONFIG_I2C_OPT3001
  #endif

  #ifdef i2c_imu
  #define CONFIG_I2C_0 i2c_imu
  #endif
#endif
//i2C parameters
#define I2C0_IMU_NUMER         CONFIG_I2C_0
#define I2C0_IMU_SPEED         i2c_400kbits
#define I2C0_IMU_MODE          I2C_BLOCKING
#define I2C0_IMU_SLAVE_ADDRES  0x08 //DFUMMY ADDRES, IN THIS CASE ONLY TO INITLIALIZE

#endif // LSM9DS1_I2C_H //
