cmake_minimum_required(VERSION 3.15)
project(AzureBot-Demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RELEASE)
add_compile_options(-O3 -w)

# ============================================================================
# Dependencies - All cross-platform compatible
# ============================================================================

# OpenCV (Required - available on both macOS and Linux)
find_package(OpenCV 4.5.4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Eigen (Required - available on both macOS and Linux)
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# Ceres (Required - for PnP and Kalman filters)
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

# OpenRM (Our custom library)
find_package(OpenRM REQUIRED)
include_directories(${OpenRM_INCLUDE_DIRS})

message(STATUS "========================================")
message(STATUS "AzureBot Demo Build Configuration")
message(STATUS "========================================")
message(STATUS "OpenCV Version: ${OpenCV_VERSION}")
message(STATUS "OpenRM Include: ${OpenRM_INCLUDE_DIRS}")
message(STATUS "OpenRM Libraries: ${OpenRM_LIBS}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "========================================")

# ============================================================================
# Demo Executables
# ============================================================================

# Demo 1: Real-time armor detection from camera
add_executable(armor_detector_demo armor_detector_demo.cpp)
target_link_libraries(armor_detector_demo
    ${OpenRM_LIBS}
    ${OpenCV_LIBS}
    ${CERES_LIBRARIES}
)

# Demo 2: Simple camera test
add_executable(camera_test camera_test.cpp)
target_link_libraries(camera_test
    ${OpenCV_LIBS}
)

message(STATUS "Build targets created:")
message(STATUS "  - armor_detector_demo: Full armor detection pipeline")
message(STATUS "  - camera_test: Simple camera verification")
message(STATUS "========================================")

