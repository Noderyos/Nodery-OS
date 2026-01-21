#include "sys/timer.h"

#include "sys/ports.h"

uint32_t tick_count = 0;

void handle_tick() {
    tick_count++;
    outb(PIC1_COMMAND, 0x20);
}

uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

double ts_per_ns = 0;

double nanotime() {
    return (double)rdtsc() / ts_per_ns;
}


int init_timer() {
    tick_count = 0;
    uint64_t start_ts = rdtsc();
    while (tick_count < 1000) {}
    uint64_t end_ts = rdtsc();
    ts_per_ns = (double)(end_ts-start_ts)/1e9;
    return 0;
}