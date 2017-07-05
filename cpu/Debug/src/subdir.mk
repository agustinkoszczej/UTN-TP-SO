################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cpu.c \
../src/cpu_interface.c \
../src/cpu_primitivas.c 

OBJS += \
./src/cpu.o \
./src/cpu_interface.o \
./src/cpu_primitivas.o 

C_DEPS += \
./src/cpu.d \
./src/cpu_interface.d \
./src/cpu_primitivas.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2017-1c-Lords-of-the-Strings/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


