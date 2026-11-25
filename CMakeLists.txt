cmake_minimum_required(VERSION 3.21)

project(
    PS3System355
    LANGUAGES C CXX ASM
    VERSION 1.0.0
    DESCRIPTION "PS3 3.55 style XMB simulation for PS1"
)

psn00bsdk_add_executable(ps3system GPREL
    src/main.c
)

psn00bsdk_add_cd_image(
    ps3system_disc
    ps3_system_355
    "${PROJECT_SOURCE_DIR}/disc/iso.xml"
    DEPENDS ps3system
)
