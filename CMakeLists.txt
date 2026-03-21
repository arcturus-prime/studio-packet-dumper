cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(StudioPacketDumper LANGUAGES CXX C)

include(FetchContent)

set(ZSTD_BUILD_STATIC ON)
set(ZSTD_BUILD_SHARED OFF)

FetchContent_Declare(
    zstd
    GIT_REPOSITORY https://github.com/facebook/zstd.git
    GIT_TAG "v1.5.7"
)

FetchContent_MakeAvailable(zstd)

add_library(dumper SHARED "src/main.cpp")

target_link_libraries(
    dumper
    PRIVATE
    libzstd_static
)

target_include_directories(
    dumper
    PRIVATE
    "${zstd_SOURCE_DIR}/lib"
)

if(MSVC)
    target_compile_options(dumper PRIVATE /W4 /WX)
else()
    target_compile_options(dumper PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_compile_features(dumper PRIVATE cxx_std_17)
