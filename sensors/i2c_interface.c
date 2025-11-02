#include "i2c_interface.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

void i2c_interface_init(i2c_port* port, unsigned int baudrate, unsigned int serial_clock_pin, unsigned int serial_data_pin) {
    i2c_init(port, baudrate);
    gpio_set_function(serial_clock_pin, GPIO_FUNC_I2C);
    gpio_pull_up(serial_clock_pin);
    gpio_set_function(serial_data_pin, GPIO_FUNC_I2C);
    gpio_pull_up(serial_data_pin);
}

void i2c_interface_read(i2c_port* port, unsigned int address, unsigned char* result, unsigned int result_size) {
    i2c_read_blocking(port, address, result, result_size, false);
}

void i2c_interface_write(i2c_port* port, unsigned int address, unsigned char* command, unsigned int command_size) {
    i2c_write_blocking(port, address, command, command_size, false);
}