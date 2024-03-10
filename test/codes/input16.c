void printint(long n);
int c;
int d;
int *e;
int f;

int main() {
    c = 12;
    d = 18;
    printint(c);
    e = &d;
    f = *e;
    printint(f);
    return (0);
}
