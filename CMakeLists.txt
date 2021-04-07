cmake_minimum_required(VERSION 3.10)

# set project name and title
project(fpsConsole VERSION 0.1)

# add compiler options
add_compile_options(-std=c++11)


# # set curses to enable wide characters
# set(CURSES_NEED_NCURSES "TRUE")
# set(CURSES_NEED_WIDE "TRUE")
# find Curses package
find_package(Curses REQUIRED)
find_package(SDL REQUIRED)

# if (NOT CURSES_FOUND)
#     add_definitions("-DNOCURSES")
# endif()

# directories for include directories
include_directories(include/fpsConsole/ ${CURSES_INCLUDE_DIRS} ${SDL_INCLUDE_DIR})

# specify cmake standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# for debugger
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CFLAGS} -g")

add_executable(${PROJECT_NAME}_fps  src/CommandLineFPS/CommandLineFPS.cpp)
target_link_libraries(${PROJECT_NAME}_fps ${CURSES_LIBRARIES} ${SDL_LIBRARY})

add_executable(${PROJECT_NAME}_triangle src/triangles.cpp)
target_link_libraries(${PROJECT_NAME}_triangle ${CURSES_LIBRARIES})
