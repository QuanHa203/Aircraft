add_executable(${ProjectName} main.cpp)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_enable_stdio_usb(${ProjectName} 1)
pico_enable_stdio_uart(${ProjectName} 0)

target_link_libraries(${ProjectName}
    pico_stdlib
    hardware_spi
    pico_multicore

    esc
    freertos_config
    FreeRTOS-Kernel-Heap4
    nrf24l01
    mx1508
    servo
)


pico_add_extra_outputs(${ProjectName})