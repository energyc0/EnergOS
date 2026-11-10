#include "uart_queue.h"

#include <stdatomic.h>
#include <stdint.h>

#include "kassert.h"
#include "portable/scheduler.h"
#include "portable/uart.h"
#include "sync.h"
#include "task.h"
#include "user/sync.h"
#include "user/syscall.h"

#define UART_QUEUE_SIZE 256u

__attribute__((section(".user_data"))) static uint8_t tx_queue[UART_QUEUE_SIZE];
__attribute__((section(".user_data"))) static _Atomic uint32_t tx_head;
__attribute__((section(".user_data"))) static _Atomic uint32_t tx_tail;
__attribute__((section(".user_data"))) static mutex_t tx_producer_mutex =
    MUTEX_INIT();

__attribute__((section(".user_text"))) static uint32_t Queue_Next(
    uint32_t index) {
  return (index + 1u) % UART_QUEUE_SIZE;
}

__attribute__((section(".user_text"))) static void Queue_Write_Byte(
    uint8_t byte) {
  while (1) {
    uint32_t tail = atomic_load_explicit(&tx_tail, memory_order_relaxed);
    uint32_t next_tail = Queue_Next(tail);
    if (next_tail != atomic_load_explicit(&tx_head, memory_order_acquire)) {
      tx_queue[tail] = byte;
      atomic_store_explicit(&tx_tail, next_tail, memory_order_release);
      return;
    }
    Yield();
  }
}

static int Queue_Peek_Byte(uint8_t *byte) {
  uint32_t head = atomic_load_explicit(&tx_head, memory_order_relaxed);
  if (head == atomic_load_explicit(&tx_tail, memory_order_acquire)) return 0;
  *byte = tx_queue[head];
  return 1;
}

static void Queue_Consume_Byte(void) {
  uint32_t head = atomic_load_explicit(&tx_head, memory_order_relaxed);
  atomic_store_explicit(&tx_head, Queue_Next(head), memory_order_release);
}

__attribute__((section(".user_text"))) static uint32_t Queue_Character(char c) {
  if (c == '\r') return 0;
  if (c == '\n') Queue_Write_Byte('\r');
  Queue_Write_Byte((uint8_t)c);
  return c == '\n' ? 2u : 1u;
}

// Priviliged task
static void Uart_Queue_Task(void *param) {
  (void)param;

  while (1) {
    uint8_t byte;
    if (!Queue_Peek_Byte(&byte)) {
      pPend_Task_Switch();
      continue;
    }

    if (pUART_SendByte(byte) == 1) {
      Queue_Consume_Byte();
    } else {
      pPend_Task_Switch();
    }
  }
}

void Uart_Queue_Init(void) {
  kassert(Task_Create("UART_Queue", Uart_Queue_Task, 0, T_KERNEL) != 0);
}

__attribute__((section(".user_text"))) uint32_t
Uart_Queue_Write(const char *str) {
  if (str == 0) return 0;

  uint32_t amount = 0;
  mutex_lock(&tx_producer_mutex);
  for (uint32_t i = 0; str[i] != '\0'; i++) {
    amount += Queue_Character(str[i]);
  }
  mutex_unlock(&tx_producer_mutex);
  return amount;
}

__attribute__((section(".user_text"))) uint32_t Uart_Queue_Write_Char(char c) {
  mutex_lock(&tx_producer_mutex);
  uint32_t amount = Queue_Character(c);
  mutex_unlock(&tx_producer_mutex);
  return amount;
}
