add_library(
    openrm_video
        SHARED
)

# =============================================================================
# CROSS-PLATFORM COMPATIBILITY: UVC Camera Support
# =============================================================================
# 
# PROBLEM: uvc.cpp uses Linux-specific headers (<linux/videodev2.h>)
#          These headers don't exist on macOS/Windows, causing compilation to fail
#
# SOLUTION: Conditionally include uvc.cpp only on Linux platforms
#
# BEHAVIOR:
#   On Linux (Jetson Orin):
#     - HAVE_UVC = TRUE
#     - uvc.cpp IS compiled and included in the library
#     - Full UVC camera support available for deployment
#
#   On macOS (Student Development):
#     - HAVE_UVC = FALSE
#     - uvc.cpp is NOT compiled or included
#     - Library builds successfully without UVC support
#     - Students can develop/test other modules without cameras
#
# KEY BENEFIT: 
#   - Same codebase works on all platforms
#   - No need for separate branches or manual code changes
#   - Students on Mac can build, develop, and test algorithms
#   - Production deployment on Jetson works with full camera support
# =============================================================================

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Running on Linux (Jetson Orin Mini PC, Ubuntu, etc.)
    set(HAVE_UVC TRUE)
    message(STATUS "Linux detected, enabling UVC camera support.")
else()
    # Running on macOS, Windows, or other non-Linux systems
    set(HAVE_UVC FALSE)
    message(STATUS "Non-Linux system detected, disabling UVC camera support.")
endif()

# =============================================================================
# OPTIONAL: DaHeng Industrial Camera Support
# =============================================================================
# Similar pattern for DaHeng cameras - only include if driver is found
# =============================================================================

# 查找 gxiapi 动态链接库
find_library(GXIAPI_LIB
    NAMES gxiapi
)

# 检查库是否找到
if(GXIAPI_LIB)
    message(STATUS "Found DaHeng: ${GXIAPI_LIB}")
    set(HAVE_GXIAPI TRUE)
else()
    message(STATUS "Could NOT found DaHeng libgxiapi.so, disable DaHeng camera support.")
    set(HAVE_GXIAPI FALSE)
endif()

# =============================================================================
# SOURCE FILES: Conditionally included based on platform/hardware availability
# =============================================================================
# Syntax: $<IF:$<BOOL:${CONDITION}>,file_if_true,file_if_false>
#   - If HAVE_UVC is TRUE: include uvc.cpp
#   - If HAVE_UVC is FALSE: skip uvc.cpp (empty second parameter)
# =============================================================================

target_sources(
    openrm_video
        PRIVATE
        $<IF:$<BOOL:${HAVE_UVC}>,${CMAKE_SOURCE_DIR}/src/video/uvc.cpp,>      # Linux only
        ${CMAKE_SOURCE_DIR}/src/video/tools.cpp                                # All platforms
        $<IF:$<BOOL:${HAVE_GXIAPI}>,${CMAKE_SOURCE_DIR}/src/video/daheng.cpp,> # If driver found
)

target_include_directories(
    openrm_video
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/openrm>
)

target_link_libraries(
    openrm_video
        PRIVATE
        ${OpenCV_LIBS}
        openrm_uniterm
        openrm_timer
        $<IF:$<BOOL:${HAVE_GXIAPI}>,gxiapi,>
)