add_library(USER
elib.c
io.c
printer.c
shell.c
sync.c
syscall.c
)
target_include_directories(USER PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(USER PUBLIC UTILITY)