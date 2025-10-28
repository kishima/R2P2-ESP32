idf_component_register(
    SRCS "src/fs_proxy.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES
        esp_driver_uart
        fatfs
        vfs
        esp_timer
)
