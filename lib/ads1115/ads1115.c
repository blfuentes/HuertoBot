#include "ads1115.h"
#include "ads115_regs.h"

#include <errno.h>
#include <esp_log.h>
#include <stdio.h>

#define ADS_BITS 15

static const float fsrToVScale[] = {
    [ADS1115_FSR_6_144V] = 6.144 / (1 << ADS_BITS),  // FSR = ±6.144V
    [ADS1115_FSR_4_096V] = 4.096 / (1 << ADS_BITS),  // FSR = ±4.096V
    [ADS1115_FSR_2_048V] = 2.048 / (1 << ADS_BITS),  // FSR = ±2.048V (default)
    [ADS1115_FSR_1_024V] = 1.024 / (1 << ADS_BITS),  // FSR = ±1.024V
    [ADS1115_FSR_0_512V] = 0.512 / (1 << ADS_BITS),  // FSR = ±0.512V
    [ADS1115_FSR_0_256V] = 0.256 / (1 << ADS_BITS)   // FSR = ±0.256V
};

static int32_t writeReg(Ads1115* ads, uint8_t reg, uint16_t value) {
    uint8_t data[3];
    data[0] = reg;
    data[1] = (value >> 8) & 0xFF;  // High byte
    data[2] = value & 0xFF;         // Low byte
    return ads->hal.write(ads->hal.handle, data, sizeof(data));
}

static int32_t readReg(Ads1115* ads, uint8_t reg, uint8_t* value) {
    uint8_t data[2];
    if (ads->hal.write(ads->hal.handle, &reg, 1) != 0) {
        return -1;  // Write failed
    }
    if (ads->hal.read(ads->hal.handle, data, sizeof(data)) != 0) {
        return -1;  // Read failed
    }
    value[0] = data[0];
    value[1] = data[1];
    return 0;  // Success
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
        if (ret == 0) {
            ads->currentFsr = config->fsr;
        }
    }
    return ret;
}

int32_t ads1115_setMux(Ads1115* ads, Ads1115Mux mux) {
    int32_t ret = 0;
    uint8_t config_data[2];
    if (ads == NULL) {
        ret = EINVAL;  // Invalid argument
    } else {
        // Read current config
        if (readReg(ads, ADS1115_REG_CFG, config_data) != 0) {
            return -1;  // Read failed
        }
        uint16_t config_reg = (config_data[0] << 8) | config_data[1];
        // Clear existing MUX bits and set new MUX
        config_reg &= ~ADS1115_CFG_MUX_MASK;                                   // Clear MUX bits
        config_reg |= (mux << ADS1115_CFG_MUX_OFFSET) & ADS1115_CFG_MUX_MASK;  // Set new MUX
        ret = writeReg(ads, ADS1115_REG_CFG, config_reg);
    }
    return ret;
}

int16_t ads1115_readRaw(Ads1115* ads) {
    int16_t ret;
    readReg(ads, ADS1115_REG_CONV, (uint8_t*)&ret);
    // ESP_LOGI("ADS", "ADC Hex: %08x", ret);
    return ((ret & 0xFF) << 8) | ((ret >> 8) & 0xFF);
}

float ads1115_readVoltage(Ads1115* ads) {
    return ads1115_readRaw(ads) * fsrToVScale[ads->currentFsr];
}