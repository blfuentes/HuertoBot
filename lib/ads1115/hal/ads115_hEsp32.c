#include <driver/i2c_master.h>

uint8_t ads1115_esp32Write(void *handle, uint8_t* data, uint32_t len)
{
    i2c_master_dev_handle_t i2c_dev = *(i2c_master_dev_handle_t*)handle;
    return i2c_master_transmit(i2c_dev, data, len, 100);
}
uint8_t ads1115_esp32Read(void *handle, uint8_t* data, uint32_t len)
{
    i2c_master_dev_handle_t i2c_dev = *(i2c_master_dev_handle_t*)handle;
    return i2c_master_receive(i2c_dev, data, len, 100);
}