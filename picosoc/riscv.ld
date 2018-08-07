/* ---- Original Script: /opt/riscv32i/riscv32-unknown-elf/lib/ldscripts/elf32lriscv.x ---- */
/* Default linker script, for normal executables */
/* Copyright (C) 2014-2017 Free Software Foundation, Inc.
   Copying and distribution of this script, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  */
OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv",
	      "elf32-littleriscv")

OUTPUT_ARCH(riscv)

MEMORY {
  ram	(!rx)	: org = 0, l = 4k
  flash	(rx)	: org = 0x100000, l = 512k /* 1<<(10 + 9) */
  rom	(!rx)	: org = 0x180000, l = 4k /* 1<<20 + 1<<19 */
}

SECTIONS
{
  .data 0x10 : AT ( 0x180000 ) {
    _data = .; PROVIDE (data = .);
    *(.data);
    _edata = .; PROVIDE (edata = .);
    _end = .; PROVIDE (end = .);
    __global_pointer$ = . ;
  } > ram
  
  .bss : {
    *(.bss);
    *(.dynamic);
  } > ram

  .sbss : {
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
  } > ram

  .text1 0x100000 : {
    start*(.text);
  } > flash
  .text2 : {
    *(.text);
    *(.sdata);
    *(.sdata2);
    *(.gcc_except_table);
    *(*);
  } > flash

  /* .preinit_array     : */
  /* { */
  /*   PROVIDE_HIDDEN (__preinit_array_start = .); */
  /*   KEEP (*(.preinit_array)) */
  /*   PROVIDE_HIDDEN (__preinit_array_end = .); */
  /* } */
  /* .init_array     : */
  /* { */
  /*   PROVIDE_HIDDEN (__init_array_start = .); */
  /*   KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*))) */
  /*   KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors)) */
  /*   PROVIDE_HIDDEN (__init_array_end = .); */
  /* } */
  /* .fini_array     : */
  /* { */
  /*   PROVIDE_HIDDEN (__fini_array_start = .); */
  /*   KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*))) */
  /*   KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors)) */
  /*   PROVIDE_HIDDEN (__fini_array_end = .); */
  /* } */
  /* .ctors          : */
  /* { */
  /*   KEEP (*crtbegin.o(.ctors)) */
  /*   KEEP (*crtbegin?.o(.ctors)) */
  /*   /\* We don't want to include the .ctor section from */
  /*      the crtend.o file until after the sorted ctors. */
  /*      The .ctor section from the crtend file contains the */
  /*      end of ctors marker and it must be last *\/ */
  /*   KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors)) */
  /*   KEEP (*(SORT(.ctors.*))) */
  /*   KEEP (*(.ctors)) */
  /* } */
  /* .dtors          : */
  /* { */
  /*   KEEP (*crtbegin.o(.dtors)) */
  /*   KEEP (*crtbegin?.o(.dtors)) */
  /*   KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors)) */
  /*   KEEP (*(SORT(.dtors.*))) */
  /*   KEEP (*(.dtors)) */
  /* } */
}
