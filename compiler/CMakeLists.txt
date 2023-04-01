set(CMAKE_CXX_STANDARD 20)

if(WIN32)
  set(LLVM_DIR ${CMAKE_SOURCE_DIR}/thirdparty/windows/LLVM/lib/cmake/llvm)
endif()

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add LLVM include directories
include_directories(${LLVM_INCLUDE_DIRS})

# Add the LLVM definitions
add_definitions(${LLVM_DEFINITIONS})

add_executable(code-to-assembly code-to-assembly.cpp)

# Link against LLVM libraries
target_link_libraries(code-to-assembly LLVMCore LLVMExecutionEngine LLVMInterpreter)