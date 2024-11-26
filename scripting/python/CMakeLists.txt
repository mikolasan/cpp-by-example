find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
message(STATUS "Found Python: ${Python3_EXECUTABLE} (${Python3_VERSION})")

add_executable(embed_test embed_test.cpp)

configure_file(neuron.py ${CMAKE_CURRENT_BINARY_DIR}/neuron.py COPYONLY)
target_include_directories(embed_test PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(embed_test PRIVATE ${Python3_LIBRARIES})