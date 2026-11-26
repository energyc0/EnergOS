#ifndef W25Q64_H
#define W25Q64_H

#include <stdint.h>

#include "stm32f4xx_hal.h"

#define MEMORY_CAPACITY_BYTES (1 << 23)                           // 8 MB
#define PAGESIZE_BYTES (1 << 8)                                   // 256 bytes
#define PAGES_COUNT (MEMORY_CAPACITY_BYTES / PAGESIZE_BYTES)      // 32768
#define SECTOR_SIZE_BYTES (1 << 12)                               // 4 KB
#define SECTOR_COUNT (MEMORY_CAPACITY_BYTES / SECTOR_SIZE_BYTES)  // 2048
#define WEAR_LEVELING (500)  // wear leveling limit

#define STATIC_READ_BUF_SIZE PAGESIZE_BYTES
#define STATIC_PROG_BUF_SIZE PAGESIZE_BYTES
// lookahead buffer must be a multiple of 32, also it has uint32_t type
#define STATIC_LOOKAHEAD_BUF_SIZE (PAGESIZE_BYTES / sizeof(uint32_t))

#define W25Q64_JEDEC_ID (0xEF4017)

typedef struct {
  SPI_HandleTypeDef *hspi;
  GPIO_TypeDef *cs_port;
  uint16_t cs_pin;
} W25Q64_Handle;

/*
  Initialize W25Q64_Handle and SPI_HandleTypeDef with given SPI_TypeDef and pin.
  Pin must be configured before using W25Q64_Init()!
  Return 0 on success, return 1 on failure.
*/
uint32_t W25Q64_Init(W25Q64_Handle *dev, SPI_HandleTypeDef *hspi,
                     SPI_TypeDef *spi, GPIO_TypeDef *cs_port, uint16_t cs_pin);

// Reads JEDEC_ID that must be 0x00EF4017
uint32_t W25Q64_ReadJEDEC_ID(W25Q64_Handle *dev);

void W25Q64_Read(W25Q64_Handle *dev, uint32_t addr, uint8_t *buf, uint32_t len);

// Write operations be used after erase commands.

// Write page to the given address, if more length than 256 is given, then
// truncate. If len == 0, then write nothing.
void W25Q64_WritePage(W25Q64_Handle *dev, uint32_t addr, const uint8_t *buf,
                      uint16_t len);

// Write len bytes from buf to the given address.
// If len == 0, do nothing.
void W25Q64_Write(W25Q64_Handle *dev, uint32_t addr, const uint8_t *buf,
                  uint16_t len);

// Return 1 on write operation executing, return 0 when cycle is finished and
// ready to accept other commands again.
uint32_t W25Q64_Is_Busy(W25Q64_Handle *dev);

// Before erasing 4KB, 32KB or 64 KB need to wait for operation to complete.
// You need to use W25Q64_Is_Busy() while

void W25Q64_EraseSector4KB(W25Q64_Handle *dev, uint32_t addr);
void W25Q64_EraseSector32KB(W25Q64_Handle *dev, uint32_t addr);
void W25Q64_EraseSector64KB(W25Q64_Handle *dev, uint32_t addr);

#endif