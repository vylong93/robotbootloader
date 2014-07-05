################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/Program Files/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3

ORDERED_OBJS += \
"./libnrf24l01/src/nRF24L01.o" \
"./libnrf24l01/src/TM4C123_nRF24L01.o" \
"./tm4c123gh6pm_startup_gcc.o" \
"./main.o" \
-Wl,-T"../bl_link.ld" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-lgcc \
-lnosys \
-ldriver \
-Wl,--end-group \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include libnrf24l01/src/subdir_vars.mk
-include subdir_rules.mk
-include libnrf24l01/src/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
bootLoaderRFV2.out \

EXE_OUTPUTS__QUOTED += \
"bootLoaderRFV2.out" \

BIN_OUTPUTS += \
bootLoaderRFV2.hex \

BIN_OUTPUTS__QUOTED += \
"bootLoaderRFV2.hex" \


# All Target
all: bootLoaderRFV2.out

# Tool invocations
bootLoaderRFV2.out: $(OBJS) $(CMD_SRCS) $(LD_SRCS) $(LDS_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Linker'
	"D:/Program Files/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/bin/arm-none-eabi-gcc.exe" -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -Dgcc -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -Wl,-Map,"bootLoaderRFV2.map" -L"D:/Program Files/TI/TivaWare_C_Series-2.1.0.12573/driverlib/gcc" -o"bootLoaderRFV2.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

bootLoaderRFV2.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: GNU Objcopy Utility'
	"D:/Program Files/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_7-2013q3/bin/arm-none-eabi-objcopy.exe"  $(EXE_OUTPUTS__QUOTED) "bootLoaderRFV2.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "main.d" "libnrf24l01\src\TM4C123_nRF24L01.d" "libnrf24l01\src\nRF24L01.d" 
	-$(RM) "tm4c123gh6pm_startup_gcc.d" 
	-$(RM) "main.o" "tm4c123gh6pm_startup_gcc.o" "libnrf24l01\src\TM4C123_nRF24L01.o" "libnrf24l01\src\nRF24L01.o" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

