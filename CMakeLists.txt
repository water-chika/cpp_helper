cmake_minimum_required(VERSION 3.20)

project(cpp_helper)

add_library(cpp_helper INTERFACE
    cpp_helper.hpp
)
target_include_directories(cpp_helper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(cpp_helper INTERFACE cxx_std_20)

target_link_libraries(cpp_helper INTERFACE hip::host hip::device)

add_executable(cpp_helper_test
    main.cpp)

target_link_libraries(cpp_helper_test PUBLIC cpp_helper)
