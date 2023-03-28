#include "message.gen.h"

volatile unsigned int* const UART0DR = (unsigned int*)0x101f1000;

void print_uart0(const char* str)
{
    while(*str != '\0') {
        *UART0DR = (unsigned int)(*str);
        str++;
    }
}

int main(void)
{
    unsigned int pc = 0;
    asm("mov %0, pc" : "=r"(pc));
    if (pc >= 0x10000) {
        return 0;
    }

    print_uart0(PLAIN_MESSAGE);

    return 0;
}
