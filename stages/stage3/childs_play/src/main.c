#include <stdint.h>

static uint32_t secret = 0;

void foo(uint32_t* num)
{
    for (int i = 0; i < 0x239490; i += 2)
    {
        (*num)++;
    }
}

void goo()
{
    int blabla = 1234;

    for (int i = 0; i < 9393; i++)
    {
        blabla *= 8;
    }

    blabla /= 12;

    while (blabla % 32 > 12)
    {
        blabla++;
    }

    blabla ^= 0x123422;
}

uint32_t boo(uint32_t num)
{
    uint32_t var = num;

    while (var % 12 > 3)
    {
        var++;
    }

    var &= 0x123;

    while (num * 2 < 0x29392ab)
    {
        num *= 0x03;
    }

    var--;

    return num;
}

int main(void)
{
    foo(&secret);

    secret = secret % 0xaa432;

    goo();

    secret = boo(secret);

    foo(&secret);

    secret += 0x74820000;

    return 0;
}
