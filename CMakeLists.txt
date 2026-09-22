cmake_minimum_required(VERSION 3.31)
project(EnergOS C ASM)

# The toolchain is built specifically for the Cortex-M4
# with crosstool-ng
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(OBJCOPY arm-none-eabi-objcopy)
#set(CMAKE_LINKER arm-none-eabi-ld)

set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_POSITION_INDEPENDENT_CODE FALSE)

add_compile_options(-mcpu=${CMAKE_SYSTEM_PROCESSOR})
add_compile_options(-mthumb)
add_compile_options(-DSTM32F411xE -DDEBUG)
add_compile_options(-fno-PIC -nostdlib)
add_compile_options(-Wall -Wunused-result -Werror -O0 -ggdb -Wno-main)
include_directories(include)
include_directories(third_party/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(third_party/STM32CubeF4/Drivers/CMSIS/Core/Include)
include_directories(third_party/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc)
include_directories(third_party/STM32CubeF4/Drivers/BSP/STM32F4xx_Nucleo_32)

set(linker_script ${CMAKE_CURRENT_SOURCE_DIR}/energos.ld)
add_link_options(-T ${linker_script})
add_link_options(--specs=nosys.specs)

FILE(GLOB hal_srcs third_party/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/*.c)
list(FILTER hal_srcs EXCLUDE REGEX ".*_template\\.c$")
add_library(hal STATIC ${hal_srcs})

add_executable(energos.elf energos.c startup.c)
