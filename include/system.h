#ifndef SYSTEM_C__
#define SYSTEM_C__

#include <driver/i2c_master.h>

#define BME280_I2C_ADDR 0x76

typedef struct {
    i2c_master_dev_handle_t bme;
} SystemDevs;

SystemDevs* system_init();

void system_sleep(void);

#endif  // SYSTEM_C__