################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
src/rtos_tasks.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/rtos_tasks.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/state_machine.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/state_machine.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_adc0_sysmonitor.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_adc0_sysmonitor.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_animal_activity.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_animal_activity.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_i2c0_imu_accel_gyro.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_i2c0_imu_accel_gyro.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_i2c0_imu_mag.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_i2c0_imu_mag.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_pwm0_1_2_leds.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_pwm0_1_2_leds.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_rfsub1ghz_ka.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_rfsub1ghz_ka.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_rfsub1ghz_rx_data_DCS.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_rfsub1ghz_rx_data_DCS.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_rfsub1ghz_rx_data_TD.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_rfsub1ghz_rx_data_TD.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_rfsub1ghz_tx_data.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_rfsub1ghz_tx_data.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_spi0_microsd.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_spi0_microsd.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_temperature.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_temperature.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_uart1_gps.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_uart1_gps.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

src/task_uart_printu.o: G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code/task_uart_printu.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti_cgt_tiarmclang_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/devices_source_code" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources" -I"G:/texas_instruments_proyects/devices_animal_monitoring/commons_sources/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -Wall -ffunction-sections -fdata-sections -march=armv7e-m -MMD -MP -MF"src/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/devices_animal_monitoring/_device_tortoiseV1.1_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


