OUTPUT_ARCH(riscv32)
OUTPUT_FORMAT(binary)
ENTRY(main)

SECTIONS {
	. = 0;
	.text : { *(.pretext); *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) *(.sdata) }
	.bss : { *(.bss) *(.sbss) }
}
