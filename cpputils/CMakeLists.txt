cmake_minimum_required(VERSION 3.10)

project(utils)
message(STATUS "some cpp utils.")

set(SRC
    ${PROJECT_SOURCE_DIR}/type.cpp
    ${PROJECT_SOURCE_DIR}/algo.cpp
    ${PROJECT_SOURCE_DIR}/math.cpp
    ${PROJECT_SOURCE_DIR}/color.cpp
    ${PROJECT_SOURCE_DIR}/image.cpp
    ${PROJECT_SOURCE_DIR}/imageio.cpp
    ${PROJECT_SOURCE_DIR}/io_png.cpp
)

if(WIN32)
	if($ENV{LIB_BASE})
	else()
		include_directories(
			$ENV{LIB_BASE}/include
		)
		link_directories(
			$ENV{LIB_BASE}/lib
		)
	endif()
endif()

set(LIB_NAME utils)
add_library(${LIB_NAME} STATIC ${SRC})
target_link_libraries(${LIB_NAME} png)
