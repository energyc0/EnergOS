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

set(BAUDRATE 115200)

add_compile_options(-mcpu=${CMAKE_SYSTEM_PROCESSOR})
add_compile_options(-mthumb)
add_compile_options(-DSTM32F411xE -DDEBUG -DUSE_HAL_DRIVER -DBAUDRATE=${BAUDRATE})
add_compile_options(-fno-pic -fno-pie -nostdlib -ffreestanding)
add_compile_options(-Wall -Wunused-result -Werror -O0 -g3 -Og -Wno-main)
add_compile_options(-mfpu=fpv4-sp-d16 -mfloat-abi=hard)

set(THIRD_PARTY ${PROJECT_SOURCE_DIR}/third_party)
set(KERNEL_DIR ${PROJECT_SOURCE_DIR}/kernel)

add_subdirectory(${THIRD_PARTY})
add_subdirectory(${KERNEL_DIR})

add_custom_target(debug_server COMMAND
    openocd -f interface/stlink.cfg -f target/stm32f4x.cfg)
