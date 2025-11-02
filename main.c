#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "libs/bootsel.h"
#include "libs/leds.h"
#include "libs/display.h"
#include "libs/threads.h"
#include "sensors/humidity_sensor.h"
#include "sensors/pressure_sensor.h"

void thread0() {
    while (true) {
        float temperature;
        temperature = humidity_sensor_temperature_read();
        threads_send_data_to_one(temperature*1000);
        temperature = pressure_sensor_temperature_read();
        threads_send_data_to_one(temperature*1000);
    }
}

bool are_temperatures_too_differents(float temperature1, float temperature2) {
    return abs(temperature1 - temperature2)/1000.f >= 0.4;
}

void thread1() {
    while (true) {
        unsigned long temperature1 = threads_receive_data_from_zero();
        display_set_x(temperature1/1000.f);
        unsigned long temperature2 = threads_receive_data_from_zero();
        display_set_y(temperature2/1000.f);
        leds_off();
        if (are_temperatures_too_differents(temperature1, temperature2)) leds_set_red(true);
        else leds_set_green(true);
    }
}

void init() {
    stdio_init_all();
    bootsel_init();
    leds_init();
    display_init();
    humidity_sensor_init();
    pressure_sensor_init();
    threads_init(thread0, thread1);
}

void main() {
    init();
    threads_start();
}