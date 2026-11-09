#ifndef KERNEL_TYPEDEFS_H
#define KERNEL_TYPEDEFS_H
#include <stdint.h>

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

#endif