add_library(esc
    src/esc.cpp
)

target_include_directories(esc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(esc PUBLIC   
    pico_stdlib
    hardware_pwm
)
