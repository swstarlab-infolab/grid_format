cmake_minimum_required(VERSION 3.0)
project(GStream3)

# C++
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-Wall -O3 --std=c++14")

include_directories(${CMAKE_SOURCE_DIR}/include)
# link_directories(${CMAKE_SOURCE_DIR}/.)

# libmixx.a
file(GLOB_RECURSE LIBMIXX_SOURCE
	${CMAKE_SOURCE_DIR}/source/mixx/*
)

add_library(mixx ${LIBMIXX_SOURCE})

# libgstream.a
file(GLOB_RECURSE LIBGSTREAM_SOURCE
	${CMAKE_SOURCE_DIR}/source/gstream/*
)

add_library(gstream ${LIBGSTREAM_SOURCE})

add_executable(grid_sim ${GRIDSIM_SOURCE})

add_dependencies(gstream mixx)
target_link_libraries(gstream PUBLIC mixx)
target_link_libraries(grid_sim PUBLIC mixx gstream)
