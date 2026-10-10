set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/stm32f411re.ld PARENT_SCOPE)
set(STM_CONFIG_DIR ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)

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