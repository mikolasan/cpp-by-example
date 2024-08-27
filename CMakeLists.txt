cmake_minimum_required(VERSION 3.14)

project(cpp_by_example VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/rapidyaml)

add_subdirectory(audio)
add_subdirectory(basics)
add_subdirectory(cli)
add_subdirectory(combinatorics)
# OpenSSL required
# add_subdirectory(cryptography)
add_subdirectory(experimental)
# OpenCV required
# add_subdirectory(gpu)

# add_subdirectory(led)

# LLVM required
# add_subdirectory(llvm)

# Lua required
# add_subdirectory(lua)
add_subdirectory(net)
add_subdirectory(oop)
add_subdirectory(optimization)
add_subdirectory(patterns)
add_subdirectory(puzzles)
add_subdirectory(rand)
add_subdirectory(serialization)
add_subdirectory(std)
add_subdirectory(templates)
add_subdirectory(time)
add_subdirectory(yaml)
