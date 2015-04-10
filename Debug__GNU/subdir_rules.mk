################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"E:/ProgramFiles/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-gcc.exe" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -Dgcc -DTARGET_IS_BLIZZARD_RB1 -DRF_USE_nRF24L01 -I"E:/ProgramFiles/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_8-2014q3/arm-none-eabi/include" -I"E:/ProgramFiles/TI/TivaWare_C_Series-2.1.0.12573" -I"F:/00_SwarmRobot/Workspace/SVN/GitHub_BootloaderRFv1/trunk" -O0 -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -MD -std=c99 -pedantic -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.S $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"E:/ProgramFiles/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-gcc.exe" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -Dgcc -DTARGET_IS_BLIZZARD_RB1 -DRF_USE_nRF24L01 -I"E:/ProgramFiles/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_8-2014q3/arm-none-eabi/include" -I"E:/ProgramFiles/TI/TivaWare_C_Series-2.1.0.12573" -I"F:/00_SwarmRobot/Workspace/SVN/GitHub_BootloaderRFv1/trunk" -O0 -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -MD -std=c99 -pedantic -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


