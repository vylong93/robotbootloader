################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"D:/Program Files/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/bin/arm-none-eabi-gcc.exe" -c -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -Dgcc -I"D:/Program Files/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/arm-none-eabi/include" -I"E:/00 SwarmRobot/00 Tiva/Workspace/bootLoaderRFV2" -I"D:/Portable/Programs/TivaDriver" -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.s $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"D:/Program Files/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/bin/arm-none-eabi-gcc.exe" -c -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -Dgcc -I"D:/Program Files/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/arm-none-eabi/include" -I"E:/00 SwarmRobot/00 Tiva/Workspace/bootLoaderRFV2" -I"D:/Portable/Programs/TivaDriver" -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


