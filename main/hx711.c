#include "hx711.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rom/ets_sys.h>

#define HX711_SCK_TIME_US      (10U)   // Time in microseconds for clock pulse
#define HX711_DATA_BITS        (24U)   // Number of data bits
#define HX711_MSB_BIT_POS      (23U)   // MSB position (bit 23)
#define HX711_NUM_PULSES       (25U)   // Base number of pulses (24 data + 1 for gain 128)

uint8_t hx711_init(Hx711* dev) {
    gpio_set_level(dev->gpio_sck, 0);
    return 0;
}

uint8_t hx711_set_tare(Hx711* dev, int32_t tare) {
    dev->tare = tare;
    return 0;
}

uint8_t hx711_set_scale_offset(Hx711* dev, float scale, float offset) {
    dev->scale  = scale;
    dev->offset = offset;
    return 0;
}

uint8_t hx711_read_value_raw(Hx711* dev, int32_t* value) {
    // Wait for data ready (DOUT goes low)
    while (gpio_get_level(dev->gpio_data) == 1) {
        vTaskDelay(1);  // Yield to IDLE task (1 tick = 10ms at 100Hz)
    }

    // Total pulses: 25 for gain 128, +1 for gain 32, +2 for gain 64
    uint8_t num_pulses = HX711_NUM_PULSES + (uint8_t)dev->channel;
    int32_t read_data  = 0;

    for (uint8_t i = 0; i < num_pulses; i++) {
        gpio_set_level(dev->gpio_sck, 1);
        ets_delay_us(HX711_SCK_TIME_US);
        gpio_set_level(dev->gpio_sck, 0);

        if (i < HX711_DATA_BITS) {
            uint8_t bit_value = gpio_get_level(dev->gpio_data);
            read_data |= (bit_value << (HX711_MSB_BIT_POS - i));
        }

        ets_delay_us(HX711_SCK_TIME_US);
    }

    // Sign-extend the 24-bit two's complement value
    if (read_data & 0x800000) {
        read_data |= 0xFF000000;
    }

    *value = read_data;
    return 0;
}

uint8_t hx711_read_value_scaled(Hx711* dev, float* grams) {
    int32_t raw_value = 0;
    if (hx711_read_value_raw(dev, &raw_value) != 0) {
        return 1;
    }

    *grams = (raw_value - dev->tare) * dev->scale + dev->offset;
    return 0;
}