#ifndef KERNEL_TYPEDEFS_H
#define KERNEL_TYPEDEFS_H
#include <stdint.h>

#include "portable/kernel_config.h"

/* Opaque storage for statically allocated mutexes. */
typedef struct mutex {
  uintptr_t _opaque[3];
} mutex_t;

/* Opaque storage for statically allocated spinlocks. */
typedef struct spinlock {
  uintptr_t _opaque[2];
} spinlock_t;

/* Opaque storage for statically allocated semaphores. */
typedef struct semaphore {
  uintptr_t _opaque[2];
} semaphore_t;

typedef enum {
  S_SUCCESS = 0,
  S_BUSY = 1,
  S_DEADTASK = 2,
  S_NOT_OWNER = 3,
  S_REENTER = 4,
} sync_result_t;

typedef struct task_args {
  int argc;
  char* argv[SHELL_MAX_ARGS];
} task_args_t;

#endif