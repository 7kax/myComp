void printint(long n);

int main() {
    int a[100];
    int *b;

    a[66] = 18;
    b = &a[66];
    printint(*b);

    return 0;
}
