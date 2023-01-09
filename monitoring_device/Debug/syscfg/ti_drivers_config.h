/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  The macros defines herein are intended for use by applications which
 *  directly include this header. These macros should NOT be hard coded or
 *  copied into library source code.
 *
 *  Symbols declared as const are intended for use with libraries.
 *  Library source code must extern the correct symbol--which is resolved
 *  when the application is linked.
 *
 *  DO NOT EDIT - This file is generated for the CC1312R1_LAUNCHXL
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_CC1312R1_LAUNCHXL
#ifndef DeviceFamily_CC13X2
#define DeviceFamily_CC13X2
#endif

#include <ti/devices/DeviceFamily.h>

#include <stdint.h>

/* support C++ sources */
#ifdef __cplusplus
extern "C" {
#endif


/*
 *  ======== CCFG ========
 */


/*
 *  ======== ADC ========
 */

/* DIO28 */
extern const uint_least8_t              adc_battery_level_CONST;
#define adc_battery_level               0
/* DIO30 */
extern const uint_least8_t              adc_5V_usb_CONST;
#define adc_5V_usb                      1
/* DIO29 */
extern const uint_least8_t              adc_battery_current_CONST;
#define adc_battery_current             2
#define CONFIG_TI_DRIVERS_ADC_COUNT     3


/*
 *  ======== GPIO ========
 */
/* Owned by CONFIG_UART2_0 as  */
extern const uint_least8_t CONFIG_GPIO_UART2_0_TX_CONST;
#define CONFIG_GPIO_UART2_0_TX 25

/* Owned by CONFIG_UART2_0 as  */
extern const uint_least8_t CONFIG_GPIO_UART2_0_RX_CONST;
#define CONFIG_GPIO_UART2_0_RX 26

/* Owned by adc_battery_level as  */
extern const uint_least8_t CONFIG_GPIO_adc_battery_level_AIN_CONST;
#define CONFIG_GPIO_adc_battery_level_AIN 28

/* Owned by adc_5V_usb as  */
extern const uint_least8_t CONFIG_GPIO_adc_5V_usb_AIN_CONST;
#define CONFIG_GPIO_adc_5V_usb_AIN 30

/* Owned by adc_battery_current as  */
extern const uint_least8_t CONFIG_GPIO_adc_battery_current_AIN_CONST;
#define CONFIG_GPIO_adc_battery_current_AIN 29

extern const uint_least8_t gpio_on_sd_CONST;
#define gpio_on_sd 11

extern const uint_least8_t gpio_on_gps_CONST;
#define gpio_on_gps 20

extern const uint_least8_t gpio_led_status0_CONST;
#define gpio_led_status0 19

extern const uint_least8_t gpio_led_status1_CONST;
#define gpio_led_status1 18

extern const uint_least8_t gpio_led_status2_CONST;
#define gpio_led_status2 17

/* Owned by i2c_imu as  */
extern const uint_least8_t CONFIG_GPIO_i2c_imu_SDA_CONST;
#define CONFIG_GPIO_i2c_imu_SDA 4

/* Owned by i2c_imu as  */
extern const uint_least8_t CONFIG_GPIO_i2c_imu_SCL_CONST;
#define CONFIG_GPIO_i2c_imu_SCL 5

/* Owned by i2s_mic as  */
extern const uint_least8_t CONFIG_GPIO_1_CONST;
#define CONFIG_GPIO_1 7

/* Owned by i2s_mic as  */
extern const uint_least8_t CONFIG_GPIO_2_CONST;
#define CONFIG_GPIO_2 1

/* Owned by i2s_mic as  */
extern const uint_least8_t CONFIG_GPIO_3_CONST;
#define CONFIG_GPIO_3 8

/* Owned by i2s_mic as  */
extern const uint_least8_t CONFIG_GPIO_4_CONST;
#define CONFIG_GPIO_4 6

/* Owned by spi_sd as  */
extern const uint_least8_t CONFIG_GPIO_0_CONST;
#define CONFIG_GPIO_0 15

/* Owned by CONFIG_SPI_0 as  */
extern const uint_least8_t CONFIG_GPIO_SPI_0_SCLK_CONST;
#define CONFIG_GPIO_SPI_0_SCLK 13

/* Owned by CONFIG_SPI_0 as  */
extern const uint_least8_t CONFIG_GPIO_SPI_0_MISO_CONST;
#define CONFIG_GPIO_SPI_0_MISO 12

/* Owned by CONFIG_SPI_0 as  */
extern const uint_least8_t CONFIG_GPIO_SPI_0_MOSI_CONST;
#define CONFIG_GPIO_SPI_0_MOSI 14

/* Owned by uart_mu1 as  */
extern const uint_least8_t CONFIG_GPIO_uart_mu1_TX_CONST;
#define CONFIG_GPIO_uart_mu1_TX 21

/* Owned by uart_mu1 as  */
extern const uint_least8_t CONFIG_GPIO_uart_mu1_RX_CONST;
#define CONFIG_GPIO_uart_mu1_RX 22

/* The range of pins available on this device */
extern const uint_least8_t GPIO_pinLowerBound;
extern const uint_least8_t GPIO_pinUpperBound;

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)


