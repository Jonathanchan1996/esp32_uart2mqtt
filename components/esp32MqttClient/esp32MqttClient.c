#include "esp32MqttClient.h"

#define TAG  "MQTT_task"
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    //esp_mqtt_client_handle_t client = event->client;
    int msg_id=3;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        
        // case MQTT_EVENT_SUBSCRIBED:
        //     ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        //     break;
        // case MQTT_EVENT_UNSUBSCRIBED:
        //     ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        //     break;
        // case MQTT_EVENT_PUBLISHED:
        //     ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        //     break;
        // case MQTT_EVENT_DATA:
        //     ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        //     printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        //     printf("DATA=%.*s\r\n", event->data_len, event->data);
        //     break;
        // case MQTT_EVENT_ERROR:
        //     ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        //     break;
        default:
            // ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
        
    }
    return ESP_OK;
}

void mqtt_init(struct mqttClient *myMqtt){
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = myMqtt->brokerUrl,
        //.event_handle = mqtt_event_handler,
        .client_id = myMqtt->client_id,
        .disable_auto_reconnect = false,
        //.keepalive = 120,
        //.transport = MQTT_TRANSPORT_OVER_TCP,
        // .user_context = (void *)your_context
    };
    myMqtt->client = esp_mqtt_client_init(&mqtt_cfg);
    //esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(myMqtt->client);
    myMqtt->errorCnt = 0;
}
int mqtt_publish(struct mqttClient *myMqtt, char *topic, char *data){
    //msg_id = esp_mqtt_client_publish(myMqtt->client, "/hkust_ia/mqtt", "data_loop", 0, 1, 0);
    return esp_mqtt_client_publish(myMqtt->client, topic, data, 0, 1, 0);
}

#undef TAG


/* End of library */
