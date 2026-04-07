#include "sensors.h"
#include "ads1115.h"
#include "bme280.h"
#include "hal/ads1115_hals.h"
#include "ibme280.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static char* TAG = "SENSORS";

#define HUMIDITY_GAIN   -48.31
#define HUMIDITY_OFFSET 149.76

struct {
    struct bme280_dev bmedev;
    Ads1115 ads;
} sensors;

void sensors_init(SensorConfig* config) {
    sensors.bmedev.read     = bme280_i2c_read;
    sensors.bmedev.write    = bme280_i2c_write;
    sensors.bmedev.delay_us = bme280_delay_us;
    sensors.bmedev.intf_ptr = &config->bmeDev;  // Set this to your I2C device handle
    sensors.bmedev.intf     = BME280_I2C_INTF;
    if (bme280_init(&sensors.bmedev) != BME280_OK) {
        while (1) {
            ESP_LOGE(TAG, "Failed to initialize BME280 sensor");
            vTaskDelay(pdMS_TO_TICKS(1000));  // Wait before retrying
        }
    } else {
        ESP_LOGI(TAG, "BME280 sensor initialized successfully");
    }
    struct bme280_settings settings;
    ESP_ERROR_CHECK(bme280_get_sensor_settings(&settings, &sensors.bmedev));
    settings.osr_h = BME280_OVERSAMPLING_16X;
    settings.osr_t = BME280_OVERSAMPLING_16X;
    settings.osr_p = BME280_OVERSAMPLING_16X;
    ESP_ERROR_CHECK(
        bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, &sensors.bmedev));
    ESP_ERROR_CHECK(bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &sensors.bmedev));

    ads1115_esp32Create(&sensors.ads,
                        &config->adsDev);  // Pass the I2C device handle to the HAL create function

    Ads1115Config ads_config = {
        .fsr  = ADS1115_FSR_4_096V,
        .dr   = ADS1115_DR_128SPS,
        .mux  = ADS1115_MUX_AIN0_GND,
        .mode = ADS1115_MODE_CONTINUOUS,
    };
    ads1115_config(&sensors.ads, &ads_config);
}

void sensors_update(SensorData* data) {
    struct bme280_data bmedata;
    ESP_ERROR_CHECK(bme280_get_sensor_data(BME280_ALL, &bmedata, &sensors.bmedev));

    // Convert to metric units
    data->bme.temperature = bmedata.temperature;       // Already in °C
    data->bme.pressure    = bmedata.pressure / 100.0;  // Convert Pa to hPa (hectopascals/millibars)
    data->bme.humidity    = bmedata.humidity;          // Already in %RH

    // ADS
    ads1115_setMux(&sensors.ads, ADS1115_MUX_AIN0_GND);  // Set MUX to read from AIN0
    vTaskDelay(pdMS_TO_TICKS(30));                       // Short delay to ensure stable reading
    data->adc_Ldr = ads1115_readVoltage(&sensors.ads);   // Read voltage from ADS1115
    ads1115_setMux(&sensors.ads, ADS1115_MUX_AIN1_GND);  // Set MUX to read from AIN1
    vTaskDelay(pdMS_TO_TICKS(30));                       // Short delay to ensure stable reading
    // data->adc_Humidity = HUMIDITY_GAIN * ads1115_readVoltage(&sensors.ads) +
    //                      HUMIDITY_OFFSET;  // Read voltage from ADS1115 and apply calibration
    // recalibrate when the sensor is in the actual environment, this is just a placeholder
    data->adc_Humidity = ads1115_readVoltage(&sensors.ads);  // Read voltage from ADS1115
}