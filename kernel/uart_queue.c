#include "uart_queue.h"

#include <stdint.h>

#include "kassert.h"
#include "portable.h"
#include "sync.h"
#include "syscall.h"
#include "task.h"

#define UART_QUEUE_SIZE 256u

static uint8_t tx_queue[UART_QUEUE_SIZE];
static uint32_t tx_head;
static uint32_t tx_tail;
static mutex_t tx_producer_mutex = {.locked = ATOMIC_FLAG_INIT, .task = 0};

static uint32_t Queue_Next(uint32_t index) {
  return (index + 1u) % UART_QUEUE_SIZE;
}

static void Queue_Write_Byte(uint8_t byte) {
  while (1) {
    uint32_t irq_state = pIRQ_Save();
    uint32_t next_tail = Queue_Next(tx_tail);
    if (next_tail != tx_head) {
      tx_queue[tx_tail] = byte;
      tx_tail = next_tail;
      pIRQ_Restore(irq_state);
      return;
    }
    pIRQ_Restore(irq_state);
    Yield();
  }
}

static int Queue_Peek_Byte(uint8_t *byte) {
  uint32_t irq_state = pIRQ_Save();
  if (tx_head == tx_tail) {
    pIRQ_Restore(irq_state);
    return 0;
  }
  *byte = tx_queue[tx_head];
  pIRQ_Restore(irq_state);
  return 1;
}

static void Queue_Consume_Byte(void) {
  uint32_t irq_state = pIRQ_Save();
  tx_head = Queue_Next(tx_head);
  pIRQ_Restore(irq_state);
}

static uint32_t Queue_Character(char c) {
  if (c == '\r') return 0;
  if (c == '\n') Queue_Write_Byte('\r');
  Queue_Write_Byte((uint8_t)c);
  return c == '\n' ? 2u : 1u;
}

static void Uart_Queue_Task(int argc, char **argv) {
  (void)argc;
  (void)argv;

  while (1) {
    uint8_t byte;
    if (!Queue_Peek_Byte(&byte)) {
      Yield();
      continue;
    }

    if (pUART_SendByte(byte) == 1) {
      Queue_Consume_Byte();
    } else {
      Yield();
    }
  }
}

void Uart_Queue_Init(void) {
  kassert(Task_Create("UART_Queue", Uart_Queue_Task) != 0);
}

uint32_t Uart_Queue_Write(const char *str) {
  if (str == 0) return 0;

  uint32_t amount = 0;
  mutex_lock(&tx_producer_mutex);
  for (uint32_t i = 0; str[i] != '\0'; i++) {
    amount += Queue_Character(str[i]);
  }
  mutex_unlock(&tx_producer_mutex);
  return amount;
}

uint32_t Uart_Queue_Write_Char(char c) {
  mutex_lock(&tx_producer_mutex);
  uint32_t amount = Queue_Character(c);
  mutex_unlock(&tx_producer_mutex);
  return amount;
}
