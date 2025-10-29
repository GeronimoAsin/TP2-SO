void echo(const char *str, int fd) {
    write(fd, str, strlen(str));
    exit();
}