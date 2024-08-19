add_library(
    openrm_kalman
        SHARED
)
target_sources(
    openrm_kalman
        PRIVATE
        # ${CMAKE_SOURCE_DIR}/src/kalman/antitopV1.cpp
        # ${CMAKE_SOURCE_DIR}/src/kalman/antitopV2.cpp
        ${CMAKE_SOURCE_DIR}/src/kalman/antitopV3.cpp
        ${CMAKE_SOURCE_DIR}/src/kalman/outpostV1.cpp
        ${CMAKE_SOURCE_DIR}/src/kalman/outpostV2.cpp
        # ${CMAKE_SOURCE_DIR}/src/kalman/runeV1.cpp
        ${CMAKE_SOURCE_DIR}/src/kalman/runeV2.cpp
        # ${CMAKE_SOURCE_DIR}/src/kalman/trackqueueV1.cpp
        # ${CMAKE_SOURCE_DIR}/src/kalman/trackqueueV2.cpp
        ${CMAKE_SOURCE_DIR}/src/kalman/trackqueueV3.cpp
        ${CMAKE_SOURCE_DIR}/src/kalman/trackqueueV4.cpp
        ${CMAKE_SOURCE_DIR}/src/kalman/trajectoryV1.cpp
)
target_include_directories(
    openrm_kalman
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/openrm>
)