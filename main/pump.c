#include "pump.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char* TAG = "Pump";

static bool activated = false;
static gpio_num_t pump_pin;

void pump_init(bool activate, gpio_num_t pin) {
    activated = activate;
    pump_pin  = pin;
}

void pump_actuate(void) {
    if (!activated) {
        ESP_LOGW("Pump", "Pump actuation skipped: Pump is not activated.");
        return;
    }
    gpio_set_level(pump_pin, 1);      // Turn pump on
    vTaskDelay(pdMS_TO_TICKS(2000));  // Run pump for 2000ms
    gpio_set_level(pump_pin, 0);      // Turn pump off
    vTaskDelay(pdMS_TO_TICKS(100));   // Short delay to ensure pump is fully off
}