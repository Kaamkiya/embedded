idf_component_register(SRCS "ws_test.c" "wifi.c" "ws.c"
                    PRIV_REQUIRES esp_http_server nvs_flash esp_wifi esp_driver_gpio
                    INCLUDE_DIRS ".")
