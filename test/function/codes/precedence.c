void printint(long n);

int main() {
    printint(5 * 3 + 6 / 3);
    printint(6 % 5 - 1 * 5);
    printint(1 << 1 + 1);
    printint(4 >> 1 + 1);
    printint(1 << 1 < 1);
    printint(1 << 1 > 1);
    printint(1 > 2 == 0);
    printint(1 < 2 != 1);
    printint(1 == 0 | 1);
    printint(1 != 0 & 1);
    printint(1 ^ 1 & 0);
    printint(0 && 1 || 1);

    return 0;
}