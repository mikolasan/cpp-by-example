
include_directories(thirdparty)
add_executable(cryptor cryptor.cpp)

#-----------------------------------

include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
  GIT_PROGRESS TRUE
  GIT_SHALLOW FALSE
  EXCLUDE_FROM_ALL
  FIND_PACKAGE_ARGS NAMES ftxui
)

FetchContent_Declare(nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
  EXCLUDE_FROM_ALL
  FIND_PACKAGE_ARGS NAMES nlohmann_json
)

FetchContent_GetProperties(ftxui)
FetchContent_GetProperties(nlohmann_json)

FetchContent_MakeAvailable(ftxui)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(test_protocol_ui test_protocol_ui.cpp)
# target_include_directories(test_protocol_ui ${ftxui_INCLUDE})
target_link_libraries(test_protocol_ui
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PUBLIC nlohmann_json::nlohmann_json
)