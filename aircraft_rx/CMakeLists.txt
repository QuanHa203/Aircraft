cmake_minimum_required(VERSION 3.13)

# Name project
SET(ProjectName aircraft_rx)
set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/components/FreeRTOS-Kernel)

include(pico_sdk_import.cmake)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(${ProjectName} C CXX ASM)
pico_sdk_init()

add_subdirectory(main)
add_subdirectory(components/freertos_config)
add_subdirectory(components/mx1508)
add_subdirectory(components/nrf24l01)
add_subdirectory(components/servo)
