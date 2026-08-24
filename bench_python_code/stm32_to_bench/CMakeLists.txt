cmake_minimum_required(VERSION 3.22)

# 设置工具链
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 工具链配置
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

# 项目设置
project(ocs8310_test_board C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# 构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

message("Build type: ${CMAKE_BUILD_TYPE}")

# 启用编译命令导出
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# MCU配置
set(MCU_FAMILY STM32G4xx)
set(MCU_MODEL STM32G431xx)
set(CPU_PARAMETERS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

# 编译选项
add_compile_options(
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -fdata-sections
    -ffunction-sections
    $<$<CONFIG:Debug>:-Og -g3 -ggdb>
    $<$<CONFIG:Release>:-O2>
)

# 链接选项
add_link_options(
    ${CPU_PARAMETERS}
    -T${CMAKE_SOURCE_DIR}/STM32G431XX_FLASH.ld
    -specs=nano.specs
    -lc
    -lm
    -lnosys
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

# 宏定义
add_compile_definitions(
    USE_HAL_DRIVER
    ${MCU_MODEL}
)

# 头文件路径
include_directories(
    Core/Inc
    Drivers/STM32G4xx_HAL_Driver/Inc
    Drivers/STM32G4xx_HAL_Driver/Inc/Legacy
    Drivers/CMSIS/Device/ST/STM32G4xx/Include
    Drivers/CMSIS/Include
    cmdline
    MDK-ARM/Code/head/0_user_h
    MDK-ARM/Code/head/1_motlib_h
)

# 源文件列表
set(SOURCES
    # 启动文件
    MDK-ARM/startup_stm32g431xx.s

    # Core源文件
    Core/Src/main.c
    Core/Src/gpio.c
    Core/Src/adc.c
    Core/Src/comp.c
    Core/Src/dma.c
    Core/Src/spi.c
    Core/Src/tim.c
    Core/Src/usart.c
    Core/Src/stm32g4xx_it.c
    Core/Src/stm32g4xx_hal_msp.c
    Core/Src/system_stm32g4xx.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c

    # HAL驱动源文件
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_comp.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_spi.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_spi_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_adc.c

    # cmdline模块
    cmdline/cmdline_internal.c
    cmdline/uart_cmd.c
    cmdline/utility.c
    cmdline/test.c

    # 用户代码 (0_user_s)
    MDK-ARM/Code/source/0_user_s/hard_init.c
    MDK-ARM/Code/source/0_user_s/mc_adc.c
    MDK-ARM/Code/source/0_user_s/user_control.c

    # 电机控制库 (1_motlib_s)
    MDK-ARM/Code/source/1_motlib_s/mc_control.c
)

# 创建可执行文件
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# 生成hex和bin文件
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMENT "生成 ${PROJECT_NAME}.hex 和 ${PROJECT_NAME}.bin"
)

# 显示大小信息
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "固件大小信息:"
)
