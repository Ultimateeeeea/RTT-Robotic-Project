################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/libc/compilers/common/cctype.c \
../rt-thread/components/libc/compilers/common/cstdio.c \
../rt-thread/components/libc/compilers/common/cstdlib.c \
../rt-thread/components/libc/compilers/common/cstring.c \
../rt-thread/components/libc/compilers/common/ctime.c \
../rt-thread/components/libc/compilers/common/cwchar.c 

OBJS += \
./rt-thread/components/libc/compilers/common/cctype.o \
./rt-thread/components/libc/compilers/common/cstdio.o \
./rt-thread/components/libc/compilers/common/cstdlib.o \
./rt-thread/components/libc/compilers/common/cstring.o \
./rt-thread/components/libc/compilers/common/ctime.o \
./rt-thread/components/libc/compilers/common/cwchar.o 

C_DEPS += \
./rt-thread/components/libc/compilers/common/cctype.d \
./rt-thread/components/libc/compilers/common/cstdio.d \
./rt-thread/components/libc/compilers/common/cstdlib.d \
./rt-thread/components/libc/compilers/common/cstring.d \
./rt-thread/components/libc/compilers/common/ctime.d \
./rt-thread/components/libc/compilers/common/cwchar.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/libc/compilers/common/%.o: ../rt-thread/components/libc/compilers/common/%.c
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32F1 -DUSE_HAL_DRIVER -DSTM32F103xE -I"E:\RT-ThreadStudio\workspace\ea1\drivers" -I"E:\RT-ThreadStudio\workspace\ea1\drivers\include" -I"E:\RT-ThreadStudio\workspace\ea1\drivers\include\config" -I"E:\RT-ThreadStudio\workspace\ea1\libraries\CMSIS\Device\ST\STM32F1xx\Include" -I"E:\RT-ThreadStudio\workspace\ea1\libraries\CMSIS\Include" -I"E:\RT-ThreadStudio\workspace\ea1\libraries\CMSIS\RTOS\Template" -I"E:\RT-ThreadStudio\workspace\ea1\libraries\STM32F1xx_HAL_Driver\Inc" -I"E:\RT-ThreadStudio\workspace\ea1\libraries\STM32F1xx_HAL_Driver\Inc\Legacy" -I"E:\RT-ThreadStudio\workspace\ea1" -I"E:\RT-ThreadStudio\workspace\ea1\applications" -I"E:\RT-ThreadStudio\workspace\ea1" -I"E:\RT-ThreadStudio\workspace\ea1\cubemx\Inc" -I"E:\RT-ThreadStudio\workspace\ea1\cubemx" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\drivers\include" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\finsh" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\libc\compilers\common\include" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\libc\compilers\newlib" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\libc\posix\io\poll" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\libc\posix\io\stdio" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\libc\posix\ipc" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\include" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\libcpu\arm\common" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\libcpu\arm\cortex-m3" -include"E:\RT-ThreadStudio\workspace\ea1\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

