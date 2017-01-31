################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ComLink3.c \
../src/Victron.c \
../src/readDatabase.c 

OBJS += \
./src/ComLink3.o \
./src/Victron.o \
./src/readDatabase.o 

C_DEPS += \
./src/ComLink3.d \
./src/Victron.d \
./src/readDatabase.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include/modbus -I/usr/include/mysql -I/usr/local/lib/pkgconfig -I/usr/local/lib -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


