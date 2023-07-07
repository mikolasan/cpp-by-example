# Arch: pacman -S opencv vtk

# list(APPEND CMAKE_PREFIX_PATH /usr/lib/cmake/opencv4)

find_package(OpenCV REQUIRED)

add_executable(crop_image crop_image.cpp)

if (OpenCV_FOUND)
  message("Found OpenCV")
  message("Includes: " ${OpenCV_INCLUDE_DIRS})
  target_include_directories(crop_image PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(crop_image ${OpenCV_LIBS} )
endif (OpenCV_FOUND)

configure_file(test.png ${CMAKE_CURRENT_BINARY_DIR}/test.png COPYONLY)
configure_file(haarcascade_frontalcatface_extended.xml ${CMAKE_CURRENT_BINARY_DIR}/haarcascade_frontalcatface_extended.xml COPYONLY)
