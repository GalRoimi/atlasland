ENTRY(entry_point)

SECTIONS
{
    . = 0x10000;
    .init :
    {
        __init_begin = .;
        *(.init)
        __init_end = .;
    }
    .text :
    {
        __text_begin = .;
        *(.text)
        __text_end = .;
    }
    .rodata :
    {
        __rodata_begin = .;
        *(.rodata)
        __rodata_end = .;
    }
    .data :
    {
        __data_begin = .;
        *(.data)
        __data_end = .;
    }
    .bss :
    {
        __bss_begin = .;
        *(.bss)
        *(.bss COMMON)
        __bss_end = .;
    }
    . = ALIGN(8);
    . = . + 0x1000; /* 4kB of stack memory */
    stack_top = .;

    . = 0xaf18dc6b;
    .secret : {
        *(.secret)
    }
}
