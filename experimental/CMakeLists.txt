set(CMAKE_CXX_STANDARD 20)

# https://www.mssl.ucl.ac.uk/swift/om/sw/help/man/bfd_toc.html

if(UNIX AND NOT APPLE)
  add_executable(bfd-example bfd-example.cpp)
  target_link_libraries(bfd-example bfd)
endif()
