set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif(OPENSSL_FOUND)

add_executable(sha_test sha-test.cpp)
target_link_libraries(sha_test ${OPENSSL_LIBRARIES})

add_executable(sha256_test sha256-test.cpp)
target_link_libraries(sha256_test ${OPENSSL_LIBRARIES})

add_executable(rsa2048 rsa2048.cpp)
target_link_libraries(rsa2048 ${OPENSSL_LIBRARIES})

add_executable(rsa_keys rsa-keys.cpp)
target_link_libraries(rsa_keys ${OPENSSL_LIBRARIES})
