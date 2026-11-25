#include "w25q64.h"

#include <stdint.h>

#include "stm32f411xe.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include "util/util.h"

#define CMD_JEDEC_ID (0x9F)

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (hspi->Instance == SPI1) {
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;  // Alternate function, push-pull
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;  // AF number for SPI1
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

static void CS_Low(W25Q64_Handle *dev) {
  HAL_GPIO_WritePin(dev->cs_port, 1 << dev->cs_pin, GPIO_PIN_RESET);
}

static void CS_High(W25Q64_Handle *dev) {
  HAL_GPIO_WritePin(dev->cs_port, 1 << dev->cs_pin, GPIO_PIN_SET);
}

uint32_t W25Q64_Init(W25Q64_Handle *dev, SPI_HandleTypeDef *hspi,
                     SPI_TypeDef *spi, GPIO_TypeDef *cs_port, uint16_t cs_pin) {
  memset(hspi, 0, sizeof(*hspi));
  // APB2 Bus - 100 MHz
  hspi->Instance = spi;
  hspi->Init.Mode = SPI_MODE_MASTER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity = SPI_POLARITY_LOW;  // CS is low (NSS)
  hspi->Init.CLKPhase = SPI_PHASE_1EDGE;      // Rising edge
  hspi->Init.NSS = SPI_NSS_SOFT;              // Programmable CS
  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode = SPI_TIMODE_DISABLE;  // Motorolla SPI
  hspi->Init.CRCCalculation =
      SPI_CRCCALCULATION_DISABLE;  // Cyclic Redundancy Check - OFF
  hspi->Init.CRCPolynomial = 10;   // Standart polynomial

  // Return 0 on success, return 1 on failure
  if (HAL_SPI_Init(hspi) != HAL_OK) return 1;

  GPIO_InitTypeDef gpio_init = {0};
  gpio_init.Pin = 1 << cs_pin;
  gpio_init.Pull = GPIO_PULLUP;
  gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(cs_port, &gpio_init);

  dev->cs_port = cs_port;
  dev->cs_pin = cs_pin;
  dev->hspi = hspi;
  CS_High(dev);
  return 0;
}

uint32_t W25Q64_ReadJEDEC_ID(W25Q64_Handle *dev) {
  uint8_t buf[4];

  buf[0] = CMD_JEDEC_ID;
  CS_Low(dev);
  HAL_SPI_Transmit(dev->hspi, &buf[0], sizeof(uint8_t), HAL_MAX_DELAY);
  HAL_SPI_Receive(dev->hspi, &buf[1], sizeof(uint8_t) * 3, HAL_MAX_DELAY);
  CS_High(dev);

  return ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | buf[3];
}

void W25Q64_Read(W25Q64_Handle *dev, uint32_t addr, uint8_t *buf, uint32_t len);

void W25Q64_WritePage(W25Q64_Handle *dev, uint32_t addr, const uint8_t *buf,
                      uint16_t len);

void W25Q64_EraseSector(W25Q64_Handle *dev, uint32_t addr);