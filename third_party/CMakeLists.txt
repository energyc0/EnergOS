FILE(GLOB hal_srcs 
STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/*.c
STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
)

list(FILTER hal_srcs EXCLUDE REGEX ".*_template\\.c$")
add_library(HAL STATIC ${hal_srcs})

target_compile_options(HAL PRIVATE -w)

target_include_directories(HAL PUBLIC
	STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include
	STM32CubeF4/Drivers/CMSIS/Core/Include
	STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc
	STM32CubeF4/Drivers/BSP/STM32F4xx_Nucleo_32
)

target_compile_definitions(HAL PUBLIC 
STM32F411xE 
USE_HAL_DRIVER 
BAUDRATE=${BAUDRATE})