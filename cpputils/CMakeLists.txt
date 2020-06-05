cmake_minimum_required(VERSION 3.10)

project(utils)
message(STATUS "some cpp utils.")



set(SRC
    ${PROJECT_SOURCE_DIR}/algo.cpp
    ${PROJECT_SOURCE_DIR}/math.cpp
)

set(LIB_NAME utils)
add_library(${LIB_NAME} STATIC ${SRC})
