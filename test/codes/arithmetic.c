void printint(long n);

int main() {
    printint(1 + 2);
    printint(5 - 4);
    printint(7 * 8);
    printint(8 / 3);
    printint(8 % 3);
    printint(2 + 1 == 3);
    printint(1 + 1 != 8);
    printint(7 > 1);
    printint(7 < 1);
    printint(7 >= 7);
    printint(8 <= 8);
    printint(1 | 2);
    printint(3 & 2);
    printint(4 ^ 1);
    printint(~2);
    printint(1 << 5);
    printint(4 >> 1);
    printint(1 && 0 && 1);
    printint(0 || 0 || 1);
    printint(!6);
    printint(-5);
    printint(+9);

    return 0;
}
