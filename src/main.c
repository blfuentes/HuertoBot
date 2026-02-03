#include "comms.h"
#include "pump.h"
#include "scanneri2c.h"
#include "sensors.h"
#include "system.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void app_main() {
    while (1) {
        scanner_scan();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    system_init();

    sensors_init();

    for (;;) {
        // sensors_update();

        // //..

        comms_send();
        pump_actuate();

        system_sleep();
    }
}