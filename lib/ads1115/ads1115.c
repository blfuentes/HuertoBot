#include "ads1115.h"
#include "ads115_regs.h"

#include <errno.h>
#include <stdio.h>

int32_t writeReg(Ads1115* ads, uint8_t reg, uint16_t value) {
    uint8_t data[3];
    data[0] = reg;
    data[1] = (value >> 8) & 0xFF;  // High byte
    data[2] = value & 0xFF;         // Low byte
    return ads->hal.write(ads->hal.handle, data, sizeof(data));
}

int32_t readReg(Ads1115* ads, uint8_t reg, uint16_t* value) {
    uint8_t data[2];
    if (ads->hal.write(ads->hal.handle, &reg, 1) != 0) {
        return -1;  // Write failed
    }
    if (ads->hal.read(ads->hal.handle, data, sizeof(data)) != 0) {
        return -1;  // Read failed
    }
    *value = (data[0] << 8) | data[1];  // Combine high and low bytes
    return 0;                           // Success
}

int32_t ads1115_init(Ads1115* ads) {
    int32_t ret = 0;
    if (ads == NULL) {
        ret = EINVAL;  // Invalid argument
    } else {
        ads->is_Initialized = 1;
    }
    return ret;
}

int32_t ads1115_config(Ads1115* ads, Ads1115Config* config) {
    int32_t ret = 0;
    if (config == NULL) {
        ret = EINVAL;  // Invalid argument
    } else {
        uint16_t config_reg = ((config->mode) << ADS1115_CFG_MODE_OFFSET & ADS1115_CFG_MODE_MASK) |
                              ((config->mux) << ADS1115_CFG_MUX_OFFSET & ADS1115_CFG_MUX_MASK) |
                              ((config->fsr) << ADS1115_CFG_PGA_OFFSET & ADS1115_CFG_PGA_MASK) |
                              ((config->dr) << ADS1115_CFG_DR_OFFSET & ADS1115_CFG_DR_MASK);
        ret = writeReg(ads, ADS1115_REG_CFG, config_reg);
    }
    return ret;
}