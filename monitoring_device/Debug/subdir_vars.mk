################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../task_i2c0_imu.cpp \
../task_i2c0_imu_mag.cpp \
../task_temp.cpp \
../task_uart0_gps.cpp 

CMD_SRCS += \
../cc13x2_cc26x2_tirtos7.cmd 

SYSCFG_SRCS += \
../tortugometroV2.0.syscfg 

C_SRCS += \
../main.c \
../rtos_tasks.c \
../task_rfsub1ghz_ka.c \
../task_spi0_microsd.c \
../ti_easylink_config.c \
../ti_radio_config.c \
./syscfg/ti_devices_config.c \
./syscfg/ti_radio_config.c \
./syscfg/ti_drivers_config.c \
./syscfg/ti_sysbios_config.c 

GEN_FILES += \
./syscfg/ti_devices_config.c \
./syscfg/ti_radio_config.c \
./syscfg/ti_drivers_config.c \
./syscfg/ti_sysbios_config.c 

GEN_MISC_DIRS += \
./syscfg/ 

C_DEPS += \
./main.d \
./rtos_tasks.d \
./task_rfsub1ghz_ka.d \
./task_spi0_microsd.d \
./ti_easylink_config.d \
./ti_radio_config.d \
./syscfg/ti_devices_config.d \
./syscfg/ti_radio_config.d \
./syscfg/ti_drivers_config.d \
./syscfg/ti_sysbios_config.d 

OBJS += \
./main.o \
./rtos_tasks.o \
./task_i2c0_imu.o \
./task_i2c0_imu_mag.o \
./task_rfsub1ghz_ka.o \
./task_spi0_microsd.o \
./task_temp.o \
./task_uart0_gps.o \
./ti_easylink_config.o \
./ti_radio_config.o \
./syscfg/ti_devices_config.o \
./syscfg/ti_radio_config.o \
./syscfg/ti_drivers_config.o \
./syscfg/ti_sysbios_config.o 

GEN_MISC_FILES += \
./syscfg/ti_radio_config.h \
./syscfg/ti_drivers_config.h \
./syscfg/ti_utils_build_linker.cmd.genlibs \
./syscfg/syscfg_c.rov.xs \
./syscfg/ti_utils_runtime_model.gv \
./syscfg/ti_utils_runtime_Makefile \
./syscfg/ti_sysbios_config.h 

CPP_DEPS += \
./task_i2c0_imu.d \
./task_i2c0_imu_mag.d \
./task_temp.d \
./task_uart0_gps.d 

GEN_MISC_DIRS__QUOTED += \
"syscfg\" 

OBJS__QUOTED += \
"main.o" \
"rtos_tasks.o" \
"task_i2c0_imu.o" \
"task_i2c0_imu_mag.o" \
"task_rfsub1ghz_ka.o" \
"task_spi0_microsd.o" \
"task_temp.o" \
"task_uart0_gps.o" \
"ti_easylink_config.o" \
"ti_radio_config.o" \
"syscfg\ti_devices_config.o" \
"syscfg\ti_radio_config.o" \
"syscfg\ti_drivers_config.o" \
"syscfg\ti_sysbios_config.o" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_radio_config.h" \
"syscfg\ti_drivers_config.h" \
"syscfg\ti_utils_build_linker.cmd.genlibs" \
"syscfg\syscfg_c.rov.xs" \
"syscfg\ti_utils_runtime_model.gv" \
"syscfg\ti_utils_runtime_Makefile" \
"syscfg\ti_sysbios_config.h" 

C_DEPS__QUOTED += \
"main.d" \
"rtos_tasks.d" \
"task_rfsub1ghz_ka.d" \
"task_spi0_microsd.d" \
"ti_easylink_config.d" \
"ti_radio_config.d" \
"syscfg\ti_devices_config.d" \
"syscfg\ti_radio_config.d" \
"syscfg\ti_drivers_config.d" \
"syscfg\ti_sysbios_config.d" 

CPP_DEPS__QUOTED += \
"task_i2c0_imu.d" \
"task_i2c0_imu_mag.d" \
"task_temp.d" \
"task_uart0_gps.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_devices_config.c" \
"syscfg\ti_radio_config.c" \
"syscfg\ti_drivers_config.c" \
"syscfg\ti_sysbios_config.c" 

C_SRCS__QUOTED += \
"../main.c" \
"../rtos_tasks.c" \
"../task_rfsub1ghz_ka.c" \
"../task_spi0_microsd.c" \
"../ti_easylink_config.c" \
"../ti_radio_config.c" \
"./syscfg/ti_devices_config.c" \
"./syscfg/ti_radio_config.c" \
"./syscfg/ti_drivers_config.c" \
"./syscfg/ti_sysbios_config.c" 

CPP_SRCS__QUOTED += \
"../task_i2c0_imu.cpp" \
"../task_i2c0_imu_mag.cpp" \
"../task_temp.cpp" \
"../task_uart0_gps.cpp" 

SYSCFG_SRCS__QUOTED += \
"../tortugometroV2.0.syscfg" 


