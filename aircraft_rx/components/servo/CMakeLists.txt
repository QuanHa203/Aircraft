add_library(servo
    src/servo.cpp
)

target_include_directories(servo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(servo PUBLIC   
    pico_stdlib
    hardware_pwm
)
