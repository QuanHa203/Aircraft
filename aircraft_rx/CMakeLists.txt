cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(aircraft_rx C CXX ASM)
pico_sdk_init()

add_subdirectory(components/drv8833)
add_subdirectory(components/nrf24l01)
add_subdirectory(components/servo)

add_executable(aircraft_rx
    main/main.cpp
)

target_link_libraries(aircraft_rx
    pico_stdlib
    hardware_spi
    nrf24l01
)

pico_add_extra_outputs(aircraft_rx)
