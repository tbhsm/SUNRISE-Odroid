################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BicycleShed.c \
../src/Display.c \
../src/GPIOs.c \
../src/ServerCom.c \
../src/Victron.c \
../src/Weather.c \
../src/readDatabase.c 

OBJS += \
./src/BicycleShed.o \
./src/Display.o \
./src/GPIOs.o \
./src/ServerCom.o \
./src/Victron.o \
./src/Weather.o \
./src/readDatabase.o 

C_DEPS += \
./src/BicycleShed.d \
./src/Display.d \
./src/GPIOs.d \
./src/ServerCom.d \
./src/Victron.d \
./src/Weather.d \
./src/readDatabase.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/ -I/usr/lib -I/usr/local/lib -I/usr/include/mysql -I/usr/local/include/modbus -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


