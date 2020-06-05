cmake_minimum_required(VERSION 3.10)

project(test_algo)

if(WIN32)
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

add_executable(test_algo ${SRC})
target_link_libraries(test_algo utils)