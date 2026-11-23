cmake_minimum_required(VERSION 3.31)

find_program(QEMU ${QEMU_SYSTEM})
if(NOT QEMU)
  message(FATAL_ERROR "qemu-system-arm is required to run the tests")
endif()

set(QEMU_TEST_HARDWARE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/hardware/${PLATFORM}")

function(add_qemu_test target)
  add_executable(${target} ${ARGN} "${QEMU_TEST_HARDWARE_DIR}/startup.S")

  target_include_directories(${target} PRIVATE
    "${PROJECT_SOURCE_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}"
    "${PORT_DIR}"
    "${TESTS_DIR}"
  )

  target_link_libraries(${target} PRIVATE PORT_LIB USER HAL UTILITY ${TOOLCHAIN_RUNTIME_LIBRARY})

  target_compile_options(${target} PRIVATE
    -ffreestanding
    -fno-builtin
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    -Werror
  )

  target_link_options(${target} PRIVATE
    -nostdlib
    "-T${QEMU_TEST_HARDWARE_DIR}/${PLATFORM}.ld"
  )

  add_test(NAME ${target}
    COMMAND "${QEMU}"
      -machine ${QEMU_MACHINE_TEST}
      -cpu ${CMAKE_SYSTEM_PROCESSOR}
      -display none
      -serial none
      -monitor none
      -semihosting-config enable=on,target=native
      -kernel $<TARGET_FILE:${target}>
  )

endfunction()

enable_testing()

add_qemu_test(task_tests
  task_tests.c
  qemu_port.c
  "${PROJECT_SOURCE_DIR}/kernel/task.c"
  "${PROJECT_SOURCE_DIR}/kernel/sync.c"
)
target_compile_definitions(task_tests PRIVATE ENERGOS_QEMU)

add_qemu_test(sync_tests
  sync_tests.c
  qemu_port.c
  "${PROJECT_SOURCE_DIR}/kernel/sync.c"
)
target_compile_definitions(sync_tests PRIVATE ENERGOS_QEMU)

add_subdirectory(hardware)
