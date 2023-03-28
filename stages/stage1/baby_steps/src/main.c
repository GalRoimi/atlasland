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
    print_uart0(PLAIN_MESSAGE);

    return 0;
}
