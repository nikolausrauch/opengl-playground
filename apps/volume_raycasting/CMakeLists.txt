add_executable( volume_raycasting
        ${CMAKE_CURRENT_SOURCE_DIR}/volume_raycasting.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/entry_exit.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/entry_exit.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/simple.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/raycast.frag
        )

target_link_libraries( volume_raycasting PRIVATE viewer )

target_compile_definitions( volume_raycasting PUBLIC ${VIEWER_DEFINES} )
target_compile_features( volume_raycasting PUBLIC cxx_std_20 )
set_target_properties( volume_raycasting PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target(volume_raycasting_copy_shader ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shader
    $<TARGET_FILE_DIR:volume_raycasting>/volume_raycasting/shader
    )
