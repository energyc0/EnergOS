#include <stdint.h>
#define GPIOA 0x40020000
#define AHB1PERIPH_BASE 0x40020000
#define RCC_OFFSET 0x00003800

#define GPIOA_BSRR (GPIOA + 0x18)
#define PA5 5

#define RCC_BASE (AHB1PERIPH_BASE + RCC_OFFSET)
#define RCC_AHB1ENR (RCC_BASE + 0x30)
#define GPIOAEN 0

#define GPIOA_MODER (GPIOA)
void main()
{
    *((volatile uint32_t*)RCC_AHB1ENR) |= (1 << GPIOAEN);

    *((volatile uint32_t*)GPIOA_MODER) &= ~(3U << (5 * 2)); 
    *((volatile uint32_t*)GPIOA_MODER) |=  (1U << (5 * 2));

    *((volatile uint32_t*)GPIOA_BSRR) = (1 << PA5);

    while(1);
}