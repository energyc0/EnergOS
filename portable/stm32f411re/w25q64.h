#ifndef W25Q64_H
#define W25Q64_H

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

typedef struct {
  SPI_HandleTypeDef *hspi;
  GPIO_TypeDef *cs_port;
  uint16_t cs_pin;
} W25Q64_Handle;

// Return 0 on success, return 1 on failure
uint32_t W25Q64_Init(W25Q64_Handle *dev, SPI_HandleTypeDef *hspi,
                     SPI_TypeDef *spi, GPIO_TypeDef *cs_port, uint16_t cs_pin);

uint32_t W25Q64_ReadJEDEC_ID(W25Q64_Handle *dev);

void W25Q64_Read(W25Q64_Handle *dev, uint32_t addr, uint8_t *buf, uint32_t len);

void W25Q64_WritePage(W25Q64_Handle *dev, uint32_t addr, const uint8_t *buf,
                      uint16_t len);

void W25Q64_EraseSector(W25Q64_Handle *dev, uint32_t addr);

#endif