#ifndef ADS1115_H__
#define ADS1115_H__

#include <stdint.h>

/* HAL */

typedef uint8_t (*adsWriteFunc)(void* handle, uint8_t* data, uint32_t len);
typedef uint8_t (*adsReadFunc)(void* handle, uint8_t* data, uint32_t len);

typedef struct {
    void* handle;
    adsWriteFunc write;
    adsReadFunc read;
} Ads1115Hal;

/* ENUMS */

typedef enum {
    ADS1115_FSR_6_144V = 0,
    ADS1115_FSR_4_096V,
    ADS1115_FSR_2_048V,
    ADS1115_FSR_1_024V,
    ADS1115_FSR_0_512V,
    ADS1115_FSR_0_256V
} Ads1115Fsr;

typedef enum {
    ADS1115_DR_8SPS = 0,
    ADS1115_DR_16SPS,
    ADS1115_DR_32SPS,
    ADS1115_DR_64SPS,
    ADS1115_DR_128SPS,
    ADS1115_DR_250SPS,
    ADS1115_DR_475SPS,
    ADS1115_DR_860SPS
} Ads1115Dr;

typedef enum {
    ADS1115_MUX_AIN0_AIN1 = 0,
    ADS1115_MUX_AIN0_AIN3,
    ADS1115_MUX_AIN1_AIN3,
    ADS1115_MUX_AIN2_AIN3,
    ADS1115_MUX_AIN0_GND,
    ADS1115_MUX_AIN1_GND,
    ADS1115_MUX_AIN2_GND,
    ADS1115_MUX_AIN3_GND
} Ads1115Mux;

typedef enum { ADS1115_MODE_CONTINUOUS = 0, ADS1115_MODE_SINGLE } Ads1115Mode;

typedef struct {
    uint8_t is_Initialized;
    Ads1115Hal hal;
} Ads1115;

typedef struct {
    Ads1115Fsr fsr;
    Ads1115Dr dr;
    Ads1115Mux mux;
    Ads1115Mode mode;
} Ads1115Config;

int32_t ads1115_init(Ads1115* ads);
int32_t ads1115_config(Ads1115* ads, Ads1115Config* config);

#endif  //ADS1115_H__