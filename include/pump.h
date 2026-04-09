#ifndef PUMP_C__
#define PUMP_C__

#include <driver/gpio.h>

void pump_init(bool activate, gpio_num_t pump_pin);

void pump_actuate(void);

#endif  // PUMP_C__