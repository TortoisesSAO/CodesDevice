################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ti_cgt_tiarmclang_2.1.2.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.o" \
"./rtos_tasks.o" \
"./task_i2c0_imu.o" \
"./task_i2c0_imu_mag.o" \
"./task_rfsub1ghz_ka.o" \
"./task_spi0_microsd.o" \
"./task_temp.o" \
"./task_uart0_gps.o" \
"./ti_easylink_config.o" \
"./ti_radio_config.o" \
"./syscfg/ti_devices_config.o" \
"./syscfg/ti_radio_config.o" \
"./syscfg/ti_drivers_config.o" \
"./syscfg/ti_sysbios_config.o" \
"./MAC_to/MAC_to.o" \
"./RTOS_AL/rtos_AL.o" \
"./RTOS_AL/semaphore_AL.o" \
"./drivers_AL/adc_AL.o" \
"./drivers_AL/gpio_AL.o" \
"./drivers_AL/i2c_AL.o" \
"./drivers_AL/pwm_AL.o" \
"./drivers_AL/uart_AL.o" \
"./drivers_AL/uart_printu.o" \
"./easylink/EasyLink.o" \
"./gps_parser/TinyGPS++.o" \
"./gps_ublox/gps_ublox.o" \
"./lsm9ds1/LSM9DS1.o" \
"./micro_sd_fat/micro_sd_fat.o" \
"../cc13x2_cc26x2_tirtos7.cmd" \
$(GEN_CMDS__FLAG) \
-Wl,-l"ti/display/lib/ticlang/m4f/display_cc13x2.a" \
-Wl,-l"ti/grlib/lib/ticlang/m4f/grlib.a" \
-Wl,-l"third_party/spiffs/lib/ticlang/m4f/spiffs_cc26xx.a" \
-Wl,-lti_utils_build_linker.cmd.genlibs \
-Wl,-l"ti/devices/cc13x2_cc26x2/driverlib/bin/ticlang/driverlib.lib" \
-Wl,-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include MAC_to/subdir_vars.mk
-include RTOS_AL/subdir_vars.mk
-include drivers_AL/subdir_vars.mk
-include easylink/subdir_vars.mk
-include gps_parser/subdir_vars.mk
-include gps_ublox/subdir_vars.mk
-include lsm9ds1/subdir_vars.mk
-include micro_sd_fat/subdir_vars.mk
-include subdir_rules.mk
-include MAC_to/subdir_rules.mk
-include RTOS_AL/subdir_rules.mk
-include drivers_AL/subdir_rules.mk
-include easylink/subdir_rules.mk
-include gps_parser/subdir_rules.mk
-include gps_ublox/subdir_rules.mk
-include lsm9ds1/subdir_rules.mk
-include micro_sd_fat/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
_tortugometro_ticlang2.out \

EXE_OUTPUTS__QUOTED += \
"_tortugometro_ticlang2.out" \

BIN_OUTPUTS += \
_tortugometro_ticlang2.hex \

BIN_OUTPUTS__QUOTED += \
"_tortugometro_ticlang2.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "_tortugometro_ticlang2.out"

# Tool invocations
_tortugometro_ticlang2.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"C:/ti/ti_cgt_tiarmclang_2.1.2.LTS/bin/tiarmclang.exe" -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -march=armv7e-m -Wl,-m"_tortugometro_ticlang2.map" -Wl,-i"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -Wl,-i"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -Wl,-i"G:/texas_instruments_proyects/_tortugometro_ticlang2/Debug/syscfg" -Wl,-i"C:/ti/ti_cgt_tiarmclang_2.1.2.LTS/lib" -Wl,--define=-–use_memcpy={fast} -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections -Wl,--xml_link_info="_tortugometro_ticlang2_linkInfo.xml" -Wl,--rom_model -o "_tortugometro_ticlang2.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

_tortugometro_ticlang2.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: Arm Hex Utility'
	"C:/ti/ti_cgt_tiarmclang_2.1.2.LTS/bin/tiarmhex.exe" --diag_wrap=off -o "_tortugometro_ticlang2.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.o" "rtos_tasks.o" "task_i2c0_imu.o" "task_i2c0_imu_mag.o" "task_rfsub1ghz_ka.o" "task_spi0_microsd.o" "task_temp.o" "task_uart0_gps.o" "ti_easylink_config.o" "ti_radio_config.o" "syscfg\ti_devices_config.o" "syscfg\ti_radio_config.o" "syscfg\ti_drivers_config.o" "syscfg\ti_sysbios_config.o" "MAC_to\MAC_to.o" "RTOS_AL\rtos_AL.o" "RTOS_AL\semaphore_AL.o" "drivers_AL\adc_AL.o" "drivers_AL\gpio_AL.o" "drivers_AL\i2c_AL.o" "drivers_AL\pwm_AL.o" "drivers_AL\uart_AL.o" "drivers_AL\uart_printu.o" "easylink\EasyLink.o" "gps_parser\TinyGPS++.o" "gps_ublox\gps_ublox.o" "lsm9ds1\LSM9DS1.o" "micro_sd_fat\micro_sd_fat.o" 
	-$(RM) "main.d" "rtos_tasks.d" "task_rfsub1ghz_ka.d" "task_spi0_microsd.d" "ti_easylink_config.d" "ti_radio_config.d" "syscfg\ti_devices_config.d" "syscfg\ti_radio_config.d" "syscfg\ti_drivers_config.d" "syscfg\ti_sysbios_config.d" "MAC_to\MAC_to.d" "RTOS_AL\rtos_AL.d" "RTOS_AL\semaphore_AL.d" "drivers_AL\adc_AL.d" "drivers_AL\gpio_AL.d" "drivers_AL\i2c_AL.d" "drivers_AL\pwm_AL.d" "drivers_AL\uart_AL.d" "drivers_AL\uart_printu.d" "easylink\EasyLink.d" "micro_sd_fat\micro_sd_fat.d" 
	-$(RM) "task_i2c0_imu.d" "task_i2c0_imu_mag.d" "task_temp.d" "task_uart0_gps.d" "gps_parser\TinyGPS++.d" "gps_ublox\gps_ublox.d" "lsm9ds1\LSM9DS1.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

