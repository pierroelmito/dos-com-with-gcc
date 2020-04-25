
OUTPUT_FORMAT(pe-i386)

SECTIONS
{
    . = 0x0100;
    .text ALIGN(1):
    {
        *(.text);
    }
    .data ALIGN(1):
    {
        *(.bss);
        *(.rodata);
        *(.data);
    }
    .rdata ALIGN(1):
    {
        *(.rdata);
    }
    _heap = ALIGN(16);
}

