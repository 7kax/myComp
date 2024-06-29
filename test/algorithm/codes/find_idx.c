void printf(char *fmt, ...);
int find_idx(int *arr, int n, int x);
void print_result(int result);

int main() {
    int arr[5];
    arr[0] = 1;

    int i;
    for (i = 1; i < 5; i++) {
        arr[i] = arr[i - 1] * 2;
    }

    int result1, result2;
    result1 = find_idx(arr, 5, 8);
    result2 = find_idx(arr, 5, 9);

    print_result(result1);
    print_result(result2);

    return 0;
}

int find_idx(int *arr, int n, int x) {
    int i;
    for (i = 0; i < n; i = i + 1) {
        if (arr[i] == x) {
            return i;
        }
    }
    return -1;
}

void print_result(int result) {
    if (result == -1) {
        printf("Not found\n");
    } else {
        printf("Found at index: %d\n", result);
    }
}