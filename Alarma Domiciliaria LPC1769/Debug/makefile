################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: TP_Integrador_ED3.axf

# Tool invocations
TP_Integrador_ED3.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Ivan-Pc\Documents\MCUXpressoIDE_11.2.0_4120\Drivers\CMSISv2p00_LPC17xx\Debug" -Xlinker -Map="TP_Integrador_ED3.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "TP_Integrador_ED3_Debug.ld" -o "TP_Integrador_ED3.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) TP_Integrador_ED3.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "TP_Integrador_ED3.axf"; # arm-none-eabi-objcopy -v -O binary "TP_Integrador_ED3.axf" "TP_Integrador_ED3.bin" ; # checksum -p LPC1769 -d "TP_Integrador_ED3.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
