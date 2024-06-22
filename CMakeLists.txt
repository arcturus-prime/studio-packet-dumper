cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(StudioPacketDumper LANGUAGES CXX C)

add_library(StudioPacketDumper SHARED "src/main.cpp")

if(MSVC)
  target_compile_options(StudioPacketDumper PRIVATE /W4 /WX)
else()
  target_compile_options(StudioPacketDumper PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_compile_features(StudioPacketDumper PRIVATE cxx_std_17)