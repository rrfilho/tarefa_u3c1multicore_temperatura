// sensor AHT10 - temperatura e humidade

#include "humidity_sensor.h"
#include <stdlib.h>
#include "pico/stdlib.h"
#include "sensors/i2c_interface.h"

#define AHT10_PORT i2c0
#define AHT10_BAUDRATE 400 * 1000 // 400 kHz
#define AHT10_SERIAL_DATA 0
#define AHT10_SERIAL_CLOCK 1
#define AHT10_ADDRESS 0x38

#define COMMAND_RESET 0xBA
#define COMMAND_INIT_1 0xBE
#define COMMAND_INIT_2 0x08
#define COMMAND_INIT_3 0x00
#define COMMAND_READ_1 0xAC
#define COMMAND_READ_2 0x33
#define COMMAND_READ_3 0x00

#define STATUS_BUSY 0x80
#define STATUS_CALIBRATED 0x08

static void reset() {
    unsigned char reset_command[] = {COMMAND_RESET};
    i2c_interface_write(AHT10_PORT, AHT10_ADDRESS, reset_command, 1);
    sleep_ms(20);
}

static void init() {
    unsigned char init_command[] = {COMMAND_INIT_1, COMMAND_INIT_2, COMMAND_INIT_3};
    i2c_interface_write(AHT10_PORT, AHT10_ADDRESS, init_command, 3);
    sleep_ms(50);
}

static bool is_calibrated(unsigned int status) {
    return (status & STATUS_CALIBRATED) == STATUS_CALIBRATED;
}

static bool is_ready() {
    unsigned char status;
    for (int i = 0; i < 10; i++) {
        i2c_interface_read(AHT10_PORT, AHT10_ADDRESS, &status, 1);
        if (is_calibrated(status)) return true;
        sleep_ms(10);
    }
    return false;
}

void humidity_sensor_init() {
    i2c_interface_init(AHT10_PORT, AHT10_BAUDRATE, AHT10_SERIAL_CLOCK, AHT10_SERIAL_DATA);
    reset();
    init();
    if (!is_ready()) exit(1);
}

static bool is_read_complete() {
    unsigned char status;
    for (int i = 0; i < 10; i++) {
        i2c_interface_read(AHT10_PORT, AHT10_ADDRESS, &status, 1);
        if (!(status & STATUS_BUSY)) break;
        busy_wait_ms(10);
    }
    return !(status & STATUS_BUSY);
}

static unsigned char* raw_read() {
    unsigned char read_command[] = {COMMAND_READ_1, COMMAND_READ_2, COMMAND_READ_3};
    static unsigned char raw_data[6];
    i2c_interface_write(AHT10_PORT, AHT10_ADDRESS, read_command, 3);
    if (!is_read_complete()) return raw_data;
    i2c_interface_read(AHT10_PORT, AHT10_ADDRESS, raw_data, 6);
    return raw_data;
}

float humidity_sensor_temperature_read() {
    unsigned char* raw_data = raw_read();
    unsigned long raw_temperature = ((unsigned long )(raw_data[3] & 0x0F) << 16) | ((unsigned long )raw_data[4] << 8) | raw_data[5];
    return ((float)raw_temperature * 200.0 / 1048576.0) - 50.0;
}

float humidity_sensor_humidity_read() {
    unsigned char* raw_data = raw_read();
    unsigned long  raw_humidity = ((unsigned long )raw_data[1] << 12) | ((unsigned long )raw_data[2] << 4) | (raw_data[3] >> 4);
    return (float)raw_humidity * 100.0 / 1048576.0;
}