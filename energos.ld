MEMORY
{
    RAM (wx) : ORIGIN = 0x20000000, LENGTH = (128K)
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 0x00080000
}

STACK_SIZE = 0x2000;

SECTIONS
{
    rom_start :
    {
        _isr_vector_start = .;
        *(.isr_vector)
        _isr_vector_end = .;
    } > FLASH

    .text :
    {
        *(.text)
    } > FLASH

    .bss (NOLOAD) :
    {
        *(.bss*)
        *(COMMON)
    } > RAM

    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        _stack_start = .;
        . += STACK_SIZE;
        _stack_end = .;
    } > RAM


}