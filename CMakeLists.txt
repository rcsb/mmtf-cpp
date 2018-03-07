
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(mmtf-cpp VERSION 1.0.0 LANGUAGES CXX)

option(mmtf_build_local "Use the submodule dependencies for building" OFF)
option(mmtf_build_examples "Build the examples" OFF)

add_library(MMTFcpp INTERFACE)
target_compile_features(MMTFcpp INTERFACE cxx_auto_type)
target_compile_options(MMTFcpp INTERFACE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

target_include_directories(MMTFcpp INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (mmtf_build_local)
	add_subdirectory(msgpack-c)
	if (BUILD_TESTS)
		add_subdirectory(Catch2)
	endif()
endif()

if (BUILD_TESTS)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()


