################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SharedLibrary/EstructurasComunes.c \
../SharedLibrary/Hilos.c \
../SharedLibrary/ServerCommons.c 

OBJS += \
./SharedLibrary/EstructurasComunes.o \
./SharedLibrary/Hilos.o \
./SharedLibrary/ServerCommons.o 

C_DEPS += \
./SharedLibrary/EstructurasComunes.d \
./SharedLibrary/Hilos.d \
./SharedLibrary/ServerCommons.d 


# Each subdirectory must supply rules for building sources it contributes
SharedLibrary/%.o: ../SharedLibrary/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


