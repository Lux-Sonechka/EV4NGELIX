ENTRY(_start)

SECTIONS
{
  . = 0x0; /* O header começa no início do arquivo */
  .header : {
    *(.header) /* Inclui a seção do header */
  }

  . = 0x1000; /* O código começa em 0x1000 */
  .text : {
    *(.text)
  }
  .rodata : {
    *(.rodata)
  }
  .data : {
    *(.data)
  }
  .bss : {
    *(.bss)
    *(COMMON)
  }
}