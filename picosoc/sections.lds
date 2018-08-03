
SECTIONS {
	.memory : {
		sram = 0;
		*(.data);
		*(.bss);
                *(.dynamic);
		_end = .; PROVIDE (end = .);
		. = 0x100000;
		start*(.text);
		*(.text);
		*(*);
	}
}
