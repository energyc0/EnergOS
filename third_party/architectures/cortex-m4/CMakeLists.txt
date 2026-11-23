set(STM32CUBE_F4_ROOT "${THIRD_PARTY_DIR}/STM32CubeF4")

file(GLOB HAL_SOURCES
    "${STM32CUBE_F4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/*.c"
    "${STM32CUBE_F4_ROOT}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c"
)

list(FILTER HAL_SOURCES EXCLUDE REGEX ".*_template\\.c$")
add_library(HAL STATIC ${HAL_SOURCES})

target_compile_options(HAL PRIVATE -w)

target_include_directories(HAL PUBLIC
    ${PROJECT_SOURCE_DIR}
    "${PROJECT_SOURCE_DIR}/portable/${PLATFORM}"
    "${STM32CUBE_F4_ROOT}/Drivers/CMSIS/Device/ST/STM32F4xx/Include"
    "${STM32CUBE_F4_ROOT}/Drivers/CMSIS/Core/Include"
    "${STM32CUBE_F4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc"
    "${STM32CUBE_F4_ROOT}/Drivers/BSP/STM32F4xx_Nucleo_32"
)
