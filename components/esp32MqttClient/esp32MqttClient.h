#ifndef __IQ_MQTT_H__
#define __IQ_MQTT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "mqtt_client.h"

struct mqttClient{
    esp_mqtt_client_handle_t client;
    char *brokerUrl;
    char *username;
    char *password;
    char *client_id;
    uint8_t errorCnt;
};

void mqtt_init(struct mqttClient *myMqtt);
int mqtt_publish(struct mqttClient *myMqtt, char *topic, char *data);
#ifdef __cplusplus
}
#endif


#endif /*#ifndef __IQ_MQTT_H__*/
