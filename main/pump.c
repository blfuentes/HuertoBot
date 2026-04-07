#include "pump.h"
#include <freertos/FreeRTOS.h>

static gpio_num_t pump_pin;

void pump_init(gpio_num_t pin) {
    pump_pin = pin;
    // Initialize pump here
}

void pump_actuate(void) {
    // Actuate pump here
    gpio_set_level(pump_pin, 1);      // Turn pump on
    vTaskDelay(pdMS_TO_TICKS(2000));  // Run pump for 2000ms
    gpio_set_level(pump_pin, 0);      // Turn pump off
    vTaskDelay(pdMS_TO_TICKS(100));   // Short delay to ensure pump is fully off
}