/*
 *  ======== I2C ========
 */

/*
 *  SCL: DIO5
 *  SDA: DIO4
 */
extern const uint_least8_t              i2c_imu_CONST;
#define i2c_imu                         0
#define CONFIG_TI_DRIVERS_I2C_COUNT     1

/* ======== I2C Addresses and Speeds ======== */
#include <ti/drivers/I2C.h>

/* ---- i2c_imu I2C bus components ---- */

/* no components connected to i2c_imu */

/* i2c_imu.maxBitRate configuration specified 1 Kbps */
#define i2c_imu_MAXSPEED   (1U) /* Kbps */
#define i2c_imu_MAXBITRATE ((I2C_BitRate)I2C_100kHz)


/*
 *  ======== I2S ========
 */

/*
 *  SCK: DIO8
 *  WS: DIO6
 *  SD0: DIO7
 *  SD1: DIO1
 */
extern const uint_least8_t              i2s_mic_CONST;
#define i2s_mic                         0
#define CONFIG_TI_DRIVERS_I2S_COUNT     1




/*
 *  ======== SD ========
 */

/*
 *  MOSI: DIO14
 *  MISO: DIO12
 *  SCLK: DIO13
 *  SS: undefined
 */
extern const uint_least8_t              spi_sd_CONST;
#define spi_sd                          0
#define CONFIG_TI_DRIVERS_SD_COUNT      1


/*
 *  ======== SPI ========
 */

/*
 *  MOSI: DIO14
 *  MISO: DIO12
 *  SCLK: DIO13
 */
extern const uint_least8_t              CONFIG_SPI_0_CONST;
#define CONFIG_SPI_0                    0
#define CONFIG_TI_DRIVERS_SPI_COUNT     1


/*
 *  ======== UART2 ========
 */

/*
 *  TX: DIO25
 *  RX: DIO26
 */
extern const uint_least8_t                  CONFIG_UART2_0_CONST;
#define CONFIG_UART2_0                      0
/*
 *  TX: DIO21
 *  RX: DIO22
 */
extern const uint_least8_t                  uart_mu1_CONST;
#define uart_mu1                            1
#define CONFIG_TI_DRIVERS_UART2_COUNT       2


/*
 *  ======== Board_init ========
 *  Perform all required TI-Drivers initialization
 *
 *  This function should be called once at a point before any use of
 *  TI-Drivers.
 */
extern void Board_init(void);

/*
 *  ======== Board_initGeneral ========
 *  (deprecated)
 *
 *  Board_initGeneral() is defined purely for backward compatibility.
 *
 *  All new code should use Board_init() to do any required TI-Drivers
 *  initialization _and_ use <Driver>_init() for only where specific drivers
 *  are explicitly referenced by the application.  <Driver>_init() functions
 *  are idempotent.
 */
#define Board_initGeneral Board_init

#ifdef __cplusplus
}
#endif

#endif /* include guard */
