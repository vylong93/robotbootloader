################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
libnrf24l01/src/TM4C123_nRF24L01.obj: ../libnrf24l01/src/TM4C123_nRF24L01.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="D:/Software Projects/TortoiseSVN/BootLoaderRF" --include_path="D:/Portable/Programs/TivaDriver" --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.5/include" -g --gcc --define=css="css" --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="libnrf24l01/src/TM4C123_nRF24L01.pp" --obj_directory="libnrf24l01/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

libnrf24l01/src/nRF24L01.obj: ../libnrf24l01/src/nRF24L01.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="D:/Software Projects/TortoiseSVN/BootLoaderRF" --include_path="D:/Portable/Programs/TivaDriver" --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.5/include" -g --gcc --define=css="css" --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="libnrf24l01/src/nRF24L01.pp" --obj_directory="libnrf24l01/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


