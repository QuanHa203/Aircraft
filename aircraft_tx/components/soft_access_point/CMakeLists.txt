idf_component_register( SRCS  "src/soft_access_point.cpp"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_wifi esp_event nvs_flash
)
