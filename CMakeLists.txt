cmake_minimum_required(VERSION 3.15)
project(OpenRM)


set(CMAKE_CXX_STANDARD 20)

# set(CMAKE_BUILD_TYPE DEBUG)
# add_compile_options(-g -O0 -w -fno-omit-frame-pointer -Wno-notes)

set(CMAKE_BUILD_TYPE RELEASE)
add_compile_options(-O3 -w)

# OpenCV
find_package(OpenCV 4.5.4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Eigen
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# Ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

# CUDA
find_package(CUDA)
if (CUDA_FOUND)
    message(STATUS "CUDA Version: ${CUDA_VERSION}")
    include_directories(/usr/local/cuda/include)
endif()

# 设置主版本号、次版本号和补丁版本号
set(OPENRM_VERSION_MAJOR 1)
set(OPENRM_VERSION_MINOR 0)
set(OPENRM_VERSION_PATCH 0)

# 生成完整的版本号
set(OPENRM_VERSION ${OPENRM_VERSION_MAJOR}.${OPENRM_VERSION_MINOR}.${OPENRM_VERSION_PATCH})

# 将宏定义添加到项目
add_definitions(-DOPENRM_VERSION="${OPENRM_VERSION}")
# add_definitions(-DPOINTER_IO)

# 将版本号定义为预定义宏，使其在程序内部可访问
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/VersionConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/VersionConfig.h)


# 添加子目录
add_subdirectory(src/attack)
add_subdirectory(src/kalman)
add_subdirectory(src/pointer)
add_subdirectory(src/solver)
add_subdirectory(src/uniterm)
add_subdirectory(src/utils)
add_subdirectory(src/video)

if (CUDA_FOUND)
    add_subdirectory(src/tensorrt)
    add_subdirectory(cuda)
endif()


# 添加目标文件
if (CUDA_FOUND)
    set(
        TARGETS_LIST
            openrm_attack
            openrm_kalman
            openrm_pointer
            openrm_solver
            openrm_delay
            openrm_print
            openrm_serial
            openrm_tf
            openrm_timer
            openrm_uniterm
            openrm_video
            openrm_tensorrt
            openrm_cudatools
    )
else()
    set(
        TARGETS_LIST
            openrm_attack
            openrm_kalman
            openrm_pointer
            openrm_solver
            openrm_delay
            openrm_print
            openrm_serial
            openrm_tf
            openrm_timer
            openrm_uniterm
            openrm_video
    )
endif()

# 安装
install(
    TARGETS
        ${TARGETS_LIST}
    EXPORT
        OpenRMTarget
    RUNTIME DESTINATION
        bin
    LIBRARY DESTINATION
        lib
    ARCHIVE DESTINATION
        lib
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/VersionConfig.h
    DESTINATION
        include/openrm
)

if (CUDA_FOUND)
    install(
        FILES
            ${CMAKE_SOURCE_DIR}/cuda/include/cudatools.h
        DESTINATION
            include/openrm
    )
endif()

install(
    DIRECTORY
        ${CMAKE_SOURCE_DIR}/include/
    DESTINATION 
        include/openrm
)

install(
    EXPORT
        OpenRMTarget
    NAMESPACE
        openrm::
    DESTINATION
        lib/cmake/openrm
)

include(CMakePackageConfigHelpers)

if (CUDA_FOUND)
    configure_package_config_file(
        ${CMAKE_SOURCE_DIR}/cmake/OpenRMConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/OpenRMConfig.cmake
        INSTALL_DESTINATION
            lib/cmake/openrm
    )
else()
    configure_package_config_file(
        ${CMAKE_SOURCE_DIR}/cmake/OpenRMConfig.cmake.in.nocuda
        ${CMAKE_CURRENT_BINARY_DIR}/OpenRMConfig.cmake
        INSTALL_DESTINATION
            lib/cmake/openrm
    )
endif()


install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/OpenRMConfig.cmake
    DESTINATION
        lib/cmake/openrm
)

# 卸载
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()

