cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED ENV{PSN00BSDK_LIBS})
    message(FATAL_ERROR "Set PSN00BSDK_LIBS to the SDK lib/libpsn00b directory")
endif()

project(PS3System355 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

psn00bsdk_add_executable(ps3system GPREL
    src/main.c
)

psn00bsdk_add_cd_image(ps3system_disc ps3_system_355
    "${PROJECT_SOURCE_DIR}/disc/iso.xml"
    DEPENDS ps3system
)
