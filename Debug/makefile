################################################################################
# 自动生成的文件。不要编辑！
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include rt-thread/src/subdir.mk
-include rt-thread/libcpu/arm/cortex-m3/subdir.mk
-include rt-thread/libcpu/arm/common/subdir.mk
-include rt-thread/components/libc/compilers/newlib/subdir.mk
-include rt-thread/components/libc/compilers/common/subdir.mk
-include rt-thread/components/finsh/subdir.mk
-include rt-thread/components/drivers/serial/subdir.mk
-include rt-thread/components/drivers/misc/subdir.mk
-include rt-thread/components/drivers/ipc/subdir.mk
-include libraries/STM32F1xx_HAL_Driver/Src/Legacy/subdir.mk
-include libraries/STM32F1xx_HAL_Driver/Src/subdir.mk
-include libraries/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/subdir.mk
-include libraries/CMSIS/Device/ST/STM32F1xx/Source/Templates/subdir.mk
-include drivers/subdir.mk
-include cubemx/Src/subdir.mk
-include applications/ultrasonic/Src/subdir.mk
-include applications/task/subdir.mk
-include applications/switch/subdir.mk
-include applications/motor/Src/subdir.mk
-include applications/logic/subdir.mk
-include applications/gyro/Src/subdir.mk
-include applications/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
rtthread.bin \

SECONDARY_SIZE += \
rtthread.siz \


# 所有目标
all: rtthread.elf secondary-outputs

# 工具调用
rtthread.elf: $(OBJS) $(USER_OBJS)
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -T "E:\RT-ThreadStudio\workspace\Robtic\linkscripts\STM32F103ZE\link.lds" -Xlinker --gc-sections -Wl,-Map,"rtthread.map" -Xlinker --cref --specs=nano.specs -o "rtthread.elf" $(OBJS) $(USER_OBJS) $(LIBS)

rtthread.bin: rtthread.elf
	arm-none-eabi-objcopy -O binary "rtthread.elf"  "rtthread.bin"

rtthread.siz: rtthread.elf
	arm-none-eabi-size --format=berkeley "rtthread.elf"

# 其他目标
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS)$(CPP_DEPS) rtthread.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
