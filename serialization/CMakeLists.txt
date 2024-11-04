add_subdirectory(json)
add_subdirectory(yaml)


set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(save_number save-number.cpp)
add_executable(write_number write-number.cpp)

add_library(project_options INTERFACE)
target_compile_options(project_options INTERFACE -O3 -ffast-math -fomit-frame-pointer -fPIC)
target_link_libraries(save_number PRIVATE project_options)

