################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
libnrf24l01/src/%.o: ../libnrf24l01/src/%.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"c:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/bin/arm-none-eabi-gcc.exe" -c -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -Dgcc -I"c:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/arm-none-eabi/include" -I"D:/Software Projects/TortoiseSVN/BootLoaderRF" -I"D:/Portable/Programs/TivaDriver" -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


