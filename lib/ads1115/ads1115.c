#include "ads1115.h"
#include "ads115_regs.h"

#include <errno.h>
#include <stdio.h>

int32_t ads1115_init(Ads1115* ads) {
    int32_t ret = 0;
    if (ads == NULL) {
        ret = EINVAL;  // Invalid argument
    } else {
        ads->is_Initialized = 1;
    }
    return ret;
}

int32_t ads1115_config(Ads1115Config* config) {
    int32_t ret = 0;
    if (config == NULL) {
        ret = EINVAL;  // Invalid argument
    } else {
        uint16_t config_reg = ((config->mode) << ADS1115_CFG_MODE_OFFSET & ADS1115_CFG_MODE_MASK) |
                              ((config->mux) << ADS1115_CFG_MUX_OFFSET & ADS1115_CFG_MUX_MASK) |
                              ((config->fsr) << ADS1115_CFG_PGA_OFFSET & ADS1115_CFG_PGA_MASK) |
                              ((config->dr) << ADS1115_CFG_DR_OFFSET & ADS1115_CFG_DR_MASK);
        write_register(ADS1115_REG_CFG, config_reg);
    }
    return ret;
}