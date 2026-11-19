#include "printer.h"

#include "io.h"
#include "portable/time.h"
#include "sync.h"
#include "user/syscall.h"

mutex_t printer_mutex = MUTEX_INIT();
#define SLEEP_TASK_TIME (0xDEADBEEF)

static void Sleepy_Task(void* param) {
  (void)param;
  uint32_t tid = Get_Tid();
  while (1) {
    Write_Str("TID: ");
    Write_Int(tid);
    Write_Str(" Go to sleep...\n");
    Sleep(SLEEP_TASK_TIME);
    Write_Str("TID: ");
    Write_Int(tid);
    Write_Str(": I am awake!\n");
  }
}

void printer(void* msg_and_time) {
  if (msg_and_time) {
    uint32_t tid = Get_Tid();
    uint32_t sleepy_task_id = Create("Sleepy_Task", Sleepy_Task, 0);
    struct msg_and_time* arg = msg_and_time;
    while (1) {
      Sleep(arg->sleep_time);
      Mutex_Lock(&printer_mutex);
      uint32_t tick = Get_Tick();
      Write_Str("TID: ");
      Write_Int(tid);
      Write_Str(" Tick: ");
      Write_Int(tick);
      Write_Str("\nPRINTER (sleep for ");
      Write_Int(arg->sleep_time);
      Write_Str("): ");
      Write_Str(arg->msg);
      Write_Str("Try to wake the sleepy task...\n");
      Wakeup(sleepy_task_id);
      Mutex_Unlock(&printer_mutex);
    }
  }
}