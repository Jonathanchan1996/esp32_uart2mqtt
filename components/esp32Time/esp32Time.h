#ifdef __cplusplus
extern "C" {
#endif

#ifndef ESP32TIME_H
#define ESP32TIME_H
#include "esp_sntp.h"

void esp32TimeInit();
void getTimestamp(int* currTime);
#endif // ifndef ESP32TIME_H
#ifdef __cplusplus
}
#endif