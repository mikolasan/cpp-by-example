add_executable(conv2d conv2d.cpp)

if(WIN32)
  set(ZLIB_DIR ${CMAKE_SOURCE_DIR}/thirdparty/windows/zlib)
  target_include_directories(conv2d PUBLIC 
    "C:\\Program Files\\NVIDIA\\CUDNN\\v8.9\\include"
    "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v11.8\\include"
  )
  target_link_directories(conv2d PUBLIC 
    "C:\\Program Files\\NVIDIA\\CUDNN\\v8.9\\lib\\x64"
    "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v11.8\\lib\\x64"
    ${ZLIB_DIR}
  )
  target_link_libraries(conv2d cublas cublasLt zlibwapi)
endif()