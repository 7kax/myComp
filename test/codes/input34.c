void printint(long n);
int main() {
    int a[12];
    a[0] = 256;
    printint(a[0]);
    a[11] = -5;
    printint(a[0] + a[11]);
    return 0;
}
