void printint(long n);
int main() {
    char a;
    char *b;
    char c;

    int d;
    int *e;
    int f;

    a = 18;
    b = &a;
    c = *b;
    printint(c);

    d = 12;
    e = &d;
    f = *e;
    printint(f);

    return 0;
}
