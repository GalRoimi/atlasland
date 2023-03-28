ENTRY(vectors_start)

SECTIONS
{
    .init :
    {
        *(.init)
    }
    .text :
    {
        *(.text)
    }
    .rodata :
    {
        *(.rodata)
    }
    .data :
    { 
        *(.data)
    }
    .bss : 
    { 
         __bss_begin = .;
        *(.bss)
        __bss_end = .;
    }
    end = .;
    . = ALIGN(8);
    . = . + 0x1000; /* 4kB of stack memory */
    stack_top = .;
    . = . + 0x1000; /* 4kB of irq stack memory */
    irq_stack_top = .;
}
