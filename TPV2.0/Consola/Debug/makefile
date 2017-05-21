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
all: Consola

dependents:
	-cd /home/utnso/workspace/TPSO/SharedLibrary/Debug && $(MAKE) all

# Tool invocations
Consola: $(OBJS) $(USER_OBJS) /home/utnso/workspace/TPSO/SharedLibrary/Debug/libSharedLibrary.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/TPSO/SharedLibrary/Debug" -o "Consola" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) Consola
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/workspace/TPSO/SharedLibrary/Debug/libSharedLibrary.so:

-include ../makefile.targets
