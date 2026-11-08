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
  /* The startup code goes first into FLASH_KERNEL */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH_KERNEL

  /* The kernel program code and other data goes into FLASH_KERNEL */
  .kernel_text :
  {
    . = ALIGN(4);
    /* .text sections (code) */
    kernel/*.o(.text .text* .glue_7 .glue_7t .eh_frame)
    portable/*.o(.text .text* .glue_7 .glue_7t .eh_frame)
    third_party/*.o(.text .text* .glue_7 .glue_7t .eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
  } >FLASH_KERNEL

  /* The user program code and other data goes into FLASH_USER */
  .user_text :
    {
    . = ALIGN(4);
    /* .text sections (code) */
    user/*.o(.text .text* .glue_7 .glue_7t .eh_frame)
    util/*.o(.text .text* .glue_7 .glue_7t .eh_frame)

    . = ALIGN(4);
  } >FLASH_USER

  /* Constant kernel data goes into FLASH_KERNEL */
  .kernel_rodata :
  {
    . = ALIGN(4);
    kernel/*.o(.rodata .rodata*)         /* .rodata sections (constants, strings, etc.) */
    portable/*.o(.rodata .rodata*)         /* .rodata sections (constants, strings, etc.) */
    third_party/*.o(.rodata .rodata*)         /* .rodata sections (constants, strings, etc.) */

    . = ALIGN(4);
  } >FLASH_KERNEL

  /* Constant user data goes into FLASH_USER */
  .user_rodata :
  {
    . = ALIGN(4);
    user/*.o(.rodata .rodata*)         /* .rodata sections (constants, strings, etc.) */
    util/*.o(.rodata .rodata*)         /* .rodata sections (constants, strings, etc.) */

    . = ALIGN(4);
  } >FLASH_USER

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH_KERNEL
  .ARM : {
    __idx_start = .;
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
  _sikerneldata = LOADADDR(.kernel_data);
  _siuserdata = LOADADDR(.user_data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .kernel_data : 
  {
    . = ALIGN(4);
    _kernel_sdata = .;        /* create a global symbol at data start */
    kernel/*.o(.data .data*)
    portable/*.o(.data .data*)
    third_party/*.o(.data .data*)

    . = ALIGN(4);
    _kernel_edata = .;        /* define a global symbol at data end */
  } >RAM_KERNEL AT> FLASH_KERNEL

  .user_data : 
  {
    . = ALIGN(4);
    _user_sdata = .;        /* create a global symbol at data start */

    user/*.o(.data .data*)
    util/*.o(.data .data*)

    . = ALIGN(4);
    _user_edata = .;        /* define a global symbol at data end */
  } >RAM_USER AT> FLASH_USER
  
  /* Uninitialized data section */
  . = ALIGN(4);
  .kernel_bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __kernel_bss_start__ = _sbss;
    kernel/*.o (.bss .bss* COMMON)
    portable/*.o (.bss .bss* COMMON)
    third_party/*.o (.bss .bss* COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __kernel_bss_end__ = _ebss;
  } >RAM_KERNEL

  . = ALIGN(4);
  .user_bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    __user_bss_start__ = .;

    user/*.o (.bss .bss* COMMON)
    util/*.o (.bss .bss* COMMON)

    . = ALIGN(4);
    __user_bss_end__ = .;
  } >RAM_USER
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
    /*libgcc.a ( * )*/
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}