cmake_minimum_required(VERSION 3.10)

# set project name and title
project(fpsConsole VERSION 0.1)

# add compiler options
add_compile_options(-std=c++11)

# find Curses package
find_package(Curses REQUIRED)

# directories for include directories
include_directories(${CURSES_INCLUDE_DIRS})

# specify cmake standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# for debugger
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CFLAGS} -g")

add_executable(${PROJECT_NAME}_fps  src/CommandLineFPS/CommandLineFPS.cpp)
target_link_libraries(${PROJECT_NAME}_fps ${CURSES_LIBRARIES})

add_executable(${PROJECT_NAME}_triangle src/triangles.cpp)
target_link_libraries(${PROJECT_NAME}_triangle ${CURSES_LIBRARIES})
