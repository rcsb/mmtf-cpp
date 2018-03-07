
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

add_executable(mmtf_tests mmtf_tests.cpp)
target_compile_features(mmtf_tests PRIVATE cxx_auto_type)
target_link_libraries(mmtf_tests Catch msgpackc MMTFcpp)

