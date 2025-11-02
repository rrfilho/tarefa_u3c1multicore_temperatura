#pragma once

typedef void (*thread)(void);
void threads_init(thread core0, thread core1);
void threads_start();
void threads_send_data_to_one(unsigned long data);
unsigned long threads_receive_data_from_zero();