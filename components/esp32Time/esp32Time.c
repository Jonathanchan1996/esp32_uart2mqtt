#include "esp32Time.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
time_t now = 0;
struct tm timeinfo = { 0 };
void esp32TimeInit(){
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "hk.pool.ntp.org");
    sntp_setservername(1, "time1.google.com");
    //sntp_setservername(2, "uk.pool.ntp.org ");
    //sntp_setservername(3, "europe.pool.ntp.org");
    //sntp_setservername(4, "us.pool.ntp.org");
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    sntp_init();
    
     while (timeinfo.tm_year < (2019 - 1900)) {
        ESP_LOGD("timee", "Waiting for system time to be set... ");
        vTaskDelay(100 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI("time", "Current Time of the world: %s", strftime_buf);
}
void getTimestamp(int* currTime){
    //time_t now = 0;
    *currTime = time(&now);
}