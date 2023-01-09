################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
state_machine/state_machine.o: D:/.shortcut-targets-by-id/1Qfg2XPZbsYpt_xe3XgBg-zZX1cI8odDR/2021_tesis_maestria_andres_oliva_trevisan/firmware/texas_instruments_proyects/libraries/state_machine/state_machine.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-armllvm_1.3.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"G:/texas_instruments_proyects/_fatsd_tirtos7_ticlang" -I"G:/texas_instruments_proyects/_fatsd_tirtos7_ticlang/Debug" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/kernel/tirtos7/packages" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_30_00_84/source/ti/posix/ticlang" -I"G:/texas_instruments_proyects/libraries" -DDeviceFamily_CC13X2 -gstrict-dwarf -gdwarf-3 -march=armv7e-m -MMD -MP -MF"state_machine/$(basename $(<F)).d_raw" -MT"$(@)" -I"G:/texas_instruments_proyects/_fatsd_tirtos7_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


