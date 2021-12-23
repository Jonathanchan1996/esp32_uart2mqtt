#ifndef __HTTPOTA_H__
#define __HTTPOTA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include <esp_http_server.h>
#include <esp_http_client.h>
#include "esp_https_ota.h"

#define httpOta "httpOta"

void httpOta_request(int OTA_curr_version, char *OTA_check_url, char *OTA_update_url);
#ifdef __cplusplus
}
#endif


#endif /*#ifndef __HTTPREQUEST_H__*/
