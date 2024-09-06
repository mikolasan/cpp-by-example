set(CMAKE_CXX_STANDARD 20)

add_executable(randomness_test randomness-test.c)
configure_file(decoded-20240131051421.bin ${CMAKE_CURRENT_BINARY_DIR}/decoded-20240131051421.bin COPYONLY)
target_link_libraries(randomness_test m)

add_executable(tony_rand lrand.c)