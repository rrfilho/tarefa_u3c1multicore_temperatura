#pragma once
#include <stdbool.h>

void leds_init();
void leds_set_red(bool status);
void leds_set_green(bool status);
void leds_set_blue(bool status);
void leds_off();