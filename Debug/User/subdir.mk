################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/IMU_WT902.c \
../User/Wire.c \
../User/kalman.c \
../User/laser.c \
../User/myUsartFunction.c \
../User/sensors.c 

OBJS += \
./User/IMU_WT902.o \
./User/Wire.o \
./User/kalman.o \
./User/laser.o \
./User/myUsartFunction.o \
./User/sensors.o 

C_DEPS += \
./User/IMU_WT902.d \
./User/Wire.d \
./User/kalman.d \
./User/laser.d \
./User/myUsartFunction.d \
./User/sensors.d 


# Each subdirectory must supply rules for building sources it contributes
User/IMU_WT902.o: ../User/IMU_WT902.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 '-DHSE_VALUE=((uint32_t)(25000000))' -DUSE_HAL_DRIVER -DARM_MATH_CM3 -DSTM32F103xB -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/CMSIS/DSP" -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/User" -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/VL6180xDriver" -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"User/IMU_WT902.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
User/Wire.o: ../User/Wire.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 '-DHSE_VALUE=((uint32_t)(25000000))' -DUSE_HAL_DRIVER -DARM_MATH_CM3 -DSTM32F103xB -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/CMSIS/DSP" -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/User" -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/VL6180xDriver" -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"User/Wire.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
User/kalman.o: ../User/kalman.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 '-DHSE_VALUE=((uint32_t)(25000000))' -DUSE_HAL_DRIVER -DARM_MATH_CM3 -DSTM32F103xB -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/CMSIS/DSP" -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/User" -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/VL6180xDriver" -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"User/kalman.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
User/laser.o: ../User/laser.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 '-DHSE_VALUE=((uint32_t)(25000000))' -DUSE_HAL_DRIVER -DARM_MATH_CM3 -DSTM32F103xB -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/CMSIS/DSP" -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/User" -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/VL6180xDriver" -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"User/laser.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
User/myUsartFunction.o: ../User/myUsartFunction.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 '-DHSE_VALUE=((uint32_t)(25000000))' -DUSE_HAL_DRIVER -DARM_MATH_CM3 -DSTM32F103xB -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/CMSIS/DSP" -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/User" -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/VL6180xDriver" -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"User/myUsartFunction.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
User/sensors.o: ../User/sensors.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 '-DHSE_VALUE=((uint32_t)(25000000))' -DUSE_HAL_DRIVER -DARM_MATH_CM3 -DSTM32F103xB -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/CMSIS/DSP" -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/User" -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"C:/Users/40207/OneDrive/OneDriveDocumentation/STM32CubeIDEWorkSpaceOnedrive/actuatoMod/Drivers/VL6180xDriver" -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"User/sensors.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

