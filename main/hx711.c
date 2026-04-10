#include "hx711.h"
#include <rom/ets_sys.h>

#define HX711_FIRST_SCK_WAIT_US (10U)  // Delay in microseconds for data ready check
#define HX711_SCK_TIME_US       (10U)  // Time in microseconds for clock pulse
#define HX711_NUM_PULSES        (25U)  // Number of pulses to read data

uint8_t hx711_init(Hx711* dev) {
    return 0;  // Return success
}

uint8_t hx711_read_values(Hx711* dev, uint32_t* value) {
    // Wait for data ready (DOUT goes low)
    while (gpio_get_level(dev->dout_pin) == 1) {
        ets_delay_us(
            HX711_FIRST_SCK_WAIT_US);  // Wait for specified microseconds before checking again
    }

    uint8_t num_pulses = HX711_NUM_PULSES + dev->channel;  // Total pulses to read data and set gain
    uint32_t read_data = 0;
    uint8_t bit_value  = 0;

    for (int i = 0; i < HX711_NUM_PULSES; i++) {
        // Set SCK high to read data bit
        gpio_set_level(dev->sck_pin, 1);
        // Wait for specified microseconds
        ets_delay_us(HX711_SCK_TIME_US);
        // Set SCK low to prepare for next bit
        gpio_set_level(dev->sck_pin, 0);
        // Read data bit from DOUT and shift it into result
        if (i < HX711_NUM_PULSES - 1) {                        // Shift bits for the first 24 pulses
            bit_value = gpio_get_level(dev->dout_pin);         // Read DOUT level
            read_data |= bit_value << (HX711_NUM_PULSES - i);  // Add the current bit to the result
        } else {  // For the 25th pulse, we just need to set the gain
            // The last pulse is used to set the gain for the next reading, so we don't shift it into the result
        }
        ets_delay_us(HX711_SCK_TIME_US);  // Wait for specified microseconds before next bit
    }

    *value = read_data;  // Store the read value in the provided pointer
    return 0;            // Return success
}