#include "bootsel.h"
#include "hardware/gpio.h"
#include "pico/bootrom.h"

#define BUTTON_PIN 6

static void callback(uint gpio, uint32_t events) {
  reset_usb_boot(0, 0);
}

void bootsel_init() {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &callback);
}