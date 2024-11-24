
if(UNIX AND NOT APPLE)
  add_executable(2070_rtc 2070-rtc-limit.c)
endif()

add_executable(scheduler scheduler.cpp)