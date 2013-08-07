MEMORY {
	FLASH (RX) : ORIGIN = 0x08000000, LENGTH = 64k
	RAM (RW) : ORIGIN = 0x20000000, LENGTH = 8k
}

SECTIONS {
	ENTRY(startup)

	_estack = ORIGIN(RAM) + LENGTH(RAM);

	.text : {
		KEEP(*(.nvic_table))

		*(.text*)

		*(.rodata*)

		_etext = .;
	} > FLASH

	.data : AT(_etext)
	{
		_data = .;
		*(.data)
		_edata = .;
	} > RAM

	.bss : {
		_bstart = .;
		*(.bss)
		*(COMMON)
		_bend = .;

	} > RAM
}
