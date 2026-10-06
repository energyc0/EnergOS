# The toolchain is built specifically for the Cortex-M4
# with crosstool-ng
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_LINKER arm-none-eabi-ld)

set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

set(BAUDRATE 115200)

add_link_options(
    -mcpu=${CMAKE_SYSTEM_PROCESSOR}
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -Wl,--gc-sections
)

add_compile_options(
    -mcpu=${CMAKE_SYSTEM_PROCESSOR}
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -DSTM32F411xE
    -DUSE_HAL_DRIVER 
    -DBAUDRATE=${BAUDRATE})

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
