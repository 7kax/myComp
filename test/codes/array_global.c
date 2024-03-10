void printint(long n);

int a[100];

int main() {
    int *b;

    a[66] = 18;
    b = &a[66];
    printint(*b);

    return 0;
}