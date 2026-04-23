#include "comms.h"
#include "pump.h"
#include "scanneri2c.h"
#include "sensors.h"
#include "system.h"

#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define PUMP_ACTIVATE (false)  // Set to true to enable pump actuation, false to disable
#define CALIBRATION   (0U)

SensorData sensor_data     = {0};
int64_t subscription_timer = 0;
TaskHandle_t hTaskSensors;
TaskHandle_t hTaskComms;
QueueHandle_t hqSensorsToComms;

void task_sensors_fn(void* arg) {
    ESP_LOGI("Sensors Task", "Starting sensors task...");
    while (1) {
        ESP_LOGI("Sensors Task", "Updating sensor data...");
        sensors_update(&sensor_data);
        xQueueSend(hqSensorsToComms, &sensor_data, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(1000));  // Placeholder for sensor update frequency
    }
}
void task_comms_fn(void* arg) {
    ESP_LOGI("Comms Task", "Starting comms task...");
    while (1) {
        if (xQueueReceive(hqSensorsToComms, &sensor_data, portMAX_DELAY) == pdPASS) {
            ESP_LOGI(
                "Comms Task", "Temperature: %.2f °C | Humidity: %.2f %%RH | Pressure: %.2f hPa",
                sensor_data.bme.temperature, sensor_data.bme.humidity, sensor_data.bme.pressure);
            ESP_LOGI(
                "Comms Task",
                "ADS1115 LDR: %.4f V | Humidity: %.4f %% | Weight: %ld | Calibrated Weight: %.2f",
                sensor_data.adc_Ldr, sensor_data.adc_Humidity, (long)sensor_data.raw_weight,
                sensor_data.grams);

            if (esp_timer_get_time() - subscription_timer > 10000000) {
                ESP_LOGI("Comms Task", "Sending data to MQTT broker...");
                comms_send(&sensor_data);
                subscription_timer = esp_timer_get_time();
            }

            if (sensor_data.adc_Humidity > 1.5) {  // threshold for pump actuation
                ESP_LOGI("Comms Task", "Humidity below threshold, actuating pump...");
                pump_actuate();
                ESP_LOGI("Comms Task", "Pump actuation complete.");
            } else {
                ESP_LOGI("Comms Task", "Humidity above threshold, skipping pump actuation.");
            }
        }
    }
}

void app_main() {
    // Uncomment this to run the I2C scanner instead of the main application logic
    // while (1) {
    //     scanner_scan();
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }

    hqSensorsToComms = xQueueCreate(10, sizeof(SensorData));

    SystemDevs* sysdevs = system_init();

    vTaskDelay(pdMS_TO_TICKS(500));  // Wait for system to stabilize

    // This will be set in system_init when the I2C device is created
    SensorConfig sensor_config = {
        .bmeDev         = sysdevs->bme,
        .adsDev         = sysdevs->ads,
        .hx711_sck_pin  = sysdevs->hx711_sck_pin,
        .hx711_dout_pin = sysdevs->hx711_dout_pin,
    };

    sensors_init(&sensor_config);
    pump_init(PUMP_ACTIVATE, sysdevs->pump_pin);
    comms_init();

    // Always auto-tare at boot (scale must be empty)
    sensors_calibrate();

    xTaskCreate(task_sensors_fn, "Sensors Task", 4096, NULL, 5, &hTaskSensors);
    xTaskCreate(task_comms_fn, "Comms Task", 4096, NULL, 5, &hTaskComms);

    return;
}