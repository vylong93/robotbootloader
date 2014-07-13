################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/Program Files/TI/ccsv6/tools/compiler/arm_5.1.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="E:/00 SwarmRobot/00 Tiva/Workspace/bootLoaderRFV2" --include_path="D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573" --include_path="D:/Program Files/TI/ccsv6/tools/compiler/arm_5.1.5/include" -g --gcc --define=css="css" --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tm4c123gh6pm_startup_ccs.obj: ../tm4c123gh6pm_startup_ccs.s $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"D:/Program Files/TI/ccsv6/tools/compiler/arm_5.1.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="E:/00 SwarmRobot/00 Tiva/Workspace/bootLoaderRFV2" --include_path="D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573" --include_path="D:/Program Files/TI/ccsv6/tools/compiler/arm_5.1.5/include" -g --gcc --define=css="css" --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tm4c123gh6pm_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


