#include "sensors.h"
#include "bme280.h"

struct {
    struct bme280_dev bmedev;
} sensors;

void sensors_init(void) {
    sensors.bmedev.read = bme280_i2c_read;
    sensors.bmedev.write = bme280_i2c_write;
    sensors.bmedev.delay_us = bme280_delay_us;
    sensors.bmedev.intf = BME280_I2C_INTF;
}

void sensors_update(void){
}