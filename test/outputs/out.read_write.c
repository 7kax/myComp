int open(char *pathname, int flags);
int read(int fd, char *buf, int count);
int write(int fd, void *buf, int count);
int close(int fd);

char buf[60];

int main() {
    int zin;
    int cnt;

    zin = open("./codes/read_write.c", 0);
    if (zin == -1) {
        return (1);
    }
    while ((cnt = read(zin, buf, 60)) > 0) {
        write(1, buf, cnt);
    }
    close(zin);
    return (0);
}
