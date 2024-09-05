add_executable( reflection_probes ${CMAKE_CURRENT_SOURCE_DIR}/reflection_probes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/scene.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/water.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/water.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/refl_prope.geom
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/refl_prope.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/debug_prope.frag
)

target_link_libraries( reflection_probes PRIVATE viewer )

target_compile_definitions( reflection_probes PUBLIC ${VIEWER_DEFINES} )
target_compile_features( reflection_probes PUBLIC cxx_std_20 )
set_target_properties( reflection_probes PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target(reflection_probes_copy_shader ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shader
    $<TARGET_FILE_DIR:reflection_probes>/reflection_probes/shader
    )
