add_executable(light_squares light-squares.cpp)

target_include_directories(light_squares PRIVATE thirdparty/include)
target_link_directories(light_squares PRIVATE thirdparty/lib)
target_link_libraries(light_squares raylib)

