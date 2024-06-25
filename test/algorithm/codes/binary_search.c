void printf(char *fmt);
void scanf(char *fmt);

int binary_search(int *arr, int l, int r, int x) {

    if (r >= l) {
        int mid;
        mid = l + (r - l) / 2;

        if (arr[mid] == x) {
            return mid;
        }

        if (arr[mid] > x) {
            return binary_search(arr, l, mid - 1, x);
        }

        return binary_search(arr, mid + 1, r, x);
    }

    return -1;
}

int do_binary_search(int *arr, int n, int x) {
    return binary_search(arr, 0, n - 1, x);
}

void print_result(int result) {
    if (result == -1) {
        printf("Not found\n");
    } else {
        printf("Found at index: %d\n", result);
    }
}

int main() {
    int arr[10];

    int i;
    for (i = 0; i < 10; i++) {
        scanf("%d", &arr[i]);
    }

    int target1, target2, target3;
    scanf("%d", &target1);
    scanf("%d", &target2);
    scanf("%d", &target3);
    int result1, result2, result3;
    result1 = do_binary_search(arr, 10, target1);
    result2 = do_binary_search(arr, 10, target2);
    result3 = do_binary_search(arr, 10, target3);

    print_result(result1);
    print_result(result2);
    print_result(result3);

    return 0;
}
