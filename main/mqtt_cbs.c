#include "mqtt_cbs.h"

#include <esp_log.h>

void watering_callback(uint8_t *payload, uint32_t length) {
    ESP_LOGI("MQTT", "Received watering command: %.*s", length, payload);
    // Handle watering command here
}

MqttSubscription mqtt_subscriptions[] = {
    { .topic = (uint8_t *)"huertobot/watering", .callback = watering_callback },
    // Add more subscriptions here
};