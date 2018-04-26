project(my_string_view)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -Wall -Wextra -Werror -pedantic")

add_executable(string_view_test
  main.cpp
  string_view.test.cpp
  memory.test.cpp)
