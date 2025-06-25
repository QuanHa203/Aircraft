cmake_minimum_required(VERSION 3.13)
project(aircraft_rx C CXX ASM)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
pico_sdk_init()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../shared_lib shared_lib_build)

add_executable(aircraft_rx
    main/main.cpp
)

target_link_libraries(aircraft_rx
    pico_stdlib
    shared_lib
)

pico_add_extra_outputs(aircraft_rx)
