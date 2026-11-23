add_qemu_test(port_tests
  "${CMAKE_CURRENT_SOURCE_DIR}/port_tests.c"
  "${PROJECT_SOURCE_DIR}/portable/${PLATFORM}/irq.c"
  "${PROJECT_SOURCE_DIR}/portable/${PLATFORM}/task.c"
)

add_qemu_test(syscall_tests
  "${CMAKE_CURRENT_SOURCE_DIR}/syscall_tests.c"
  "${PROJECT_SOURCE_DIR}/portable/${PLATFORM}/irq.c"
)