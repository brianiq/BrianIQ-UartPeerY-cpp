idf_component_register (
        SRCS "BrianIQ-UartPeerY.cpp"
        INCLUDE_DIRS "."
        REQUIRES
                esp_driver_gpio
                esp_driver_uart
)
