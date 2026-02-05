#include "ibme280.h"

#include <string.h>
#include <rom/ets_sys.h>
#include <driver/i2c_master.h>

uint8_t send_buf[SEND_BUF_SIZE] = {0};

BME280_INTF_RET_TYPE bme280_i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t len,
                                     void* intf_ptr) {
    i2c_master_dev_handle_t i2c_dev = *(i2c_master_dev_handle_t*)intf_ptr;
    send_buf[0] = reg_addr;
    return i2c_master_transmit_receive(i2c_dev, send_buf, 1, reg_data, len, SEND_TIMEOUT_MS);
}

BME280_INTF_RET_TYPE bme280_i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len,
                                      void* intf_ptr) {
    i2c_master_dev_handle_t i2c_dev = *(i2c_master_dev_handle_t*)intf_ptr;
    send_buf[0] = reg_addr;
    memcpy(&send_buf[1], reg_data, len);
    return i2c_master_transmit(i2c_dev, send_buf, len + 1, SEND_TIMEOUT_MS);
}

void bme280_delay_us(uint32_t period, void* intf_ptr) {
    ets_delay_us(period);
}