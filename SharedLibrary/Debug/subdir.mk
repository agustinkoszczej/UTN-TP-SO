################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Conversor.c \
../CustomCommons.c \
../Results.c \
../Sender.c \
../logger.c 

OBJS += \
./Conversor.o \
./CustomCommons.o \
./Results.o \
./Sender.o \
./logger.o 

C_DEPS += \
./Conversor.d \
./CustomCommons.d \
./Results.d \
./Sender.d \
./logger.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


