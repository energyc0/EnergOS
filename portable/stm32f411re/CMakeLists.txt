set(LINKER_SCRIPT ${TOOLCHAIN_DIR}/stm32f411re.ld PARENT_SCOPE)

message(STATUS "Current linker script directory: ${CMAKE_CURRENT_LIST_DIR}")

add_library(PORT_LIB
context_switch.S
irq.c
startup.c
task.c
timer.c
uart.c
)

target_include_directories(PORT_LIB PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${PORT_DIR})
target_include_directories(PORT_LIB PRIVATE ${KERNEL_DIR})
target_include_directories(HAL PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(PORT_LIB PRIVATE HAL)
target_link_libraries(PORT_LIB PUBLIC UTILITY)

add_custom_target(debug_server COMMAND
    openocd -f interface/stlink.cfg -f target/stm32f4x.cfg)