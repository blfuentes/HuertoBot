#include "scanneri2c.h"

#include <driver/i2c_master.h>
#include <esp_log.h>

const char* TAG = "SCANNER";

void scanner_scan(void) {
    ESP_LOGI(TAG, "-- Starting I2C scan --");
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = GPIO_NUM_8,
        .scl_io_num = GPIO_NUM_9,
        .intr_priority = 0,
        .flags =
            {
                .enable_internal_pullup = 1,
                .allow_pd = 0,
            },
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));
    ESP_LOGI(TAG, "I2C master bus initialized on SDA GPIO %d, SCL GPIO %d", bus_config.sda_io_num,
             bus_config.scl_io_num);
    ESP_LOGI(TAG, "Scanning for I2C devices...");

    // Basic scan implementation
    // for (uint8_t address = 1; address < 127; address++) {
    //     esp_err_t ret = i2c_master_probe(bus_handle, address, 100);
    //     if (ret == ESP_OK) {
    //         ESP_LOGI(TAG, "## Found device at 0x%02X", address);
    //     }
    //     // No need to log errors for addresses that are not found
    //     // else if (ret != ESP_ERR_NOT_FOUND) {
    //     //     ESP_LOGE(TAG, "!! Error probing address 0x%02X: %s", address, esp_err_to_name(ret));
    //     // }
    // }

    // Graphical scan implementation
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    printf("00:         ");
    for (uint8_t address = 0x03; address < 0x78; address++) {
        if (address % 16 == 0) {
            printf("\n%.2x:", address);
        }

        esp_err_t ret = i2c_master_probe(bus_handle, address, 100);

        if (ret == ESP_OK) {
            printf(" %.2x", address);
        } else {
            printf(" --");
        }
    }
    i2c_del_master_bus(bus_handle);
    ESP_LOGI(TAG, "-- I2C scan complete --");
}