void printint(long n);
int main() {
    int i;
    char j;

    j = 20;
    printint(j);
    i = 10;
    printint(i);

    for (i = 1; i <= 5; i++) {
        printint(i);
    }
    for (j = -2; j != 2; j++) {
        printint(j);
    }

    return 0;
}
