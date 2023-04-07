# Search for rocm in common locations
list(APPEND CMAKE_PREFIX_PATH /opt/rocm/hip /opt/rocm)
# Find hip
find_package(hip)

add_executable(rocm_test test.cpp)

# Link with HIP
target_link_libraries(rocm_test hip::host hip::device)