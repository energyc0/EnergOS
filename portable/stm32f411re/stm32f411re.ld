INCLUDE memory_map.ld

/* Entry Point */
ENTRY(Reset_Handler)

/* Specify the memory areas */
MEMORY
{
  FLASH_KERNEL (rx) : ORIGIN = FLASH_KERNEL_BASE, LENGTH = FLASH_KERNEL_SIZE
  FLASH_USER (rx) : ORIGIN = FLASH_USER_BASE, LENGTH = FLASH_USER_SIZE

  RAM_KERNEL (rw) : ORIGIN = SRAM_KERNEL_BASE, LENGTH = SRAM_KERNEL_SIZE
  RAM_USER (rw) : ORIGIN = SRAM_USER_BASE, LENGTH = SRAM_USER_SIZE

  PERIPH (rw)    : ORIGIN = PERIPH_BASE, LENGTH = PERIPH_SIZE
  PRIV_PERIPH (rw) : ORIGIN = PRIV_PERIPH_BASE, LENGTH = PRIV_PERIPH_SIZE
}

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM_KERNEL) + LENGTH(RAM_KERNEL);    /* end of RAM */

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH_KERNEL

  /* The program code and other data goes into FLASH */
  .kernel_text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.kernel_text)    /* .kernel_text sections (code) */
    *(.kernel_text*)   /* .kernel_text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
  } >FLASH_KERNEL

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH_KERNEL

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH_KERNEL
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH_KERNEL

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH_KERNEL
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH_KERNEL
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH_KERNEL

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM_KERNEL AT> FLASH_KERNEL

  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM_KERNEL

  /* User_heap_stack section, used to check that there is enough RAM left */
  /*._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >RAM */

  
  __kernel_text_start = ORIGIN(FLASH_KERNEL);
  __kernel_text_end   = ORIGIN(FLASH_KERNEL) + LENGTH(FLASH_KERNEL);
  __kernel_ram_start  = ORIGIN(RAM_KERNEL);
  __kernel_ram_end    = ORIGIN(RAM_KERNEL) + LENGTH(RAM_KERNEL);

  __user_text_start   = ORIGIN(FLASH_USER);
  __user_text_end     = ORIGIN(FLASH_USER) + LENGTH(FLASH_USER);
  __user_ram_start    = ORIGIN(RAM_USER);
  __user_ram_end      = ORIGIN(RAM_USER) + LENGTH(RAM_USER);

  __periph_start      = ORIGIN(PERIPH);
  __periph_end        = ORIGIN(PERIPH) + LENGTH(PERIPH);

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}