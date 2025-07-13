add_library(mx1508
    src/mx1508.cpp
)

target_include_directories(mx1508 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mx1508 PUBLIC   
    pico_stdlib
    hardware_pwm
)
