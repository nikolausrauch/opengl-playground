############################################
#            OpenGL-Playground             #
#------------------------------------------#
# @author  Nikolaus Rauch                  #
# @date    13.02.2023                      #
#------------------------------------------#
############################################
cmake_minimum_required( VERSION 3.9 )
project( opengl-playground )

message("\n * OpenGL Playground ")
message("\n * https://github.com/nikolausrauch/opengl-playground \n")

message(STATUS "${PROJECT_NAME} build environment --")
message(STATUS "Build system:     ${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}")

#########################################
#                Options                #
#########################################
option(VIEWER_DEBUG "build with debug features" OFF)
option(VIEWER_LOG "build with logging features" ON)

if(VIEWER_DEBUG)
    set(VIEWER_DEFINES "${VIEWER_DEFINES}; PLATFORM_DEBUG")
    message(STATUS "viewer debug mode enabled")
endif()

if(VIEWER_LOG)
    set(VIEWER_DEFINES "${VIEWER_DEFINES}; PLATFORM_LOG")
    message(STATUS "viewer logging enabled")
endif()

#########################################
#              CMake-Stuff              #
#########################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)


#########################################
#        Build External-Libraries       #
#########################################
add_subdirectory(external/glm)
target_compile_options(glm_static PRIVATE "$<$<CXX_COMPILER_ID:GNU>:${GCC_COMPILE_SUPPRESS_WARING}>")
target_include_directories( glm_static PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external>)

add_subdirectory(external/glad)
target_compile_options(glad_static PRIVATE "$<$<CXX_COMPILER_ID:GNU>:${GCC_COMPILE_SUPPRESS_WARING}>")
target_include_directories( glad_static PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include> )

add_subdirectory(external/glfw)
target_compile_options(glfw PRIVATE "$<$<CXX_COMPILER_ID:GNU>:${GCC_COMPILE_SUPPRESS_WARING}>")
target_include_directories( glfw PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include> )

add_subdirectory(external/imgui)
target_compile_options(imgui_static PRIVATE "$<$<CXX_COMPILER_ID:GNU>:${GCC_COMPILE_SUPPRESS_WARING}>")

add_subdirectory(external/stb_image)
target_compile_options(stb_image PRIVATE "$<$<CXX_COMPILER_ID:GNU>:${GCC_COMPILE_SUPPRESS_WARING}>")

add_subdirectory(external/fmt)
target_compile_options(fmt PRIVATE "$<$<CXX_COMPILER_ID:GNU>:${GCC_COMPILE_SUPPRESS_WARING}>")
target_include_directories( fmt PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/fmt/include> )


#########################################
#        Global Compiler Options        #
#########################################
set(GCC_COMPILE_OPTIONS "-Wall")
set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-ggdb;-O0")
set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(MSVC_COMPILE_OPTIONS "/Wall")
set(MSVC_COMPILE_DEBUG_OPTIONS "${MSVC_COMPILE_OPTIONS};/Zi")
set(MSVC_COMPILE_RELEASE_OPTIONS "${MSVC_COMPILE_OPTIONS};/O2")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:${GCC_COMPILE_DEBUG_OPTIONS}>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:${GCC_COMPILE_RELEASE_OPTIONS}>")

add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:${MSVC_COMPILE_DEBUG_OPTIONS}>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:${MSVC_COMPILE_RELEASE_OPTIONS}>")


#########################################
#    Build OpenGL Wrapper and Viewer    #
#########################################
add_subdirectory(viewer)

#########################################
#            Build Examples             #
#########################################
add_subdirectory(apps)
