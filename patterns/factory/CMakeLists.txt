set(CMAKE_CXX_STANDARD 17)

set(PROJECT_SOURCES
  TimeFactory.cpp
  SystemTime.cpp
  MotherboardRTC.cpp
  RandomTime.cpp
  main.cpp
)

add_executable(time_factory ${PROJECT_SOURCES})