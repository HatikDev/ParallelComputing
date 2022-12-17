#include "Util.h"

namespace
{
int powerArray[] = { 256, 512, 1024, 2048, 4096, 8192, 16384 };
int host_maxPower(uint16_t number)
{
    for (int i = 7; i >= 0; --i)
    {
        if (number & powerArray[i])
            return i + 8;
    }

    return 0;
}
}

uint8_t host_plus(uint8_t num1, uint8_t num2)
{
    return num1 ^ num2;
}

uint8_t host_mult(uint8_t num1, uint8_t num2)
{
    uint16_t result = 0;

    for (int i = 0; i < 8; ++i)
    {
        if (!(num1 & 1))
        {
            num1 >>= 1;
            continue;
        }

        uint8_t num2_2 = num2;
        for (int j = 0; j < 8; ++j)
        {
            if (!(num2_2 & 1))
            {
                num2_2 >>= 1;
                continue;
            }

            result ^= (1 << (i + j));
            num2_2 >>= 1;
        }

        num1 >>= 1;
    }

    while (result > 255)
    {
        int power = host_maxPower(result);
        result ^= 0x11b << (power - 8);
    }

    return static_cast<uint8_t>(result);
}

size_t getRand(size_t min, size_t max)
{
    return (rand() % (max - min + 1)) + min;
}

int hex2Dec(char ch)
{
    if (ch >= 'a' && ch <= 'f')
        ch = ch - 'a' + 10;
    if (ch >= '0' && ch <= '9')
        ch -= '0';
    return ch;
}

int char2dec(char ch1, char ch2)
{
    return 16 * hex2Dec(ch1) + hex2Dec(ch2);
}
