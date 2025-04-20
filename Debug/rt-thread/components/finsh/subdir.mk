################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/finsh/cmd.c \
../rt-thread/components/finsh/msh.c \
../rt-thread/components/finsh/msh_parse.c \
../rt-thread/components/finsh/shell.c 

OBJS += \
./rt-thread/components/finsh/cmd.o \
./rt-thread/components/finsh/msh.o \
./rt-thread/components/finsh/msh_parse.o \
./rt-thread/components/finsh/shell.o 

C_DEPS += \
./rt-thread/components/finsh/cmd.d \
./rt-thread/components/finsh/msh.d \
./rt-thread/components/finsh/msh_parse.d \
./rt-thread/components/finsh/shell.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/finsh/%.o: ../rt-thread/components/finsh/%.c
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32F1 -DUSE_HAL_DRIVER -DSTM32F103xE -I"E:\RT-ThreadStudio\workspace\Robtic\drivers" -I"E:\RT-ThreadStudio\workspace\Robtic\drivers\include" -I"E:\RT-ThreadStudio\workspace\Robtic\drivers\include\config" -I"E:\RT-ThreadStudio\workspace\Robtic\libraries\CMSIS\Device\ST\STM32F1xx\Include" -I"E:\RT-ThreadStudio\workspace\Robtic\libraries\CMSIS\Include" -I"E:\RT-ThreadStudio\workspace\Robtic\libraries\CMSIS\RTOS\Template" -I"E:\RT-ThreadStudio\workspace\Robtic\libraries\STM32F1xx_HAL_Driver\Inc" -I"E:\RT-ThreadStudio\workspace\Robtic\libraries\STM32F1xx_HAL_Driver\Inc\Legacy" -I"E:\RT-ThreadStudio\workspace\Robtic" -I"E:\RT-ThreadStudio\workspace\Robtic\applications" -I"E:\RT-ThreadStudio\workspace\Robtic" -I"E:\RT-ThreadStudio\workspace\Robtic\cubemx\Inc" -I"E:\RT-ThreadStudio\workspace\Robtic\cubemx" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\drivers\include" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\finsh" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\libc\compilers\common\include" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\libc\compilers\newlib" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\libc\posix\io\poll" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\libc\posix\io\stdio" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\components\libc\posix\ipc" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\include" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\libcpu\arm\common" -I"E:\RT-ThreadStudio\workspace\Robtic\rt-thread\libcpu\arm\cortex-m3" -include"E:\RT-ThreadStudio\workspace\Robtic\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

