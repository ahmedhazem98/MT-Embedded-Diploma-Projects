################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App.c \
../UART.c \
../buzzer.c \
../external_eeprom.c \
../gpio.c \
../motor.c \
../pwm.c \
../timer.c \
../twi.c 

OBJS += \
./App.o \
./UART.o \
./buzzer.o \
./external_eeprom.o \
./gpio.o \
./motor.o \
./pwm.o \
./timer.o \
./twi.o 

C_DEPS += \
./App.d \
./UART.d \
./buzzer.d \
./external_eeprom.d \
./gpio.d \
./motor.d \
./pwm.d \
./timer.d \
./twi.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


