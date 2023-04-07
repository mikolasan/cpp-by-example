set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif(OPENSSL_FOUND)

add_executable(sha_test sha-test.cpp)
target_link_libraries(sha_test ${OPENSSL_LIBRARIES})