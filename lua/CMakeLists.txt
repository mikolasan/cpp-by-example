find_package(Lua51 REQUIRED)

add_executable(stack stack.cpp)

if (LUA51_FOUND)
  include_directories(${LUA_INCLUDE_DIR})
  target_link_libraries (stack ${LUA_LIBRARIES})
endif (LUA51_FOUND)

