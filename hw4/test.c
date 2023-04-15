#include <stdio.h>

int main(int argc, char **argv) {
    const char hexstring[] = "f4800df8d1bc61fc95220645938cd65532a64067", *pos = hexstring;
    unsigned char val[20];

     /* WARNING: no sanitization or error-checking whatsoever */
    for (size_t count = 0; count < sizeof val/sizeof *val; count++) {
        sscanf(pos, "%2hhx", &val[count]);
        pos += 2;
    }

    printf("0x");
    for(size_t count = 0; count < sizeof val/sizeof *val; count++)
        printf("%02x", val[count]);
    printf("\n");

    return 0;
}