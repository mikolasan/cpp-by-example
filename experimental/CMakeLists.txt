set(CMAKE_CXX_STANDARD 20)

# https://www.mssl.ucl.ac.uk/swift/om/sw/help/man/bfd_toc.html

add_executable(bfd-example bfd-example.cpp)
target_link_libraries(bfd-example bfd)