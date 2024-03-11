set(CMAKE_CXX_STANDARD 20)

add_executable(light_squares light-squares.cpp)

# target_include_directories(light_squares PRIVATE thirdparty/include)
# target_link_directories(light_squares PRIVATE thirdparty/lib)
target_link_libraries(light_squares raylib)

# configure_file(test.pcapng ${CMAKE_CURRENT_BINARY_DIR}/test.pcapng COPYONLY)
configure_file(config.yml ${CMAKE_CURRENT_BINARY_DIR}/config.yml COPYONLY)


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
  show/show_abstract.cpp
)

target_include_directories(show_viz PRIVATE
  ./
  sender
  show
  stream
  cppzmq
  ../thirdparty/rapidyaml/src
)
target_link_directories(show_viz PRIVATE thirdparty/lib)
target_link_libraries(show_viz 
  pcap
  pthread
  ryml
  raylib
  zmq
)


add_executable(led_proxy_next 
  led-proxy-next.cpp
  sender/network_error.cpp
  sender/udp_sender.cpp
  show/show.cpp
  show/show_abstract.cpp
)

target_include_directories(led_proxy_next PRIVATE
  ./
  sender
  show
  stream
  cppzmq
  ../thirdparty/rapidyaml/src
)
target_link_directories(led_proxy_next PRIVATE thirdparty/lib)
target_link_libraries(led_proxy_next 
  pcap
  pthread
  ryml
  raylib
  zmq
)