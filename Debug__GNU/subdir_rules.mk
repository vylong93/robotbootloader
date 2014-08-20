################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"c:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/bin/arm-none-eabi-gcc.exe" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -Dgcc -DTARGET_IS_BLIZZARD_RB1 -I"c:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/arm-none-eabi/include" -I"D:/Portable/Programs/TivaDriver" -I"D:/Software Projects/ArmTI/BootloaderRFV3" -O0 -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.S $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"c:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/bin/arm-none-eabi-gcc.exe" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -Dgcc -DTARGET_IS_BLIZZARD_RB1 -I"c:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/arm-none-eabi/include" -I"D:/Portable/Programs/TivaDriver" -I"D:/Software Projects/ArmTI/BootloaderRFV3" -O0 -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


