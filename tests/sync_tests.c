#include <stdint.h>

#include "kernel/sync.h"
#include "kernel/task.h"
#include "kernel/typedefs.h"
#include "test_support.h"

task_t *current_task;
mutex_t tasks_mutex = MUTEX_INIT();
static task_t task_a;
static task_t task_b;
static mutex_t mutex = MUTEX_INIT();
static semaphore_t semaphore = SEMAPHORE_INIT(2);

task_t *Get_Current_Task(void) { return current_task; }

int main(void) {
  task_a.state = T_RUNNING;
  task_b.state = T_RUNNING;

  current_task = &task_a;
  TEST_CHECK(Kernel_Mutex_Lock(&mutex) == S_SUCCESS);
  TEST_CHECK(Kernel_Mutex_Lock(&mutex) == S_REENTER);

  current_task = &task_b;
  TEST_CHECK(Kernel_Mutex_Try_Lock(&mutex) == S_BUSY);
  TEST_CHECK(Kernel_Mutex_Unlock(&mutex) == S_NOT_OWNER);

  current_task = &task_a;
  TEST_CHECK(Kernel_Mutex_Unlock(&mutex) == S_SUCCESS);
  TEST_CHECK(Kernel_Mutex_Try_Lock(&mutex) == S_SUCCESS);
  TEST_CHECK(Kernel_Mutex_Unlock(&mutex) == S_SUCCESS);

  TEST_CHECK(Kernel_Semaphore_Wait(&semaphore) == S_SUCCESS);
  TEST_CHECK(Kernel_Semaphore_Wait(&semaphore) == S_SUCCESS);
  TEST_CHECK(Kernel_Semaphore_Signal(&semaphore) == S_SUCCESS);
  TEST_CHECK(Kernel_Semaphore_Signal(&semaphore) == S_SUCCESS);
  TEST_CHECK(Kernel_Semaphore_Signal(&semaphore) == S_SUCCESS);
  TEST_CHECK(Kernel_Semaphore_Wait(&semaphore) == S_SUCCESS);
  TEST_CHECK(Kernel_Semaphore_Wait(&semaphore) == S_SUCCESS);

  TEST_PASS("sync tests passed");
}
