set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/energos.ld)

set(KERNEL_SRC 
${FIRMWARE_NAME}.c 
startup.c 
uart.c 
)

add_executable(${FIRMWARE_NAME}.elf ${KERNEL_SRC})
target_include_directories(${FIRMWARE_NAME}.elf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${FIRMWARE_NAME}.elf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hal)
target_link_libraries(${FIRMWARE_NAME}.elf HAL)
target_link_options(${FIRMWARE_NAME}.elf PRIVATE
    --specs=nano.specs
    --specs=nosys.specs
    -mcpu=${CMAKE_SYSTEM_PROCESSOR}
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
    -Wl,--gc-sections
    -T ${LINKER_SCRIPT})

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
#-ex "break main" 
#-ex "continue"
${FIRMWARE_NAME}.elf)