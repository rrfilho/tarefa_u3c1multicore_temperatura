#include "threads.h"
#include "pico/multicore.h"

static thread _core0;
static thread _core1;

void threads_init(thread core0, thread core1) {
    _core0 = core0;
    _core1 = core1;
}

void threads_start() {
    multicore_launch_core1(_core1);
    _core0();
}

void threads_enqueue(unsigned long data) {
    multicore_fifo_push_blocking(data);
}

unsigned long threads_dequeue() {
    return multicore_fifo_pop_blocking();
}