cmake_minimum_required(VERSION 3.10)

project(test_mw)

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
if(UNIX)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
	include_directories(${GTK2_INCLUDE_DIRS})
	link_directories(${GTK2_LIBRARY_DIRS})
	list(APPEND FC_DEP_LIBS ${GTK2_LIBRARIES})
	pkg_check_modules(GTH2 REQUIRED gthread-2.0)
	include_directories(${GTH2_INCLUDE_DIRS})
	link_directories(${GTH2_LIBRARY_DIRS})
	list(APPEND FC_DEP_LIBS ${GTH2_LIBRARIES})
endif()

set(SRC
    ${PROJECT_SOURCE_DIR}/test.cpp
)

add_executable(test_mw ${SRC})
target_link_libraries(test_mw ${LIB_NAME} ${FC_DEP_LIBS})