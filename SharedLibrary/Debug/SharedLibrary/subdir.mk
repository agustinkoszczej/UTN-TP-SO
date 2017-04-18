################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SharedLibrary/Conversor.c \
../SharedLibrary/CustomCommons.c \
../SharedLibrary/Results.c \
../SharedLibrary/Sender.c \
../SharedLibrary/ServerManager.c \
../SharedLibrary/logger.c 

OBJS += \
./SharedLibrary/Conversor.o \
./SharedLibrary/CustomCommons.o \
./SharedLibrary/Results.o \
./SharedLibrary/Sender.o \
./SharedLibrary/ServerManager.o \
./SharedLibrary/logger.o 

C_DEPS += \
./SharedLibrary/Conversor.d \
./SharedLibrary/CustomCommons.d \
./SharedLibrary/Results.d \
./SharedLibrary/Sender.d \
./SharedLibrary/ServerManager.d \
./SharedLibrary/logger.d 


# Each subdirectory must supply rules for building sources it contributes
SharedLibrary/%.o: ../SharedLibrary/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


