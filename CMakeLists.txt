cmake_minimum_required(VERSION 3.14)

project(twitter_slides VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/rapidyaml)

add_subdirectory(stdstring)
add_subdirectory(combinations)
add_subdirectory(compiler)
add_subdirectory(const)
add_subdirectory(crypto)
add_subdirectory(destructor)
add_subdirectory(enable_if)
add_subdirectory(fork)
add_subdirectory(lua)
add_subdirectory(non_copyable)
add_subdirectory(pointing_arguments)
#add_subdirectory(rabbit)
add_subdirectory(raw_pointer_container)
# add_subdirectory(rocm)
add_subdirectory(send_udp) # c++17
add_subdirectory(serialization)
add_subdirectory(string_view) # c++17
add_subdirectory(variadic)
add_subdirectory(virtual_call)
add_subdirectory(weird_map) # c++17
add_subdirectory(yaml)