################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/screens/lcd/ILI9341_GFX.c \
../Core/Src/screens/lcd/ILI9341_STM32_Driver.c \
../Core/Src/screens/lcd/ILI9341_Touchscreen.c \
../Core/Src/screens/lcd/LCD.c 

OBJS += \
./Core/Src/screens/lcd/ILI9341_GFX.o \
./Core/Src/screens/lcd/ILI9341_STM32_Driver.o \
./Core/Src/screens/lcd/ILI9341_Touchscreen.o \
./Core/Src/screens/lcd/LCD.o 

C_DEPS += \
./Core/Src/screens/lcd/ILI9341_GFX.d \
./Core/Src/screens/lcd/ILI9341_STM32_Driver.d \
./Core/Src/screens/lcd/ILI9341_Touchscreen.d \
./Core/Src/screens/lcd/LCD.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/screens/lcd/%.o: ../Core/Src/screens/lcd/%.c Core/Src/screens/lcd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/odemki/work/projects/my_projects/STM32/projects/STM32F407VG/Core/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

