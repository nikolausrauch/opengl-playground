add_executable( deferred_rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/deferred_rendering.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/gpass.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/gpass.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/light_dir.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/light_dir.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/light_spots.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/light_spots.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/debug_light.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/shader/debug_light.vert
)

target_link_libraries( deferred_rendering PRIVATE viewer )

target_compile_definitions( deferred_rendering PUBLIC ${VIEWER_DEFINES} )
target_compile_features( deferred_rendering PUBLIC cxx_std_20 )
set_target_properties( deferred_rendering PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target(deferred_rendering_copy_shader ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shader
    $<TARGET_FILE_DIR:deferred_rendering>/deferred_rendering/shader
    )
