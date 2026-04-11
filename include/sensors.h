#ifndef SENSORS_C__
#define SENSORS_C__

#include <driver/i2c_master.h>

typedef struct {
    i2c_master_dev_handle_t bmeDev;
    i2c_master_dev_handle_t adsDev;
    gpio_num_t hx711_sck_pin;
    gpio_num_t hx711_dout_pin;
} SensorConfig;

typedef struct {
    struct {
        float pressure;
        float temperature;
        float humidity;
    } bme;
    float adc_Ldr;
    float adc_Humidity;
    int32_t raw_weight;
    float grams;
} SensorData;

void sensors_init(SensorConfig* config);

void sensors_update(SensorData* data);

void sensors_calibrate();
#endif  // SENSORS_C__