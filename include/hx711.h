#ifndef HX711_C__
#define HX711_C__

#include <driver/gpio.h>

typedef enum {
    HX711_CH_A_GAIN_128 = 0,  // Channel A, gain 128
    HX711_CH_B_GAIN_32  = 1,  // Channel B, gain 32
    HX711_CH_A_GAIN_64  = 2   // Channel A, gain 64
} Hx711Channel;

typedef struct {
    gpio_num_t dout_pin;   // Data output pin
    gpio_num_t sck_pin;    // Clock pin
    Hx711Channel channel;  // Selected channel and gain
} Hx711;

uint8_t hx711_init(Hx711* dev);
uint8_t hx711_read_values(Hx711* dev, uint32_t* value);

#endif  // HX711_C__