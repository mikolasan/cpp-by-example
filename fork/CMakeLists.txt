file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/long_process.sh
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(fork fork.cpp)
