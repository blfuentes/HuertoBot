#ifndef IBME280_H__
#define IBME280_H__

#include "bme280_defs.h"

#define SEND_BUF_SIZE 10
#define SEND_TIMEOUT_MS 100

BME280_INTF_RET_TYPE bme280_i2c_read(
    uint8_t reg_addr, 
    uint8_t *reg_data, 
    uint32_t len, void *intf_ptr);

BME280_INTF_RET_TYPE bme280_i2c_write(
    uint8_t reg_addr, 
    const uint8_t *reg_data, 
    uint32_t len, void *intf_ptr);

void bme280_delay_us(uint32_t period, void *intf_ptr);


#endif  // IBME280_H__