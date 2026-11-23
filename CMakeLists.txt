cmake_minimum_required(VERSION 3.31)
project(EnergOS C ASM)

message(STATUS "Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "System processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Linker script: ${LINKER_SCRIPT}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Adding debug flags.")
    add_compile_definitions(DEBUG)
    add_compile_options(-Og -g3)
endif()

if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE is not set.")
endif()

if(NOT EXISTS ${CMAKE_TOOLCHAIN_FILE})
    message(FATAL_ERROR "Toolchain file not found.")
endif()

if(NOT LINKER_SCRIPT)
    message(FATAL_ERROR "Linker script file not found.")
endif()

if (NOT PLATFORM)
  message(FATAL_ERROR "PLATFORM variable is not set.")
endif()

set(FIRMWARE_NAME energos)

find_program(MINICOM_EXECUTABLE minicom)
find_program(OPENOCD_EXECUTABLE openocd)
find_program(GDB_EXECUTABLE gdb-multiarch)

#-ffunction-sections - for dead code elimination
add_compile_options(-Wall -Wextra -Werror -Wunused-result -Wno-main -O0 -ffunction-sections -fdata-sections -fno-pic -fno-pie -ffreestanding)
add_link_options(-nostdlib)

set(PLATFORM_DIR ${PROJECT_SOURCE_DIR}/portable/${PLATFORM})
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(PORT_DIR ${PROJECT_SOURCE_DIR}/portable)
set(KERNEL_DIR ${PROJECT_SOURCE_DIR}/kernel)
set(USER_DIR ${PROJECT_SOURCE_DIR}/user)
set(UTIL_DIR ${PROJECT_SOURCE_DIR}/util)
set(TOOLCHAIN_DIR ${PROJECT_SOURCE_DIR}/toolchain)
set(TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)
set(LINKER_SCRIPT ${TOOLCHAIN_DIR}/${PLATFORM}.ld)

add_subdirectory(${THIRD_PARTY_DIR})
add_subdirectory(${UTIL_DIR})
add_subdirectory(${PORT_DIR})
add_subdirectory(${USER_DIR})
add_subdirectory(${KERNEL_DIR})
if (BUILD_TESTING)
    add_subdirectory(${TESTS_DIR})
endif()

add_custom_target(minicom COMMAND
    ${MINICOM_EXECUTABLE} -D ${MCU_DEVICE_FILE} -b ${BAUDRATE})
    