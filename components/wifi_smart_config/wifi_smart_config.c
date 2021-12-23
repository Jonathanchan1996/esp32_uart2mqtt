#include "wifi_smart_config.h"
//#include "networkDef.h"
EventGroupHandle_t wifi_event_group;
const int ESPTOUCH_DONE_BIT = BIT1;

#define TAG "SmartConfig"
#define wifiTAG "WIFI-Fixed"
// #define debugWifiMode
static void smartconfig_loop_task(void * parm);

static void saveWifiConfig(wifi_config_t *wifi_config){
    //ESP_ERROR_CHECK(nvs_flash_init());
    nvs_handle my_handle;
    nvs_open(nvs_wifi_config, NVS_READWRITE, &my_handle);
 
    ESP_ERROR_CHECK(nvs_set_blob(my_handle, nvs_wifi_config_key, wifi_config, sizeof(wifi_config_t)));
 
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}
 
static esp_err_t readWifiConfig(wifi_config_t *sta_config){
    //ESP_ERROR_CHECK(nvs_flash_init());
    nvs_handle my_handle;
    nvs_open(nvs_wifi_config, NVS_READWRITE, &my_handle);
 
    // sta_config = {
    //     .sta = {
    //         .ssid = "ssid",
    //         .password = "password",
    //         .bssid_set = false}}
 
    uint32_t len = sizeof(wifi_config_t);
    esp_err_t err = nvs_get_blob(my_handle, nvs_wifi_config_key, sta_config, &len);
 
    nvs_close(my_handle);

    return err;
}


static esp_err_t fixed_wifi_event_handler(void *ctx, system_event_t *event){
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
    	ESP_LOGI(wifiTAG, "SYSTEM_EVENT_STA_GOT_IP: %s\n",
		ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
    	ESP_LOGI(wifiTAG, "station:"MACSTR" join, AID=%d\n",
		MAC2STR(event->event_info.sta_connected.mac),
		event->event_info.sta_connected.aid);
    	xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    	break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
    	ESP_LOGI(wifiTAG, "station:"MACSTR" leave, AID=%d\n",
		MAC2STR(event->event_info.sta_disconnected.mac),
		event->event_info.sta_disconnected.aid);
    	xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
    	break;
    default:
        break;
    }
    return ESP_OK;
}

static void smart_wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        wifi_config_t sta_config;
        esp_err_t err = readWifiConfig(&sta_config);
        if (err == ESP_ERR_NVS_NOT_FOUND){ //
            xTaskCreate(smartconfig_loop_task, "smartconfig_loop_task", 4096, NULL, 3, NULL);
        }
        else if (err == ESP_OK){
            ESP_LOGI(TAG, "readWifiConfig ESP_OK");
            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
            ESP_ERROR_CHECK(esp_wifi_connect());
        }  
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(smart_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(smart_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        ESP_LOGI(TAG, "Found channel");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };
        uint8_t rvd_data[33] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
            ESP_LOGI(TAG, "RVD_DATA:");
            for (int i=0; i<33; i++) {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        ESP_ERROR_CHECK( esp_wifi_disconnect() );
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
        esp_wifi_connect();

        saveWifiConfig(&wifi_config);

    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(smart_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

void initialise_wifi(wifi_config_t *sta_config){
    ESP_ERROR_CHECK(esp_netif_init());
    smart_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
#ifdef debugWifiMode
    ESP_ERROR_CHECK(esp_event_loop_init(fixed_wifi_event_handler, NULL) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, sta_config) );
    ESP_LOGI(TAG, "WIFI with fixed SSID");
#else
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &smart_wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &smart_wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &smart_wifi_event_handler, NULL) );
    ESP_LOGI(TAG, "ESP-Touch mode");
#endif
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void smartconfig_loop_task(void * parm){
    EventBits_t uxBits;
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );
    while (1) {
        //ESP_LOGI("smart_wifi","%d", ESPTOUCH_DONE_BIT);
        uxBits = xEventGroupWaitBits(smart_wifi_event_group, WIFI_CONNECTED_BIT | ESPTOUCH_DONE_BIT, false, false, portMAX_DELAY);
        if(uxBits & WIFI_CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi Connected to ap");
        }
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
            
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "task killed");
    vTaskDelete(NULL);
}
#undef TAG