#include "libc.c"

const char msg[] = "Hello, world!\n";

char buf[32];

void _start(void) {
    write(1, msg, 14);

    int f = open("HELLO.TXT", 0);
    if (f < 0) exit(1);
    uint32_t r = read(f, buf, 32);
    write(1, buf, r);

    close(f);
    exit(0);
}
