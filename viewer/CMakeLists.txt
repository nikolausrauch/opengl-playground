#########################################
#       OpenGL-Wrapper and Viewer       #
#---------------------------------------#
# @author Nikolaus Rauch                #
# @date 13.02.2020                      #
#---------------------------------------#
#                                       #
#########################################

#################################
#          Libraries            #
#################################
find_package( Threads REQUIRED )

set( OpenGL_GL_PREFERENCE GLVND )
find_package( OpenGL 3.0 REQUIRED )


#################################
#     Engine Source Files       #
#################################
set( CORE_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/core/frameclock.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/log.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/keyboard.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/mouse.cpp"
 )

set( CORE_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/core/platform.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/frameclock.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/log.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/assert.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/time.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/keyboard.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/mouse.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/window.h"
 )

set( ASSET_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/image.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/texture.cpp"
 )

set( ASSET_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/fwd.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/image.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/texture.h"
 )

#################################
#   Platform Specific Source    #
#################################
set( PLATFORM_GLFW_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/keyboard.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/mouse.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/window.cpp"
 )

set( PLATFORM_GLFW_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/keyboard.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/mouse.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/window.h"
 )

set( PLATFORM_OPENGL_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/context.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/shaderprogram.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/vertexarray.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/texture.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/renderbuffer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/framebuffer.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/buffer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/indexbuffer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/vertexbuffer.h"
 )

set( PLATFORM_OPENGL_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/shaderprogram.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/vertexarray.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/texture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/renderbuffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl/framebuffer.cpp"
 )


#################################
#         Source Group          #
#################################
set( GUI_SRC ${CORE_SRC} ${PLATFORM_OPENGL_SRC} ${PLATFORM_GLFW_SRC} ${ASSET_SRC} )
set( GUI_HDR ${CORE_HDR} ${PLATFORM_OPENGL_HDR} ${PLATFORM_GLFW_HDR} ${ASSET_HDR} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GUI_SRC} ${GUI_HDR})


#################################
#      Build GUI Executable     #
#################################
add_library( viewer STATIC ${GUI_SRC} ${GUI_HDR} )

target_link_libraries( viewer Threads::Threads OpenGL::GL glfw glm_static glad_static imgui_static stb_image fmt )
target_include_directories( viewer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    )

target_compile_definitions( viewer PUBLIC ${ENGINE_DEFINES} )
target_compile_features( viewer PUBLIC cxx_std_20 )
set_target_properties( viewer PROPERTIES CXX_EXTENSIONS OFF )