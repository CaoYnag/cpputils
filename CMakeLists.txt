cmake_minimum_required(VERSION 3.10)

project(cpputils)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
link_directories(
    ${PROJECT_SOURCE_DIR}/bin
)

if(WIN32 AND NOT MINGW)
	add_compile_options("/std:c++latest")
	add_compile_options("/Zc:__cplusplus") # bugs in vs for nlohmann json
	add_definitions(-DNOMINMAX) # cancel min max macro in vs17
	
	set(CMAKE_DEBUG_POSTFIX "_d")
endif()
if(UNIX OR MINGW)
	set( CMAKE_CXX_FLAGS "-std=c++1z")
endif()

add_subdirectory(cpputils)
add_subdirectory(tests)