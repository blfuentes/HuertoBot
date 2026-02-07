#ifndef SENSORS_C__
#define SENSORS_C__

#include <driver/i2c_master.h>

typedef struct {
    i2c_master_dev_handle_t bmeDev;
    i2c_master_dev_handle_t adsDev;
} SensorConfig;

typedef struct {
    float pressure;
    float temperature;
    float humidity;
} SensorData;

void sensors_init(SensorConfig* config);

void sensors_update(SensorData* data);

#endif  // SENSORS_C__