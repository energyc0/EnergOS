cmake_minimum_required(VERSION 3.31)
project(EnergOS C ASM)

# The toolchain is built specifically for the Cortex-M4
# with crosstool-ng
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(OBJCOPY arm-none-eabi-objcopy)
set(FIRMWARE_NAME energos)

set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

add_compile_options(-mcpu=${CMAKE_SYSTEM_PROCESSOR})
add_compile_options(-mthumb)
add_compile_options(-DSTM32F411xE -DDEBUG -DUSE_HAL_DRIVER)
add_compile_options(-fno-pic -fno-pie -nostdlib -ffreestanding)
add_compile_options(-Wall -Wunused-result -Werror -O0 -g3 -Og -Wno-main)
add_compile_options(-mfpu=fpv4-sp-d16 -mfloat-abi=hard)
add_compile_options(--specs=nano.specs --specs=nosys.specs)
add_compile_options(-Wl,--gc-sections)
include_directories(include)
include_directories(third_party/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(third_party/STM32CubeF4/Drivers/CMSIS/Core/Include)
include_directories(third_party/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc)
include_directories(third_party/STM32CubeF4/Drivers/BSP/STM32F4xx_Nucleo_32)

set(linker_script ${CMAKE_CURRENT_SOURCE_DIR}/energos.ld)
add_link_options(-T ${linker_script})
add_link_options(
    --specs=nosys.specs
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16)

FILE(GLOB hal_srcs 
third_party/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/*.c
third_party/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
)
list(FILTER hal_srcs EXCLUDE REGEX ".*_template\\.c$")
add_library(hal STATIC ${hal_srcs})

add_executable(${FIRMWARE_NAME}.elf ${FIRMWARE_NAME}.c startup.c)
target_link_libraries(${FIRMWARE_NAME}.elf PRIVATE hal)

add_custom_command(TARGET ${FIRMWARE_NAME}.elf POST_BUILD
COMMAND ${OBJCOPY} -O binary ${FIRMWARE_NAME}.elf ${FIRMWARE_NAME}.bin)

add_custom_target(flash COMMAND 
    openocd -f interface/stlink.cfg 
    -f target/stm32f4x.cfg -c "program ${FIRMWARE_NAME}.elf verify reset exit"
    DEPENDS ${FIRMWARE_NAME}.elf)

add_custom_target(debug_server COMMAND
    openocd -f interface/stlink.cfg -f target/stm32f4x.cfg)

add_custom_target(gdb COMMAND
    gdb-multiarch
    -ex "target extended-remote :3333" 
    -ex "monitor reset halt" 
    -ex "load" 
    -ex "break Reset_Handler"
    #-ex "break main" 
    #-ex "continue"
    ${CMAKE_BINARY_DIR}/${FIRMWARE_NAME}.elf)