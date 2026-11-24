#include <stdint.h>

#include "portable/irq.h"
#include "portable/scheduler.h"
#include "portable/time.h"
#include "portable/uart.h"

#ifndef ENERGOS_QEMU
#error "qemu_port.c must only be compiled for QEMU tests"
#endif

static uint32_t Semihosting_Call(uint32_t operation, const void *argument) {
  register uint32_t r0 __asm("r0") = operation;
  register const void *r1 __asm("r1") = argument;
  __asm volatile("bkpt 0xab" : "+r"(r0) : "r"(r1) : "memory");
  return r0;
}

uint32_t pIRQ_Save(void) { return 0; }
void pIRQ_Restore(uint32_t state) { (void)state; }
void pDisable_IRQ(void) {}
void pEnable_IRQ(void) {}

void pScheduler_Init(void) {}
void pStart_First_Task(void) {}
void pJump_First_Task(void) {}
void pSet_Current_Task_Privilege(void) {}
void pPend_Task_Switch(void) {}

uint32_t pGet_Tick(void) { return 0; }
uint32_t pSetup_Timer_IRQ(void) { return 1; }

uint32_t pUart_Send(const uint8_t *data, uint32_t length) {
  if (data == 0 || length == 0) return 0;
  const uint32_t block[] = {1, (uint32_t)data, length};
  uint32_t unsent = Semihosting_Call(0x05, block);
  return unsent <= length ? length - unsent : 0;
}

uint32_t pUART_SendString(const uint8_t *str) {
  if (str == 0) return 0;
  uint32_t length = 0;
  while (str[length] != '\0') length++;
  return Semihosting_Call(0x04, str) == 0 ? length : 0;
}

uint32_t pUART_SendByte(uint8_t byte) {
  Semihosting_Call(0x03, &byte);
  return 1;
}

uint32_t pUART_Receive(uint8_t *buf, uint32_t size, uint32_t timeout_ms) {
  (void)buf;
  (void)size;
  (void)timeout_ms;
  return 0;
}

static uint32_t Send_Integer(uint32_t value, uint32_t radix, uint8_t prefix) {
  uint8_t buffer[12];
  uint32_t length = 0;
  do {
    uint32_t digit = value % radix;
    buffer[length++] =
        digit < 10 ? (uint8_t)('0' + digit) : (uint8_t)('A' + digit - 10);
    value /= radix;
  } while (value != 0);

  if (prefix != 0) buffer[length++] = prefix;
  for (uint32_t i = 0; i < length / 2; i++) {
    uint8_t temp = buffer[i];
    buffer[i] = buffer[length - i - 1];
    buffer[length - i - 1] = temp;
  }
  buffer[length] = '\0';
  return pUART_SendString(buffer);
}

uint32_t pUart_Send_Hex(uint32_t value) {
  uint8_t buffer[] = "0x00000000";
  for (uint32_t i = 0; i < 8; i++) {
    uint32_t digit = (value >> (4 * (7 - i))) & 0xFu;
    buffer[i + 2] = (uint8_t)(digit < 10 ? '0' + digit : 'A' + digit - 10);
  }
  return pUART_SendString(buffer);
}

uint32_t pUart_Send_Int(int32_t value) {
  uint32_t magnitude = value < 0 ? 0u - (uint32_t)value : (uint32_t)value;
  return Send_Integer(magnitude, 10, value < 0 ? '-' : 0);
}

void Uart_Queue_Init(void) {}
