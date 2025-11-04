#include <stdio.h>
#include <qrencode.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <data_to_encode>\n", argv[0]);
        return 1;
    }

    return 0;
}
