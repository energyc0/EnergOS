set(KERNEL_SRC 
${FIRMWARE_NAME}.c 
syscall.c
task.c
uart_queue.c
sync.c
slab.c
)

add_executable(${FIRMWARE_NAME}.elf ${KERNEL_SRC})
# use "-Wl,--orphan-handling=warn" to warn about orphan sections,
# or  "-Wl,--orphan-handling=place" to place them at the end of the output file
target_link_options(${FIRMWARE_NAME}.elf PRIVATE
    "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${FIRMWARE_NAME}.map"
    "-Wl,--orphan-handling=place"
)

target_include_directories(${FIRMWARE_NAME}.elf PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(${FIRMWARE_NAME}.elf PORT_LIB USER HAL UTILITY gcc)

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