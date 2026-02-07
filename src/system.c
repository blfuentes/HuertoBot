#include "system.h"

#include <driver/i2c_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static SystemDevs global_devs = {
    .bme = NULL,
    .ads = NULL,
};
i2c_master_bus_handle_t bus_handle = NULL;

SystemDevs* system_init() {
    i2c_master_bus_config_t bus_config = {
        .i2c_port      = I2C_NUM_0,
        .clk_source    = I2C_CLK_SRC_DEFAULT,
        .sda_io_num    = GPIO_NUM_8,
        .scl_io_num    = GPIO_NUM_9,
        .intr_priority = 0,
        .flags =
            {
                .enable_internal_pullup = 1,
                .allow_pd               = 0,
            },
        .glitch_ignore_cnt = 7,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    i2c_device_config_t device_config = {.dev_addr_length = I2C_ADDR_BIT_7,
                                         .device_address  = BME280_I2C_ADDR,  // BME280 I2C address
                                         .scl_speed_hz    = 100000,           // Standard I2C speed
                                         .scl_wait_us     = 0,
                                         .flags.disable_ack_check = 0};

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &device_config, &global_devs.bme));

    // ADS1115 uses the same I2C bus, so we can add it as well
    device_config.device_address = ADS1115_I2C_ADDR;  // ADS1115 I2C address
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &device_config, &global_devs.ads));

    return &global_devs;
}

void system_sleep(void) {}