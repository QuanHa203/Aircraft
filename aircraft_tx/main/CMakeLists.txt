idf_component_register( SRCS "main.cpp"
                        INCLUDE_DIRS "."
                        PRIV_REQUIRES shared_lib
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../shared_lib ${CMAKE_BINARY_DIR}/shared_lib_build)
target_link_libraries(${COMPONENT_LIB} PUBLIC shared_lib)