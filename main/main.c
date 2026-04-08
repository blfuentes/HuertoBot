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
    pump_init(sysdevs->pump_pin);

    for (;;) {
        sensors_update(&sensor_data);
        printf("Temperature: %.2f °C | Humidity: %.2f %%RH | Pressure: %.2f hPa\n",
               sensor_data.bme.temperature, sensor_data.bme.humidity, sensor_data.bme.pressure);
        // //..
        printf("ADS1115 LDR: %.4f V | Humidity: %.4f %%\n", sensor_data.adc_Ldr,
               sensor_data.adc_Humidity);
        // comms_send();

        if (sensor_data.adc_Humidity > 1.5) {  // treshold for pump actuation
            printf("Humidity below threshold, actuating pump...\n");
            pump_actuate();
            printf("Pump actuation complete.\n");
        } else {
            printf("Humidity above threshold, skipping pump actuation.\n");
        }

        system_sleep();

        vTaskDelay(pdMS_TO_TICKS(200));  // Update every 5 seconds
    }
}