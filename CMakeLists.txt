cmake_minimum_required(VERSION 3.20)

project(cpp_helper)

add_library(cpp_helper
    cpp_helper.hpp
    cpp_helper.cpp
)
target_include_directories(cpp_helper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(cpp_helper_test
    main.cpp)

target_compile_features(cpp_helper_test PUBLIC cxx_std_20)

target_link_libraries(cpp_helper_test PUBLIC cpp_helper)
