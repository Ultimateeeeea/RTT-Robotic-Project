################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/board.c \
../drivers/drv_adc.c \
../drivers/drv_clk.c \
../drivers/drv_common.c \
../drivers/drv_eth.c \
../drivers/drv_flash_f1.c \
../drivers/drv_gpio.c \
../drivers/drv_hwtimer.c \
../drivers/drv_pwm.c \
../drivers/drv_qspi.c \
../drivers/drv_rtc.c \
../drivers/drv_sdio.c \
../drivers/drv_soft_i2c.c \
../drivers/drv_spi.c \
../drivers/drv_usart.c \
../drivers/drv_usbd.c \
../drivers/drv_usbh.c \
../drivers/drv_wdt.c 

OBJS += \
./drivers/board.o \
./drivers/drv_adc.o \
./drivers/drv_clk.o \
./drivers/drv_common.o \
./drivers/drv_eth.o \
./drivers/drv_flash_f1.o \
./drivers/drv_gpio.o \
./drivers/drv_hwtimer.o \
./drivers/drv_pwm.o \
./drivers/drv_qspi.o \
./drivers/drv_rtc.o \
./drivers/drv_sdio.o \
./drivers/drv_soft_i2c.o \
./drivers/drv_spi.o \
./drivers/drv_usart.o \
./drivers/drv_usbd.o \
./drivers/drv_usbh.o \
./drivers/drv_wdt.o 

C_DEPS += \
./drivers/board.d \
./drivers/drv_adc.d \
./drivers/drv_clk.d \
./drivers/drv_common.d \
./drivers/drv_eth.d \
./drivers/drv_flash_f1.d \
./drivers/drv_gpio.d \
./drivers/drv_hwtimer.d \
./drivers/drv_pwm.d \
./drivers/drv_qspi.d \
./drivers/drv_rtc.d \
./drivers/drv_sdio.d \
./drivers/drv_soft_i2c.d \
./drivers/drv_spi.d \
./drivers/drv_usart.d \
./drivers/drv_usbd.d \
./drivers/drv_usbh.d \
./drivers/drv_wdt.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32F1 -DUSE_HAL_DRIVER -DSTM32F103xE -I"E:\RT-ThreadStudio\workspace\ea1\drivers" -I"E:\RT-ThreadStudio\workspace\ea1\drivers\include" -I"E:\RT-ThreadStudio\workspace\ea1\drivers\include\config" -I"E:\RT-ThreadStudio\workspace\ea1\libraries\CMSIS\Device\ST\STM32F1xx\Include" -I"E:\RT-ThreadStudio\workspace\ea1\libraries\CMSIS\Include" -I"E:\RT-ThreadStudio\workspace\ea1\libraries\CMSIS\RTOS\Template" -I"E:\RT-ThreadStudio\workspace\ea1\libraries\STM32F1xx_HAL_Driver\Inc" -I"E:\RT-ThreadStudio\workspace\ea1\libraries\STM32F1xx_HAL_Driver\Inc\Legacy" -I"E:\RT-ThreadStudio\workspace\ea1" -I"E:\RT-ThreadStudio\workspace\ea1\applications" -I"E:\RT-ThreadStudio\workspace\ea1" -I"E:\RT-ThreadStudio\workspace\ea1\cubemx\Inc" -I"E:\RT-ThreadStudio\workspace\ea1\cubemx" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\drivers\include" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\finsh" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\libc\compilers\common\include" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\libc\compilers\newlib" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\libc\posix\io\poll" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\libc\posix\io\stdio" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\components\libc\posix\ipc" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\include" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\libcpu\arm\common" -I"E:\RT-ThreadStudio\workspace\ea1\rt-thread\libcpu\arm\cortex-m3" -include"E:\RT-ThreadStudio\workspace\ea1\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

