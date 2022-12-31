set(CMAKE_CXX_STANDARD 17)
add_executable(holding holding.cpp)
add_executable(numbers-to-stream numbers-to-stream.cpp)
add_executable(isaac isaac.cpp)

add_library(project_options INTERFACE)
target_compile_options(project_options INTERFACE -fomit-frame-pointer)

FIND_PACKAGE(Boost 1.80 COMPONENTS math_c99 REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
ADD_EXECUTABLE(chi_square chi_square_std_dev_test.cpp)
TARGET_LINK_LIBRARIES(chi_square LINK_PUBLIC ${Boost_LIBRARIES})
target_link_libraries(chi_square PRIVATE project_options)

add_executable(bias_finder bias-finder.cpp)
TARGET_LINK_LIBRARIES(bias_finder LINK_PUBLIC ${Boost_LIBRARIES})

add_executable(bias_picture bias-picture.cpp)
add_executable(scale scale.cpp)
add_executable(rand_picture rand-picture.cpp)