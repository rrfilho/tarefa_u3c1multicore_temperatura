#include "leds.h"
#include <stdbool.h>
#include "pico/stdlib.h"

#define GREEN_LED_PIN 11
#define BLUE_LED_PIN 12
#define RED_LED_PIN 13

static void led_init(unsigned int pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

void leds_init() {
    led_init(GREEN_LED_PIN);
    led_init(BLUE_LED_PIN);
    led_init(RED_LED_PIN);
}

void leds_set_red(bool status) {
    gpio_put(RED_LED_PIN, status);
}

void leds_set_green(bool status) {
    gpio_put(GREEN_LED_PIN, status);
}

void leds_set_blue(bool status) {
    gpio_put(BLUE_LED_PIN, status);
}

void leds_off() {
    leds_set_green(false);
    leds_set_blue(false);
    leds_set_red(false);
}