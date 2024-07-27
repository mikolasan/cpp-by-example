set(CMAKE_CXX_STANDARD 20)

add_executable(bfd-example bfd-example.cpp)
target_link_libraries(bfd-example bfd)