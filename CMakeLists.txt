
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(mmtf-cpp VERSION 1.0.0 LANGUAGES CXX)

option(mmtf_build_local "Use the submodule dependencies for building" OFF)
option(mmtf_build_examples "Build the examples" OFF)

add_library(MMTFcpp INTERFACE)
target_compile_features(MMTFcpp INTERFACE cxx_auto_type)

target_include_directories(MMTFcpp INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (mmtf_build_local)
    # use header only
    set(MSGPACKC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/msgpack-c/include)
    add_library(msgpackc INTERFACE)
    target_include_directories(msgpackc INTERFACE ${MSGPACKC_INCLUDE_DIR})
    if (BUILD_TESTS)
        set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Catch2/single_include)
        add_library(Catch INTERFACE)
        target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})
    endif()
endif()

if (NOT TARGET msgpackc)
    find_package(msgpack)
endif()

target_link_libraries(MMTFcpp INTERFACE msgpackc)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()

if (mmtf_build_examples)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif()


if (build_py)

    if(NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE Release)
    endif()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    add_definitions(-DNDEBUG)
    add_library(mmtf_bindings SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/python_src/bindings.cpp
    )

    set(MSGPACKC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/msgpack-c/include)
	set(PYBIND_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pybind11_SRC/include)

    target_include_directories(mmtf_bindings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_include_directories(mmtf_bindings PUBLIC ${MSGPACKC_INCLUDE_DIR})
    target_include_directories(mmtf_bindings PUBLIC ${PYBIND_INCLUDE_DIR})
    target_include_directories(mmtf_bindings PUBLIC ${python_include_A})
    target_include_directories(mmtf_bindings PUBLIC ${python_include_B})

    set_target_properties(mmtf_bindings PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set_target_properties(mmtf_bindings PROPERTIES PREFIX "")
    set_target_properties(mmtf_bindings PROPERTIES SUFFIX ".so")
    set_target_properties(mmtf_bindings PROPERTIES CXX_STANDARD 17)
endif()

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION  "include"
    FILES_MATCHING PATTERN "*.hpp")
