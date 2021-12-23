#ifndef __WIFI_SMART_CONFIG_H__
#define __WIFI_SMART_CONFIG_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "nvs_flash.h"

#include "esp_netif.h"
#include "esp_smartconfig.h"
#include "esp_event.h"

#define smart_wifi_event_group wifi_event_group
extern EventGroupHandle_t smart_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
//#define ESPTOUCH_DONE_BIT BIT1
#define nvs_wifi_config "WIFI_CONFIG"
#define nvs_wifi_config_key "wifi_config"

void initialise_wifi(wifi_config_t *sta_config);


#ifdef __cplusplus
}
#endif

#endif /*#ifndef __WIFI_SMART_CONFIG_H__*/

