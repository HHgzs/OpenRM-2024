cmake_minimum_required(VERSION 3.15)
project(OpenRM-terminal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE DEBUG)
add_compile_options(-g -O0 -w -fno-omit-frame-pointer -Wno-notes)

# OpenRM
find_package(OpenRM REQUIRED)
include_directories(${OpenRM_INCLUDE_DIRS})

# CUDA
find_package(CUDA REQUIRED)
include_directories(/usr/local/cuda/include)

# OpenCV
find_package(OpenCV 4.5.4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Eigen
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# Ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(
    openrm
        ${CMAKE_SOURCE_DIR}/src/main.cpp
)
target_link_libraries(
    openrm
        ${OpenRM_LIBS}
        ${OpenCV_LIBS}
        ncurses
)

install(
    TARGETS
        openrm
    RUNTIME DESTINATION
        bin
)