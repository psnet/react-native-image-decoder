cmake_minimum_required(VERSION 3.4.1)
project(rn_imagedecoder)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)

set(PACKAGE_NAME "rn_imagedecoder")
set(COMMON_DIR ${CMAKE_SOURCE_DIR})

add_library(
    ${PACKAGE_NAME} SHARED
    ${COMMON_DIR}/src/main/cpp/cpp-adapter.cpp
)

find_package(ReactAndroid REQUIRED CONFIG)

target_compile_options(
    ${PACKAGE_NAME}
    PRIVATE -O2
            -fexceptions
            -frtti
            -Wall
            -Wextra
            -Wno-unused-parameter
            -Wshorten-64-to-32
            -Wstrict-prototypes
)

target_link_libraries(${PACKAGE_NAME} ReactAndroid::jsi android)
