#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/long_process.sh
#        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(fork fork.cpp)
configure_file(long_process.sh ${CMAKE_CURRENT_BINARY_DIR}/long_process.sh COPYONLY)