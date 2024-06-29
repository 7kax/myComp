void printf(char *fmt, ...);

int main() {
    int a;
    char b;
    long c;
    char *d;
    int *e;

    a = 255;
    b = 'a';
    c = 1234567890;
    d = "Hello, World!";
    e = &a;

    printf("a = %d\n", a);
    printf("b = %c\n", b);
    printf("c = %ld\n", c);
    printf("d = %s\n", d);
    printf("e = %d\n", *e);

    return 0;
}
