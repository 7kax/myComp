void printint(long n);

int main() {
    int arr[5];
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;
    arr[4] = 5;

    int max, i;
    max = -1;

    for (i = 0; i < 5; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }

    printint(max);

    return 0;
}