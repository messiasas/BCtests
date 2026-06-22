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

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
NOSTRIP += \
app.nostrip \


# All Target
all: BCprolin-testes

# Tool invocations
BCprolin-testes: $(NOSTRIP)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Strip'
	"C:/Users/filipe.silva/prolin_sdk_win-3.5_/prolin_sdk_win-3.5/sdk/toolchains/arm-4.6.3/bin/arm-none-linux-gnueabi-strip" -g $(NOSTRIP) -o"BCprolin-testes"
	@echo 'Finished building target: $@'
	@echo ' '

app.nostrip: $(OBJS) $(USER_OBJS)
	@echo 'Invoking: GCC Linker'
	"C:/Users/filipe.silva/prolin_sdk_win-3.5_/prolin_sdk_win-3.5/sdk/toolchains/arm-4.6.3/bin/arm-none-linux-gnueabi-gcc" -o"app.nostrip" $(OBJS) $(USER_OBJS) $(LIBS) -L"C:/Users/filipe.silva/prolin_sdk_win-3.5_/prolin_sdk_win-3.5/sdk/platforms/prolin-dev-4.6.3/lib" -Wl,-rpath=//opt/lib,-rpath=./lib,-rpath=/system/lib -Wl,-rpath-link,"C:/Users/filipe.silva/prolin_sdk_win-3.5_/prolin_sdk_win-3.5/sdk/platforms/prolin-dev-4.6.3/lib" -losal -lD_DEVICE_LIB_v615 -lF_AE_LIB_v254 -lF_DPAS_LIB_v155 -lF_EMV_LIB_v660 -lF_ENTRY_LIB_v560 -lF_MC_LIB_v558_01 -lF_PUBLIC_LIB_v115 -lF_WAVE_LIB_v359_02 -lPPCOMP_Prolin_v162 -lxui -lts -L"C:/Users/filipe.silva/workspaces2/BCprolin-testes/lib"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(NOSTRIP) BCprolin-testes
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
