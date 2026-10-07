set(KERNEL_SRC 
${FIRMWARE_NAME}.c 
syscall.c
task.c
io.c
spinlock.c
)

add_executable(${FIRMWARE_NAME}.elf ${KERNEL_SRC})
target_include_directories(${FIRMWARE_NAME}.elf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${FIRMWARE_NAME}.elf PRIVATE ${CMAKE_SOURCE_DIR}/portable)
target_link_libraries(${FIRMWARE_NAME}.elf PORT_LIB)

add_custom_command(TARGET ${FIRMWARE_NAME}.elf POST_BUILD
COMMAND ${OBJCOPY} -O binary ${FIRMWARE_NAME}.elf ${FIRMWARE_NAME}.bin)

add_custom_target(flash COMMAND 
    openocd -f interface/stlink.cfg 
    -f target/stm32f4x.cfg -c "program ${FIRMWARE_NAME}.elf verify reset exit"
    DEPENDS ${FIRMWARE_NAME}.elf)

add_custom_target(gdb COMMAND
gdb-multiarch
-ex "target extended-remote :3333" 
-ex "monitor reset halt" 
-ex "load" 
-ex "break Reset_Handler"
${FIRMWARE_NAME}.elf)