cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)
project(StudioPacketDumper LANGUAGES CXX C)

# FETCH DEPS
find_package(Vulkan REQUIRED)

include(FetchContent)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG master)

FetchContent_MakeAvailable(glfw imgui)

# SETUP GLFW

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# SETUP IMGUI

add_library(imgui_glfw_vulkan STATIC
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp

	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

target_link_libraries(imgui_glfw_vulkan PUBLIC Vulkan::Vulkan glfw)
target_include_directories(imgui_glfw_vulkan PUBLIC
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends
)

# COMPILE PROJECT
set(PROJECT_SOURCE_FILES 
	src/main.cpp 
	src/gui.cpp
)

add_library(dumper SHARED ${PROJECT_SOURCE_FILES})

if(MSVC)
  target_compile_options(dumper PRIVATE /W4 /WX)
else()
  target_compile_options(dumper PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_compile_features(dumper PRIVATE cxx_std_17)
target_link_libraries(dumper PRIVATE imgui_glfw_vulkan)
