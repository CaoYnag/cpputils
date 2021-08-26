cmake_minimum_required(VERSION 3.10)

project(utils)
message(STATUS "some cpp utils.")

set(SRC
    ${PROJECT_SOURCE_DIR}/type.cpp
    ${PROJECT_SOURCE_DIR}/algo.cpp
    ${PROJECT_SOURCE_DIR}/math.cpp
    ${PROJECT_SOURCE_DIR}/color.cpp
    ${PROJECT_SOURCE_DIR}/image.cpp
    ${PROJECT_SOURCE_DIR}/canvas.cpp
    ${PROJECT_SOURCE_DIR}/canvas_func.cpp
	${PROJECT_SOURCE_DIR}/io_png.cpp
	${PROJECT_SOURCE_DIR}/io_jpeg.cpp
	${PROJECT_SOURCE_DIR}/io_gif.cpp
	${PROJECT_SOURCE_DIR}/imageio.cpp
	${PROJECT_SOURCE_DIR}/utils.cpp)

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
	add_compile_options("-static")
endif()

set(LIB_NAME spes)
add_library(${LIB_NAME} STATIC ${SRC})
target_link_libraries(${LIB_NAME} png jpeg)
if(WIN32)
	target_link_libraries(${LIB_NAME} OpenGL32 glu32 glew32)
endif()
if(UNIX)
	target_link_libraries(${LIB_NAME} ${FC_DEP_LIBS} pthread xcb)
endif()
