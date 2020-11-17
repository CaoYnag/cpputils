cmake_minimum_required(VERSION 3.10)
project(topo_sort)

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

	find_package(Boost 1.67 COMPONENTS)
	if(Boost_FOUND)
		include_directories(${Boost_INCLUDE_DIRS})
	endif()
endif()

set(SRC
    ${PROJECT_SOURCE_DIR}/test.cpp
)

add_executable(topo_sort ${SRC})