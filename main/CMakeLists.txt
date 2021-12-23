idf_component_register(SRCS "esp32_uart2mqtt.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash esp_https_ota mqtt 
                    esp32MqttClient esp32Time
                    esp32Uart wifi_smart_config 
                    )
