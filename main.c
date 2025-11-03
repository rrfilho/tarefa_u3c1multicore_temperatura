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
        leds_set_green(true);
        float temperature;
        temperature = humidity_sensor_temperature_read();
        threads_enqueue(temperature*1000);
        temperature = pressure_sensor_temperature_read();
        threads_enqueue(temperature*1000);
        leds_set_green(false);
    }
}

void thread1() {
    while (true) {
        leds_set_blue(true);
        sleep_ms(500);
        unsigned long temperature1 = threads_dequeue();
        display_set_x(temperature1/1000.f);
        unsigned long temperature2 = threads_dequeue();
        display_set_y(temperature2/1000.f);
        leds_set_blue(false);
        sleep_ms(500);
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