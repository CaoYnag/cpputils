cmake_minimum_required(VERSION 3.10)

project(cpputils VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ".")
set(LIBRARY_OUTPUT_PATH ".")

option(BUILD_TESTS "build tests and unit_tests" ON)
option(BUILD_SAMPLES "build some sample program" ON)

set(CMAKE_IGNORE_PATH "${CMAKE_INSTALL_PREFIX}/spes")

include_directories(
		${PROJECT_SOURCE_DIR}/include
)
link_directories(
		${PROJECT_SOURCE_DIR}/bin
)

if(WIN32 AND NOT MINGW)
	add_compile_options("/std:c++latest")
	add_compile_options("/Zc:__cplusplus") # bugs in vs for nlohmann json
	add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS) # cancel min max macro in vs17

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_DEBUG_POSTFIX "_d")
endif()
if(UNIX OR MINGW)
	set( CMAKE_CXX_FLAGS "-std=c++1z")
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		add_compile_options("-g")
	endif()
endif()

set(LIB_NAME spes)
add_subdirectory(src)

if(BUILD_TESTS)
	add_subdirectory(tests)
	add_subdirectory(benchmarks)
	add_subdirectory(unit_tests)
endif()
if(BUILD_SAMPLES)
	add_subdirectory(prac)
endif()
install(TARGETS ${LIB_NAME}
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin)
file(GLOB header_files "${PROJECT_SOURCE_DIR}/include/spes/*.h")
install(FILES
		${header_files}
		DESTINATION include/spes
)
