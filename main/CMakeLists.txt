idf_component_register(SRCS "hello_world_main.c"
                       PRIV_REQUIRES spi_flash
                       REQUIRES esp_driver_gpio esp_driver_ledc
                       INCLUDE_DIRS "")
