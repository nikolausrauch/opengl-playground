add_executable( temporal_anti_aliasing
    ${CMAKE_CURRENT_SOURCE_DIR}/temporal_anti_aliasing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/render.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/render.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/post.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/post.frag
)

target_link_libraries( temporal_anti_aliasing PRIVATE viewer )


target_compile_definitions( temporal_anti_aliasing PUBLIC ${VIEWER_DEFINES} )
target_compile_features( temporal_anti_aliasing PUBLIC cxx_std_20 )
set_target_properties( temporal_anti_aliasing PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target(temporal_anti_aliasing_copy_shader ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shader
    $<TARGET_FILE_DIR:temporal_anti_aliasing>/temporal_anti_aliasing/shader
    )

