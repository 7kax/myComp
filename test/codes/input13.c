void printint(long n);
int fred() { return (56); }

int main() {
    int dummy;
    int result;
    printint(23);
    result = fred();
    printint(result);

    return 0;
}
