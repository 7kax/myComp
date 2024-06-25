void printint(long n);
void printchar(long x);

int main() {
    char *str1;
    str1 = "Hello, World!";
    char str2[14];

    int i;
    for (i = 0; i < 14; i++) {
        str2[i] = str1[i];
    }

    for (i = 0; i < 13; i++) {
        printchar(str2[i]);
    }

    return 0;
}
