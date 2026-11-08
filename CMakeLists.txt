cmake_minimum_required(VERSION 3.31)
project(EnergOS C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_definitions(DEBUG)
    add_compile_options(-Og -g3)
endif()

if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Toolchain file not found.")
endif()

if(NOT LINKER_SCRIPT)
    message(FATAL_ERROR "Linker script file not found.")
endif()

message(STATUS "Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "System processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Linker script: ${LINKER_SCRIPT}")
set(FIRMWARE_NAME energos)
set(MCU_DEVICE_FILE /dev/ttyACM0)

#-ffunction-sections - for dead code elimination
add_compile_options(-Wall -Wextra -Werror -Wunused-result -Wno-main -O0 -ffunction-sections -fdata-sections -fno-pic -fno-pie -ffreestanding)
add_link_options(-nostdlib)

set(THIRD_PARTY ${PROJECT_SOURCE_DIR}/third_party)
set(PORT_DIR ${PROJECT_SOURCE_DIR}/portable)
set(KERNEL_DIR ${PROJECT_SOURCE_DIR}/kernel)
set(USER_DIR ${PROJECT_SOURCE_DIR}/user)
set(UTIL_DIR ${PROJECT_SOURCE_DIR}/util)
set(TOOLCHAIN_DIR ${PROJECT_SOURCE_DIR}/toolchain)

add_subdirectory(${THIRD_PARTY})
add_subdirectory(${UTIL_DIR})
add_subdirectory(${PORT_DIR})
add_subdirectory(${USER_DIR})
add_subdirectory(${KERNEL_DIR})

add_custom_target(minicom COMMAND
    minicom -D ${MCU_DEVICE_FILE} -b ${BAUDRATE})
    