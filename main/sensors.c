#include "sensors.h"
#include "ads1115.h"
#include "bme280.h"
#include "hal/ads1115_hals.h"
#include "hx711.h"
#include "ibme280.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static char* TAG       = "SENSORS";
static char* TAG_HX711 = "HX711";

#define HUMIDITY_GAIN   -48.31
#define HUMIDITY_OFFSET 149.76

#define CALIBRATION_HX711_SAMPLES (100U)
#define CALIBRATION_HX711_WARMUP  (1500U)  // HX711 warm-up time for thermal settling
#define CALIBRATION_HX711_DISCARD (10U)    // Readings to discard after warm-up
#define NUM_SAMPLES_TO_AVERAGE    (10U)
// #define HX711_GAIN                (2.43278e-4f)  // grams per count
// #define HX711_OFFSET              (31.571f)      // y-intercept from two-point calibration
#define HX711_GAIN   (-9.38272e-4f)  // grams per count (negative: output decreases with weight)
#define HX711_OFFSET (4.279f)       // y-intercept from two-point calibration

struct {
    struct bme280_dev bmedev;
    Ads1115 ads;
    Hx711 hx711;
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

    sensors.hx711.channel   = HX711_CHA_128;
    sensors.hx711.gpio_sck  = config->hx711_sck_pin;
    sensors.hx711.gpio_data = config->hx711_dout_pin;

    hx711_init(&sensors.hx711);
    hx711_set_scale_offset(&sensors.hx711, HX711_GAIN, HX711_OFFSET);
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

    // HX711
    float grams    = 0.0f;
    float sumGrams = 0.0f;
    for (size_t i = 0; i < NUM_SAMPLES_TO_AVERAGE; i++) {
        hx711_read_value_scaled(&sensors.hx711, &grams);
        sumGrams += grams;
    }
    data->grams = sumGrams / NUM_SAMPLES_TO_AVERAGE;
    hx711_read_value_raw(&sensors.hx711, &data->raw_weight);
}

void sensors_calibrate() {
    // Wait for HX711 thermal settling
    ESP_LOGI(TAG_HX711, "Warming up HX711 (%lu ms)...", (unsigned long)CALIBRATION_HX711_WARMUP);
    vTaskDelay(pdMS_TO_TICKS(CALIBRATION_HX711_WARMUP));

    // Discard initial unstable readings
    int32_t discard;
    for (size_t i = 0; i < CALIBRATION_HX711_DISCARD; i++) {
        hx711_read_value_raw(&sensors.hx711, &discard);
    }

    int32_t value     = 0;
    int64_t sumValues = 0;
    for (size_t i = 0; i < CALIBRATION_HX711_SAMPLES; i++) {
        hx711_read_value_raw(&sensors.hx711, &value);
        sumValues += value;
    }
    int32_t average = (int32_t)(sumValues / CALIBRATION_HX711_SAMPLES);
    // Adjust tare to compensate for offset: at 0g, (raw - tare) * gain + offset = 0
    int32_t adjusted_tare = average + (int32_t)(HX711_OFFSET / HX711_GAIN);
    ESP_LOGI(TAG_HX711, "Calibration complete. Average tare value: %d (adjusted: %d)", (int)average,
             (int)adjusted_tare);
    hx711_set_tare(&sensors.hx711, adjusted_tare);
}