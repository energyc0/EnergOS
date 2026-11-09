#ifndef USER_SYNC_H
#define USER_SYNC_H

#include <stdint.h>

#include "kernel/typedefs.h"

#define MUTEX_INIT() {0}

void mutex_lock(mutex_t* mtx);
void mutex_unlock(mutex_t* mtx);
// Returns 1 if the mutex was successfully locked, 0 otherwise
int mutex_try_lock(mutex_t* mtx);

#endif