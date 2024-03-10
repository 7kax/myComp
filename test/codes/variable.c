void printint(long n);

int main() {
    int a, b, c, d, e;
    a = 1;
    b = c = ++a;
    d = b++;
    e = a + b * c / d;

    printint(a);
    printint(b);
    printint(c);
    printint(d);
    printint(e);

    return 0;
}
