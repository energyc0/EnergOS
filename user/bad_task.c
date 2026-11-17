#include "bad_task.h"

#include "io.h"

void Bad_Task(void *param) {
  (void)param;
  Write_Str("Bad task tryes to write to NULL pointer!\n");
  // NOLINTNEXTLINE(clang-analyzer-core.NullDereference)
  *(volatile uint32_t *)0 = 0xDEADBEEF;
  Write_Str("MPU works wrong! :)\n");
  while (1);
}