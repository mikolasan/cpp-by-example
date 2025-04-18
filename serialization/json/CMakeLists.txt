
# include(FetchContent)

# FetchContent_Declare(nlohmann_json
#   URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
#   EXCLUDE_FROM_ALL
#   FIND_PACKAGE_ARGS NAMES nlohmann_json
# )

# FetchContent_GetProperties(nlohmann_json)
# FetchContent_MakeAvailable(nlohmann_json)

# add_executable(json_test json.cpp)
# target_link_libraries(json_test
#   PUBLIC nlohmann_json::nlohmann_json
# )

configure_file(example.json ${CMAKE_CURRENT_BINARY_DIR}/example.json COPYONLY)