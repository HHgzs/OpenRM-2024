cmake_minimum_required(VERSION 3.15)
project(OpenRM-Test)

set(CMAKE_CXX_STANDARD 11)
# add_compile_options(-g -O0 -w -fno-omit-frame-pointer -Wno-notes)

add_compile_options(-O3 -w)

# Cuda
find_package(CUDA REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})

# OpenCV
find_package(OpenCV 4.5.4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Eigen
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# Ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

# OpenRM
find_package(OpenRM REQUIRED)
include_directories(${OpenRM_INCLUDE_DIRS})

message(STATUS "OpenRM_INCLUDE_DIRS: ${OpenRM_INCLUDE_DIRS}")
message(STATUS "OpenRM_LIBS: ${OpenRM_LIBS}")

add_executable(uvc_lib uvc_lib.cpp)
target_link_libraries(uvc_lib
    ${OpenRM_LIBS}
    ${OpenCV_LIBS}
    ${CERES_LIBRARIES}    
)
