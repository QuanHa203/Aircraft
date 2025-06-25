add_library(shared_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/interface.c
)

target_include_directories(shared_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
