#pragma once

#include <stdatomic.h>
#include <stdint.h>

typedef struct task task_t;

typedef struct spinlock {
  atomic_flag locked;
  task_t* task;
} spinlock_t;

void lock(spinlock_t* lck);
void unlock(spinlock_t* lck);
