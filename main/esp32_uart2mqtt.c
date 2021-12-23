#include "main.h"
#include "LED.c"

char macAddr[8*6] = {0};
struct esp32Uart myUart = {.uartPort=UART_NUM_2, .TX_PIN=17, .RX_PIN=5, .bandwidth=115200};
wifi_config_t sta_config = {.sta = {}};
struct mqttClient iaMqttSer = {.brokerUrl=mqttBroker,};
struct mqttClient iaMqttSer2 = {.brokerUrl=mqttBroker2,};
uint16_t wifiErrorCnt = 1;
uint8_t uartRxData[64]={0};
int uartRxSize=0;
// char *readLine(struct esp32Uart *myUart) {
//     static char line[128];
// 	int uartRxSize;
// 	char *ptr = line;
// 	while(1) {
// 		uartRxSize = uart_read_bytes(myUart->uartPort, (unsigned char *)ptr, 1, portMAX_DELAY);
// 		if (uartRxSize == 1) {
//             //ESP_LOGI("ua","..");
// 			if (*ptr == '\n') {
// 				*ptr = 0;
//                 //ESP_LOGI("ua","endd");
// 				return line;
// 			}
// 			ptr++;
// 		}
// 	}
// }
void readLine(struct esp32Uart *myUart) {
    for(int i=0;i<sizeof(uartRxData);i++) uartRxData[i] = 0;
	uartRxSize=0;
	char ptr ;
	while(uartRxSize<sizeof(uartRxData)) {
		uart_read_bytes(myUart->uartPort, &ptr, 1, portMAX_DELAY);
        uartRxData[uartRxSize] = ptr;
        if (ptr == '\n') {
            //uartRxData[uartRxSize] = ' ';
            ptr = 0;
            return ;
        }
        uartRxSize++;
	}
}
void ledTask(void *arg){ //Uart print data via usb
    gpio_config_t GPIO_output = { .intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = GPIO_OUTPUT_PIN_SEL, .pull_down_en = 0, .pull_up_en = 0};
    gpio_config(&GPIO_output);
    
    //bool pinLv = 0;
    gpio_set_level(fan, 1);
    pwm_init();
    ledc_fade_func_install(0);

    while(1){
        if(xEventGroupGetBits(wifi_event_group)){wifiErrorCnt=0;}
        else {wifiErrorCnt++;}
        if (wifiErrorCnt>0){
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LED_max_pwm);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(LEDTog_wifiDisconnect / portTICK_PERIOD_MS);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(LEDTog_wifiDisconnect / portTICK_PERIOD_MS);
        }
        else{ //normal 
            if(1){
                #define delayy 6000
                ledc_set_fade_with_time(LEDC_MODE,LEDC_CHANNEL, LED_max_pwm, delayy);
                ledc_fade_start(LEDC_MODE,LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
                vTaskDelay(delayy / portTICK_PERIOD_MS);
                ledc_set_fade_with_time(LEDC_MODE,LEDC_CHANNEL, 0, delayy);
                ledc_fade_start(LEDC_MODE,LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
                vTaskDelay(delayy*2 / portTICK_PERIOD_MS);
                #undef delayy 
            }
        }
    }
}
static void mqtt_task(void *pvParameters){
    esp32UartInit(&myUart);
    uint8_t macAddrAry[6]={0};
    esp_efuse_mac_get_default(macAddrAry);
    sprintf(macAddr,"%02x%02x%02x%02x%02x%02x", macAddrAry[0], macAddrAry[1], macAddrAry[2], macAddrAry[3], macAddrAry[4], macAddrAry[5]);
    ESP_LOGI("MacAddress","%s", macAddr);
    
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,false, false, portMAX_DELAY);
    //mqtt_init(&iaMqttSer);
    mqtt_init(&iaMqttSer2);
    char strTopic[128]; char strData[128];
    esp32TimeInit();
    int currTime = 0;
    getTimestamp(&currTime);
    ESP_LOGI("time","got time");
    while (1){
        readLine(&myUart);
        ESP_LOGI("uart Rx","%s", uartRxData);
        xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,false, false, portMAX_DELAY);
        getTimestamp(&currTime);
        sprintf(strTopic,mqtt_topic"/s%s/sensor", macAddr); 
        sprintf(strData,"{\"ts\":%d, \"val\":\"%s\"}",
        currTime, uartRxData);
        //mqtt_publish(&iaMqttSer, strTopic, strData);
        mqtt_publish(&iaMqttSer2, strTopic, strData);
        ESP_LOGI("mqtt tx","%s", strData);
        //ESP_LOGI("MATT_Task", "Looping");
        vTaskDelay(900 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

static void button_task(void *pvParameters){
    
    ESP_LOGI("button_task", "start.");
    uint16_t pushTime = 0;
    bool pastState = false;
    while(1){
        if(isbuttonPush){

            pushTime++;
            pastState = true;
            ESP_LOGI("button_task", "time=%.03fs", pushTime/10.0f);
        }
        else{
            //handler
            if(pastState==true){
                if(buttonPress(pushTime, pushBtnTime_rstWifi)){
                    ESP_LOGI("button_task", "Rst Wifi");
                    nvs_handle my_handle;
                    nvs_open(nvs_wifi_config, NVS_READWRITE, &my_handle);
                    nvs_erase_key(my_handle,nvs_wifi_config_key);
                    esp_err_t err = nvs_flash_erase_partition(nvs_wifi_config);
                    nvs_commit(my_handle);
                    nvs_close(my_handle);
                    esp_restart();// reset and waiting for new wifi config
                    if(err!= ESP_OK){
                        ESP_LOGI("NVS Wifi", "errr");
                    }
                }
            }
            pushTime=0;
            pastState = false;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void app_main(void){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    initialise_wifi(&sta_config);

    xTaskCreate(ledTask, "led_Task", 1024*2, NULL, 13, NULL);
    xTaskCreate(button_task, "button_task", 1024*2, NULL, 5, NULL);
    xTaskCreate(mqtt_task, "mqtt_task", 1024*8, NULL, 4, NULL);
}

/* End of library */
