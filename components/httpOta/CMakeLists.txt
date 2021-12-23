idf_component_register(SRCS "httpOta.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_http_server esp_http_client esp_https_ota)
