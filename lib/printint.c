#include <stdio.h>
#include <unistd.h>

void printint(long x) {
    printf("%ld\n", x);
}

void printchar(long x) {
    putc((char)(x & 0b1111111), stdout);
}
