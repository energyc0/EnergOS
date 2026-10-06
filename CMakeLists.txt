cmake_minimum_required(VERSION 3.31)
project(EnergOS C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_definitions(DEBUG)
endif()

message(STATUS "Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "System processor: ${CMAKE_SYSTEM_PROCESSOR}")
set(FIRMWARE_NAME energos)
set(MCU_DEVICE_FILE /dev/ttyACM0)

#-ffunction-sections - for dead code elimination
add_compile_options(-Wall -Wextra -Werror -Wunused-result -Og -Wno-main -O0 -g3 -ffunction-sections -fdata-sections -fno-pic -fno-pie -ffreestanding)
add_link_options(-nostdlib)

set(THIRD_PARTY ${PROJECT_SOURCE_DIR}/third_party)
set(PORT_DIR ${PROJECT_SOURCE_DIR}/portable)
set(KERNEL_DIR ${PROJECT_SOURCE_DIR}/kernel)

add_subdirectory(${THIRD_PARTY})
add_subdirectory(${PORT_DIR})
if(NOT LINKER_SCRIPT)
    message(FATAL_ERROR "The platform did not provide a linker script")
endif()
message(STATUS "Linker script: ${LINKER_SCRIPT}")
add_link_options("-T${LINKER_SCRIPT}")

add_subdirectory(${KERNEL_DIR})

add_custom_target(debug_server COMMAND
    openocd -f interface/stlink.cfg -f target/stm32f4x.cfg)

add_custom_target(minicom COMMAND
    minicom -D ${MCU_DEVICE_FILE} -b ${BAUDRATE})
    