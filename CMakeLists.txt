cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(PacketDumper LANGUAGES CXX C)


add_library(lib SHARED "src/main.cpp")