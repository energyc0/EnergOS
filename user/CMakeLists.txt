add_library(USER
elib.c
io.c
printer.c
shell.c
sync.c
syscall.c
)
target_include_directories(USER
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
PRIVATE ${KERNEL_DIR} ${PORT_DIR}
)
target_link_libraries(USER PUBLIC UTILITY)