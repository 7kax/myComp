void printint(long n);

int main() {
    int a, b, c, d, e;
    a = 1;
    b = ++a;
    c = a << b;
    d = c & b;
    e = ~d;

    printint(a);
    printint(b);
    printint(c);
    printint(d);
    printint(e);

    return 0;
}
