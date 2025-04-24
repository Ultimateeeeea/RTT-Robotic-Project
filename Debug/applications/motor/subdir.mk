################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../applications/motor/motor_cmd.c \
../applications/motor/motor_motion.c \
../applications/motor/motor_state.c \
../applications/motor/motor_thread.c 

OBJS += \
./applications/motor/motor_cmd.o \
./applications/motor/motor_motion.o \
./applications/motor/motor_state.o \
./applications/motor/motor_thread.o 

C_DEPS += \
./applications/motor/motor_cmd.d \
./applications/motor/motor_motion.d \
./applications/motor/motor_state.d \
./applications/motor/motor_thread.d 


# Each subdirectory must supply rules for building sources it contributes
applications/motor/%.o: ../applications/motor/%.c
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32F1 -DUSE_HAL_DRIVER -DSTM32F103xE -I"E:\RT-ThreadStudio\workspace\Robtic\drivers" -I"E:\RT-ThreadStudio\workspace\Robtic\drivers\include" -I"E:\RT-ThreadStudio\workspace\Robtic\drivers\include\config" -I"E:\RT-ThreadStudio\workspace\Robtic\libraries\CMSIS\Device\ST\STM32F1xx\Include" -I"E:\RT-ThreadStudio\workspace\Robtic\libraries\CMSIS\Include" -I"E:\RT-ThreadStudio\workspace\Robtic\libraries\CMSIS\RTOS\Template" -I"E:\RT-ThreadStudio\workspace\Robtic\libraries\STM32F1xx_HAL_Driver\Inc" -I"E:\RT-ThreadStudio\workspace\Robtic\libraries\STM32F1xx_HAL_Driver\Inc\Legacy" -I"E:\RT-ThreadStudio\workspace\Robtic" -I"E:\RT-ThreadStudio\workspace\Robtic\applications" -I"E:\RT-ThreadStudio\workspace\Robtic" -I"E:\RT-ThreadStudio\workspace\Robtic\cubemx\Inc" -I"E:\RT-ThreadStudio\workspace\Robtic\cubemx" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\drivers\include" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\finsh" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\libc\compilers\common\include" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\libc\compilers\newlib" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\libc\posix\io\poll" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\libc\posix\io\stdio" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\libc\posix\ipc" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\include" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\libcpu\arm\common" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\libcpu\arm\cortex-m3" -include"E:\RT-ThreadStudio\workspace\Robtic\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

