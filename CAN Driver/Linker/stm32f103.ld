/* ===== stm32f103.ld — STM32F103C8 (64KB Flash, 20KB RAM) ===== */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

ENTRY(Reset_Handler)

/* Initial SP */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  /* Vector table */
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  /* ---- Code + consts + lib tables + C init arrays (all BEFORE _etext) ---- */
  .text :
  {
    *(.text*)                  /* code */
    *(.rodata*)                /* const */

    /* tiny sections some libs emit */
    *(.init) *(.fini)

    /* unwind tables sometimes appear with nano/newlib */
    *(.ARM.extab*) *(.gnu.linkonce.armextab.*)
    *(.ARM.exidx*) *(.gnu.linkonce.armexidx.*)

    /* C runtime arrays in FLASH */
    . = ALIGN(4);
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);

    . = ALIGN(4);
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*))) KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);

    . = ALIGN(4);
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*))) KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);

    . = ALIGN(4);
    _etext = .;                /* LMA for .data starts AFTER everything above */
  } > FLASH

  /* ---- Initialized data in RAM (copied from FLASH at _etext) ---- */
  .data : AT(_etext)
  {
    _sidata = LOADADDR(.data); /* flash address for copy */

    _sdata = .;                /* start of .data in RAM */
    *(.data*)
    . = ALIGN(4);
    _edata = .;                /* end of .data in RAM */
  } > RAM

  /* ---- Zero-initialized data in RAM ---- */
  .bss (NOLOAD) :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;

    /* heap base symbols for sbrk/newlib */
    _end = .;
    PROVIDE(end = _end);
  } > RAM

  /* Discard notes/comments */
  /DISCARD/ : { *(.note*) *(.comment*) }
}
