cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(aircraft_rx C CXX ASM)
pico_sdk_init()

set(FREERTOS_PORT GCC_ARM_CM0)
set(FREERTOS_HEAP 4)

add_subdirectory(components/freertos_config)
add_subdirectory(components/FreeRTOS-Kernel)
add_subdirectory(components/mx1508)
add_subdirectory(components/nrf24l01)
add_subdirectory(components/servo)

add_executable(aircraft_rx
    main/main.cpp
)

target_link_libraries(aircraft_rx
    pico_stdlib
    hardware_spi
    pico_multicore

    freertos_kernel
    nrf24l01
    mx1508
    servo
)


pico_add_extra_outputs(aircraft_rx)
