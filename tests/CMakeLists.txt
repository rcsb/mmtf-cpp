
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

add_executable(mmtf_tests mmtf_tests.cpp)
target_compile_features(mmtf_tests PRIVATE cxx_auto_type)
if(WIN32)
    target_link_libraries(mmtf_tests Catch msgpackc MMTFcpp ws2_32)
else()
    target_link_libraries(mmtf_tests Catch msgpackc MMTFcpp)
endif()

# test for multi-linking
add_executable(multi_cpp_test multi_cpp_test.cpp multi_cpp_test_helper.cpp)
target_compile_features(multi_cpp_test PRIVATE cxx_auto_type)
if(WIN32)
    target_link_libraries(multi_cpp_test MMTFcpp ws2_32)
else()
    target_link_libraries(multi_cpp_test MMTFcpp)
endif()
