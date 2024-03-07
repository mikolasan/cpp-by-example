set(CMAKE_CXX_STANDARD 20)

add_executable(light_squares light-squares.cpp)

# target_include_directories(light_squares PRIVATE thirdparty/include)
# target_link_directories(light_squares PRIVATE thirdparty/lib)
target_link_libraries(light_squares raylib)

configure_file(test.pcapng ${CMAKE_CURRENT_BINARY_DIR}/test.pcapng COPYONLY)


add_executable(artnet-from-cache
  examples/artnet-from-cache.cpp
  show/show.cpp
)
target_include_directories(artnet-from-cache PRIVATE
  ./
  sender
  show
  stream
)
target_link_libraries(artnet-from-cache
  pcap
  pthread
  ryml
)


add_executable(show_viz 
  show-viz.cpp
  show/show.cpp
)

target_include_directories(show_viz PRIVATE
  ./
  sender
  show
  stream
  ../thirdparty/rapidyaml/src
)
target_link_directories(show_viz PRIVATE thirdparty/lib)
target_link_libraries(show_viz 
  pcap
  pthread
  ryml
  raylib
)