cmake_minimum_required(VERSION 3.14)

project(cpp_by_example VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# thirdparty
add_subdirectory(thirdparty/rapidyaml)

## windows
### SDL2
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/thirdparty/windows/SDL2/x86_64-w64-mingw32")
message("[SDL2] CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
find_package(SDL2 CONFIG REQUIRED)

add_subdirectory(audio)
add_subdirectory(basics)
add_subdirectory(cli)
add_subdirectory(combinatorics)
# OpenSSL required
# add_subdirectory(cryptography)

# add_subdirectory(experimental)
# OpenCV required
# add_subdirectory(gpu)
add_subdirectory(graphics)
# add_subdirectory(led)

# LLVM required
add_subdirectory(llvm)

add_subdirectory(net)
add_subdirectory(oop)
add_subdirectory(optimization)
add_subdirectory(patterns)
add_subdirectory(puzzles)
add_subdirectory(rand)
add_subdirectory(scripting)
add_subdirectory(serialization)
add_subdirectory(std)
add_subdirectory(templates)
add_subdirectory(time)
