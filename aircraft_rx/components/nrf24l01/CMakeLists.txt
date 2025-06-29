add_library(nrf24l01
    src/rp2040_nrf24l01.cpp
)

target_include_directories(nrf24l01 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(nrf24l01 PUBLIC   
    pico_stdlib
    hardware_spi
)
