cmake_minimum_required(VERSION 3.10)

project(test_wa)

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

	if($ENV{Boost_ROOT})
		message(FATAL_ERROR "env Boost_ROOT not set")
	else()
		include_directories(
			$ENV{Boost_ROOT}/
		)
	endif()
endif()

set(SRC
    ${PROJECT_SOURCE_DIR}/test.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${LIB_NAME})