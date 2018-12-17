cmake_minimum_required(VERSION 3.12)
project(my_string_view)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++1z -std=c++17 -Wall -Wextra -Werror -pedantic")

add_executable(string_view_test
  main.cpp
  string_view.test.cpp
  memory.test.cpp
  type_traits.test.cpp)

target_include_directories(string_view_test SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/../external/include")
