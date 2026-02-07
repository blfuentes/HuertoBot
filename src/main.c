#include "comms.h"
#include "pump.h"
#include "scanneri2c.h"
#include "sensors.h"
#include "system.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void app_main() {
    // Uncomment this to run the I2C scanner instead of the main application logic
    // while (1) {
    //     scanner_scan();
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }

    SystemDevs* sysdevs    = system_init();
    SensorData sensor_data = {0};

    vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for system to stabilize

    // This will be set in system_init when the I2C device is created
    SensorConfig sensor_config = {
        .bmeDev = sysdevs->bme,
        .adsDev = sysdevs->ads,
    };

    sensors_init(&sensor_config);

    for (;;) {
        sensors_update(&sensor_data);
        printf("Temperature: %.2f °C | Humidity: %.2f %%RH | Pressure: %.2f hPa\n",
               sensor_data.bme.temperature, sensor_data.bme.humidity, sensor_data.bme.pressure);
        // //..
        printf("ADS1115 Raw Value: %d\n", sensor_data.ads_value);
        // comms_send();
        // pump_actuate();

        system_sleep();

        vTaskDelay(pdMS_TO_TICKS(5000));  // Update every 5 seconds
    }
}