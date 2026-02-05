#include "sensors.h"
#include "bme280.h"
#include "ibme280.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

static char* TAG = "SENSORS";

struct {
    struct bme280_dev bmedev;
} sensors;

void sensors_init(SensorConfig* config) {
    sensors.bmedev.read = bme280_i2c_read;
    sensors.bmedev.write = bme280_i2c_write;
    sensors.bmedev.delay_us = bme280_delay_us;
    sensors.bmedev.intf_ptr = &config->bme;  // Set this to your I2C device handle
    sensors.bmedev.intf = BME280_I2C_INTF;
    if(bme280_init(&sensors.bmedev) != BME280_OK) {
        while(1) {
            ESP_LOGE(TAG, "Failed to initialize BME280 sensor");
            vTaskDelay(pdMS_TO_TICKS(1000));  // Wait before retrying
        }
    }
    else {
        ESP_LOGI(TAG, "BME280 sensor initialized successfully");
    }
    struct bme280_settings settings;
    ESP_ERROR_CHECK(bme280_get_sensor_settings(&settings, &sensors.bmedev));
    settings.osr_h = 5;
    settings.osr_t = 5;
    settings.osr_p = 5;
    ESP_ERROR_CHECK(bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, &sensors.bmedev));
    ESP_ERROR_CHECK(bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &sensors.bmedev));
}

void sensors_update(SensorData* data) {
  struct bme280_data bmedata;
  ESP_ERROR_CHECK(bme280_get_sensor_data(BME280_ALL, &bmedata, &sensors.bmedev));

  // Convert to metric units
  data->temperature = bmedata.temperature;  // Already in °C
  data->pressure = bmedata.pressure / 100.0;  // Convert Pa to hPa (hectopascals/millibars)
  data->humidity = bmedata.humidity;  // Already in %RH
}