void printint(long n);
int main() {
    int i, j, k;
    i = 1;
    j = 2;
    k = 3;

    if (i > j) {
        printint(i);
    } else {
        if (j > k) {
            printint(j);
        } else {
            printint(k);
        }
    }

    return 0;
}
