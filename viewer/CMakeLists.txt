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
    "${CMAKE_CURRENT_SOURCE_DIR}/viewer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/frameclock.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/log.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/keyboard.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/mouse.cpp"
 )

set( CORE_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/viewer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/platform.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/msg_bus.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/msg.h"
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
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/model.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/obj_model.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset/pointcloud.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/asset/detail/obj_model.h"
 )

set( UTILITY_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/camera_control.cpp"
 )

set( UTILITY_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/camera.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/camera_control.h"
 )

set( GUI_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/manager.cpp"
 )

set( GUI_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/manager.h"
 )

#################################
#   Platform Specific Source    #
#################################
set( PLATFORM_GLFW_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/base.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/keyboard.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/mouse.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/window.cpp"
 )

set( PLATFORM_GLFW_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw/base.h"
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
set( VIEWER_SRC ${CORE_SRC} ${PLATFORM_OPENGL_SRC} ${PLATFORM_GLFW_SRC} ${ASSET_SRC} ${GUI_SRC} ${UTILITY_SRC} )
set( VIEWER_HDR ${CORE_HDR} ${PLATFORM_OPENGL_HDR} ${PLATFORM_GLFW_HDR} ${ASSET_HDR} ${GUI_HDR} ${UTILITY_HDR} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VIEWER_SRC} ${VIEWER_HDR})


#################################
#      Build GUI Executable     #
#################################
add_library( viewer STATIC ${VIEWER_SRC} ${VIEWER_HDR} )

target_link_libraries( viewer Threads::Threads OpenGL::GL glfw glm_static glad_static imgui_static stb_image fmt tiny_obj tinyply )
target_include_directories( viewer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    )

target_compile_definitions( viewer PUBLIC ${VIEWER_DEFINES} )
target_compile_features( viewer PUBLIC cxx_std_20 )
set_target_properties( viewer PROPERTIES CXX_EXTENSIONS OFF )
