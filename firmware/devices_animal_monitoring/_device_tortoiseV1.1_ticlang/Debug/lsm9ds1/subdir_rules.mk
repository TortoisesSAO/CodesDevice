################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
lsm9ds1/LSM9DS1.o: G:/texas_instruments_proyects/libraries/lsm9ds1/LSM9DS1.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.2.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/_tortugometro_ticlang2" -I"G:/texas_instruments_proyects/_tortugometro_ticlang2/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -march=armv7e-m -MMD -MP -MF"lsm9ds1/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/_tortugometro_ticlang2/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


