add_catch(test_bmp_reader tests/bmp_tests.cpp src/bmp_reader.cpp)
add_catch(test_parsing tests/parsing_tests.cpp src/console_interface.cpp)
add_catch(test_filters tests/filters_tests.cpp src/console_interface.cpp src/bmp_reader.cpp src/filters.cpp)

add_executable(
    image_processor
    src/filters.cpp
    src/bmp_reader.cpp
    src/console_interface.cpp
    src/processor.cpp
    image_processor.cpp
)
