void printint(long n);
int main() {
    int arr[5];
    arr[0] = 1;

    int i;
    for (i = 1; i < 5; i++) {
        arr[i] = arr[i - 1] * 2;
    }

    printint(arr[4]);

    return 0;
}
