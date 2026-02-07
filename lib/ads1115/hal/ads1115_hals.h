#ifndef ADS1115_HALS_H__
#define ADS1115_HALS_H__

#ifdef ESP_PLATFORM

#include <driver/i2c_master.h>
#include "ads1115.h"

void ads1115_esp32Create(Ads1115* ads, i2c_master_dev_handle_t* dev);

#endif

#endif  // ADS1115_HALS_H__