#include "Util.h"

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
