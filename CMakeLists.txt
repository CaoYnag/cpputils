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
	add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS) # cancel min max macro in vs17
	
	set(CMAKE_DEBUG_POSTFIX "_d")
endif()
if(UNIX OR MINGW)
	set( CMAKE_CXX_FLAGS "-std=c++1z")
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		add_compile_options("-g")
	endif()
endif()

add_subdirectory(cpputils)
add_subdirectory(tests)
install(TARGETS utils 
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin)
install(FILES 
	cpputils/type.h
	cpputils/algo.h
	cpputils/math.h
	cpputils/color.h
	cpputils/image.h
	cpputils/io_png.h
	cpputils/imageio.h
	cpputils/canvas.h
	cpputils/canvas_func.h
	DESTINATION include/spes
)