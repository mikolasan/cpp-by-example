cmake_minimum_required(VERSION 3.14)

project(cpp_by_example VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/rapidyaml)

set(OpenGL_GL_PREFERENCE GLVND)
add_subdirectory(thirdparty/raylib-git)

add_subdirectory(_oop)
add_subdirectory(_std)
add_subdirectory(_templates)

add_subdirectory(big_files)
add_subdirectory(combinations)
if(CPP_BY_EXAMPLE_LLVM_READY)
    add_subdirectory(compiler)
endif()
add_subdirectory(const)
add_subdirectory(crypto)
add_subdirectory(fork)
add_subdirectory(led)
add_subdirectory(lua)
if(CPP_BY_EXAMPLE_OPENCV_READY)
    add_subdirectory(opencv)
endif()
add_subdirectory(pointing_arguments)
add_subdirectory(raw_pointer_container)
# add_subdirectory(rocm)
add_subdirectory(send_udp) # c++17
# add_subdirectory(serial)
add_subdirectory(serialization)
add_subdirectory(time)
add_subdirectory(yaml)