#define NULL ((void*)0)

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;

uint32_t write(int fildes, const void *buf, uint32_t nbyte) {
    uint32_t ret;
    asm volatile (
        "int $0x69"
        : "=a"(ret)
        : "a"(4), "b"(fildes), "c"(buf), "d"(nbyte)
        : "memory"
    );
    return ret;
}

uint32_t read(int fildes, void *buf, uint32_t nbyte) {
    uint32_t ret;
    asm volatile (
        "int $0x69"
        : "=a"(ret)
        : "a"(3), "b"(fildes), "c"(buf), "d"(nbyte)
        : "memory"
    );
    return ret;
}

int open(const char *path, int oflag) {
    int ret;
    asm volatile (
        "int $0x69"
        : "=a"(ret)
        : "a"(5), "b"(path), "c"(oflag)
        : "memory"
    );
    return ret;
}

int close(int fildes) {
    int ret;
    asm volatile (
        "int $0x69"
        : "=a"(ret)
        : "a"(6), "b"(fildes)
        : "memory"
    );
    return ret;
}

int exit(int exit_code) {
    int ret;
    asm volatile (
        "int $0x69"
        : "=a"(ret)
        : "a"(1), "b"(exit_code)
        : "memory"
    );
    return ret;
}

