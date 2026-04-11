#ifndef HX711_C__
#define HX711_C__

#include <driver/gpio.h>

typedef enum {
    HX711_CHA_128 = 0,  // Channel A, gain 128
    HX711_CHB_32  = 1,  // Channel B, gain 32
    HX711_CHA_64  = 2   // Channel A, gain 64
} Hx711Channel;

typedef struct {
    gpio_num_t gpio_data;   // Data output pin
    gpio_num_t gpio_sck;    // Clock pin
    Hx711Channel channel;   // Selected channel and gain
    int32_t tare;           // Tare value (raw baseline at 0g)
    float scale;            // Scale factor (grams per count)
    float offset;           // Y-intercept offset (grams)
} Hx711;

uint8_t hx711_init(Hx711* dev);
uint8_t hx711_set_tare(Hx711* dev, int32_t tare);
uint8_t hx711_set_scale_offset(Hx711* dev, float scale, float offset);
uint8_t hx711_read_value_raw(Hx711* dev, int32_t* value);
uint8_t hx711_read_value_scaled(Hx711* dev, float* grams);

#endif  // HX711_C__