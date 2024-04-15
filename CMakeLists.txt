cmake_minimum_required(VERSION 3.8)

project(image_processor)

set(CMAKE_CXX_STANDARD 20)

add_library(contrib_catch_main
  contrib/catch/catch_main.cpp)

target_include_directories(contrib_catch_main
  PUBLIC contrib/catch)

# add_catch(test_bmp_reader tests/bmp_tests.cpp src/bmp_reader.cpp)
# add_catch(test_parsing tests/parsing_tests.cpp src/console_interface.cpp)
# add_catch(test_filters tests/filters_tests.cpp src/console_interface.cpp src/bmp_reader.cpp src/filters.cpp)

add_executable(
    image_processor
    src/filters.cpp
    src/bmp_reader.cpp
    src/console_interface.cpp
    src/processor.cpp
    image_processor.cpp
)
