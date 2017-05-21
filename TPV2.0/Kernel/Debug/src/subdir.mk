################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ConfigKernel.c \
../src/Kernel.c \
../src/Planificacion.c \
../src/Procesos.c 

OBJS += \
./src/ConfigKernel.o \
./src/Kernel.o \
./src/Planificacion.o \
./src/Procesos.o 

C_DEPS += \
./src/ConfigKernel.d \
./src/Kernel.d \
./src/Planificacion.d \
./src/Procesos.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/TPSO/SharedLibrary" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


