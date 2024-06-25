void printint(long n);
char a;
char *b;
char c;

int d;
int *e;
int f;

int main() {
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
