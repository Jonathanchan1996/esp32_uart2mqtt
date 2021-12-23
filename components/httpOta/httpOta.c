#include "httpOta.h"

EventGroupHandle_t wifi_event_group;

char httpOtaCheckReading[128] = {0};

#define TAG httpOta
esp_err_t _http_ota_version_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // Write out data
                // printf("%.*s", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

esp_err_t _http_ota_check_event_handle(esp_http_client_event_t *evt){
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_OTA_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_OTA_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_OTA_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_OTA_EVENT_ON_HEADER");
            //printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_OTA_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
                strcpy(httpOtaCheckReading, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_OTA_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_OTA_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;

}

void httpOta_request(int OTA_curr_version, char *OTA_check_url, char *OTA_update_url){

    esp_http_client_config_t config = {
        .url = OTA_check_url,
        .event_handler = _http_ota_check_event_handle,
        .is_async = true,
        .user_data = httpOtaCheckReading,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    (void)err;
    int latestVers = atoi(httpOtaCheckReading);
    if (0){ //OTA_curr_version == latestVers
        ESP_LOGI(TAG, "Latest version, v%d", latestVers);
    }
    else{
        ESP_LOGI(TAG, "Updatable, v%d", latestVers);
        ESP_LOGI(TAG, "Start downloading new firmware");
        esp_http_client_config_t config = {
            .url = OTA_update_url,
            //.cert_pem = (char *)server_cert_pem_start,
            .event_handler = _http_ota_version_event_handler,
            .keep_alive_enable = true,
        };
        esp_err_t ret = esp_https_ota(&config);
        if (ret == ESP_OK) {
            ESP_LOGE(TAG, "Firmware OK -> System restarts");
            esp_restart();
        } else {
            ESP_LOGE(TAG, "Firmware upgrade failed");
        }
    }
}

#undef TAG

/* End of library */
