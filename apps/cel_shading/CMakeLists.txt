add_executable( cel_shading
        ${CMAKE_CURRENT_SOURCE_DIR}/cel_shading.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/cel_shading.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/cel_shading.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/basic.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/edge_detect.frag
        )

target_link_libraries( cel_shading PRIVATE viewer )

target_compile_definitions( cel_shading PUBLIC ${VIEWER_DEFINES} )
target_compile_features( cel_shading PUBLIC cxx_std_20 )
set_target_properties( cel_shading PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target(cel_shading_copy_shader ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shader
    $<TARGET_FILE_DIR:cel_shading>/cel_shading/shader
    )
