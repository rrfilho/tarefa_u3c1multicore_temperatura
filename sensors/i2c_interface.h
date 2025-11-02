#pragma once

#include "hardware/i2c.h"

typedef struct i2c_inst i2c_port;

void i2c_interface_init(i2c_port* port, unsigned int baudrate, unsigned int serial_clock_pin, unsigned int serial_data_pin);
void i2c_interface_read(i2c_port* port, unsigned int address, unsigned char* result, unsigned int result_size);
void i2c_interface_write(i2c_port* port, unsigned int address, unsigned char* command, unsigned int command_size);