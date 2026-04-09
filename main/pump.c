#include "pump.h"
#include <freertos/FreeRTOS.h>

static bool activated = false;
static gpio_num_t pump_pin;

void pump_init(bool activate, gpio_num_t pin) {
    activated = activate;
    pump_pin = pin;    
}

void pump_actuate(void) {
    if (!activated) {
        printf("Pump actuation skipped: Pump is not activated.\n");
        return;
    }
    gpio_set_level(pump_pin, 1);      // Turn pump on
    vTaskDelay(pdMS_TO_TICKS(2000));  // Run pump for 2000ms
    gpio_set_level(pump_pin, 0);      // Turn pump off
    vTaskDelay(pdMS_TO_TICKS(100));   // Short delay to ensure pump is fully off
}