
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

SET(executables mmtf_demo traverse)

foreach(exe ${executables})
	add_executable(${exe} ${exe}.cpp)
	target_compile_features(${exe} PRIVATE cxx_auto_type)
	target_link_libraries(${exe} MMTFcpp)
endforeach(exe)

