set(CMAKE_CXX_STANDARD 17)
add_executable(send-udp send-udp.cpp)

include_directories(thirdparty)
add_executable(websocket
    thirdparty/easywsclient.cpp
    websocket.cpp)