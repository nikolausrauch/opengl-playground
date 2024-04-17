add_executable( shadow_mapping
    ${CMAKE_CURRENT_SOURCE_DIR}/shadow_mapping.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/blinn_phong.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/blinn_phong.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/shadow_map.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/shadow_map.frag
)

target_link_libraries( shadow_mapping PRIVATE viewer )

target_compile_definitions( shadow_mapping PUBLIC ${VIEWER_DEFINES} )
target_compile_features( shadow_mapping PUBLIC cxx_std_20 )
set_target_properties( shadow_mapping PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target(shadow_mapping_copy_shader ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shader
    $<TARGET_FILE_DIR:shadow_mapping>/shadow_mapping/shader
    )
