cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(StudioPacketDumper LANGUAGES CXX C)

add_library(StudioPacketDumper SHARED "src/main.cpp")

target_compile_features(StudioPacketDumper PUBLIC cxx_std_17)