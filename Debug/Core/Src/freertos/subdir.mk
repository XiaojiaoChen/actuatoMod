################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/freertos/freertos.c 

OBJS += \
./Core/Src/freertos/freertos.o 

C_DEPS += \
./Core/Src/freertos/freertos.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/freertos/freertos.o: ../Core/Src/freertos/freertos.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 '-DHSE_VALUE=((uint32_t)(25000000))' -DUSE_HAL_DRIVER -DARM_MATH_CM3 -DSTM32F103xB -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/CMSIS/DSP" -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/User" -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/VL6180xDriver" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/freertos/freertos.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

