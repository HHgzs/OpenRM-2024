add_library(
    openrm_delay
        SHARED
)
target_sources(
    openrm_delay
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src/utils/delay.cpp
)
target_include_directories(
    openrm_delay
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/openrm>
)



add_library(
    openrm_print
        SHARED
)
target_sources(
    openrm_print
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src/utils/print.cpp
)
target_include_directories(
    openrm_print
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/openrm>
)




add_library(
    openrm_serial
        SHARED
)
target_sources(
    openrm_serial
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src/utils/serial.cpp
)
target_include_directories(
    openrm_serial
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/openrm>
)




add_library(
    openrm_tf
        SHARED
)
target_sources(
    openrm_tf
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src/utils/tf_rotate.cpp
        ${CMAKE_SOURCE_DIR}/src/utils/tf_trans.cpp
        ${CMAKE_SOURCE_DIR}/src/utils/tf_tools.cpp
)
target_include_directories(
    openrm_tf
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/openrm>
)




add_library(
    openrm_timer
        SHARED
)
target_sources(
    openrm_timer
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src/utils/timer.cpp
)
target_include_directories(
    openrm_timer
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/openrm>
)
