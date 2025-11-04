#include <stdio.h>
#include <qrencode.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <data_to_encode>\n", argv[0]);
        return 1;
    }

    QRcode *qrcode = QRcode_encodeString(argv[1], 0, QR_ECLEVEL_L, QR_MODE_8, 0);

    if (qrcode == NULL)
    {
        printf("Failed to encode the string into QR code.\n");
        return 1;
    }

    // Print ASCII version
    for (int y = 0; y < qrcode->width; y++)
    {
        for (int x = 0; x < qrcode->width; x++)
        {
            putchar((qrcode->data[y * qrcode->width + x] & 1) ? '#' : ' ');
            putchar(' ');
        }
        putchar('\n');
    }

    QRcode_free(qrcode);

    return 0;
}
