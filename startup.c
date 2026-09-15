#include <stdint.h>

extern int main(void);
extern char _stack_end;

void reset_handler()
{
    main();
}

__attribute__((section(".isr_vector")))
uint32_t* isr_vector[] = {
    (uint32_t*)&_stack_end,
    (uint32_t*)reset_handler,
};
