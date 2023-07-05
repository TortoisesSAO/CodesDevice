################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1022600127: ../device_tortoiseV2.0.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/sysconfig_1_14_0/sysconfig_cli.bat" -s "C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/.metadata/product.json" --script "G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/device_tortoiseV2.0.syscfg" -o "syscfg" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_devices_config.c: build-1022600127 ../device_tortoiseV2.0.syscfg
syscfg/ti_radio_config.c: build-1022600127
syscfg/ti_radio_config.h: build-1022600127
syscfg/ti_drivers_config.c: build-1022600127
syscfg/ti_drivers_config.h: build-1022600127
syscfg/ti_utils_build_linker.cmd.genlibs: build-1022600127
syscfg/syscfg_c.rov.xs: build-1022600127
syscfg/ti_utils_runtime_model.gv: build-1022600127
syscfg/ti_utils_runtime_Makefile: build-1022600127
syscfg/ti_sysbios_config.h: build-1022600127
syscfg/ti_sysbios_config.c: build-1022600127
syscfg/: build-1022600127

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

proyect_device_battery_percent_table.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/proyect_device_battery_percent_table.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

proyect_device_id_table.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/proyect_device_id_table.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

ti_radio_config_custom.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/ti_radio_config_custom.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


