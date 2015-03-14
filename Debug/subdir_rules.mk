################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
bl_main.obj: ../bl_main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="E:/00 SwarmRobot/Workspace/SVN/BootloaderRFv1/trunk" --include_path="D:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs --define=PART_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="bl_main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

bl_startup_ccs.obj: ../bl_startup_ccs.s $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="E:/00 SwarmRobot/Workspace/SVN/BootloaderRFv1/trunk" --include_path="D:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs --define=PART_TM4C123GH6PM --define=TARGET_IS_BLIZZARD_RB1 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="bl_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


