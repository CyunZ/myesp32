idf_component_register(SRCS "station_example_main.c" "hello_world_main.c" "spi_lcd_touch_example_main.c" "lvgl_demo_ui.c" 
                       PRIV_REQUIRES spi_flash
                       REQUIRES esp_wifi nvs_flash esp_driver_gpio esp_driver_ledc esp_timer
                       INCLUDE_DIRS "")
