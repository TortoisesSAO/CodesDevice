################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
inc/time_AL/time_AL.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries/time_AL/time_AL.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"inc/time_AL/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


