void printint(long n);
int main() {
    int a, b, c, d, e;
    a = 1;
    b = a != 1;
    c = b == 0;
    d = c || 0;
    e = d && 1;

    printint(a);
    printint(b);
    printint(c);
    printint(d);
    printint(e);

    return 0;
}
