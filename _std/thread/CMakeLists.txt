add_executable(condition_variable condition-variable.cpp)
add_executable(condition_variable_future condition-variable-and-future.cpp)
add_executable(static_threads static-threads.cpp)
add_executable(lambda_in_thread lambda-in-thread.cpp)

add_executable(wait_for_input wait-for-input.cpp)
target_link_libraries(wait_for_input PRIVATE ncurses)