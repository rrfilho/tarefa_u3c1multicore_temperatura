#pragma once

typedef void (*thread)(void);
void threads_init(thread core0, thread core1);
void threads_start();
void threads_enqueue(unsigned long data);
unsigned long threads_dequeue();