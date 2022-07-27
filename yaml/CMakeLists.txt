add_executable(yaml yaml.cpp)
target_include_directories(yaml PRIVATE ../thirdparty/rapidyaml/src)
target_link_libraries(yaml ryml)
configure_file(config.yaml ${CMAKE_CURRENT_BINARY_DIR}/config.yaml COPYONLY)