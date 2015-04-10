################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := E:/ProgramFiles/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_8-2014q3

ORDERED_OBJS += \
"./bl_main.o" \
"./bl_startup_gcc.o" \
"./libnrf24l01/src/TM4C123_nRF24L01.o" \
"./libnrf24l01/src/nRF24L01.o" \
"./libcc2500/src/TM4C123_CC2500.o" \
"./libcc2500/src/cc2500.o" \
-Wl,-T"../tm4c123gh6pm.ld" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-l"c" \
-l"driver" \
-l"gcc" \
-l"nosys" \
-Wl,--end-group \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include libnrf24l01/src/subdir_vars.mk
-include libcc2500/src/subdir_vars.mk
-include subdir_rules.mk
-include libnrf24l01/src/subdir_rules.mk
-include libcc2500/src/subdir_rules.mk
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
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
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
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
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
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
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
BootloaderRFV4.out \

EXE_OUTPUTS__QUOTED += \
"BootloaderRFV4.out" \

BIN_OUTPUTS += \
BootloaderRFV4.hex \

BIN_OUTPUTS__QUOTED += \
"BootloaderRFV4.hex" \


# All Target
all: BootloaderRFV4.out

# Tool invocations
BootloaderRFV4.out: $(OBJS) $(LD_SRCS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Linker'
	"E:/ProgramFiles/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-gcc.exe" -march=armv7e-m -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -Dgcc -DTARGET_IS_BLIZZARD_RB1 -DRF_USE_nRF24L01 -O0 -ffunction-sections -fdata-sections -g -gstrict-dwarf -Wall -Wl,-Map,"BootloaderRFV4.map" -Wl,--gc-sections -L"E:/ProgramFiles/TI/TivaWare_C_Series-2.1.0.12573/driverlib/gcc" -o"BootloaderRFV4.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

BootloaderRFV4.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: GNU Objcopy Utility'
	"E:/ProgramFiles/TI/ccsv6/tools/compiler/gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-objcopy.exe"  $(EXE_OUTPUTS__QUOTED) "BootloaderRFV4.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "bl_startup_gcc.d" 
	-$(RM) "bl_main.d" "libnrf24l01\src\TM4C123_nRF24L01.d" "libnrf24l01\src\nRF24L01.d" "libcc2500\src\TM4C123_CC2500.d" "libcc2500\src\cc2500.d" 
	-$(RM) "bl_main.o" "bl_startup_gcc.o" "libnrf24l01\src\TM4C123_nRF24L01.o" "libnrf24l01\src\nRF24L01.o" "libcc2500\src\TM4C123_CC2500.o" "libcc2500\src\cc2500.o" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-arm-none-eabi-size BootloaderRFV4.out
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

