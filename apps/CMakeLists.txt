add_subdirectory(00_demo_viewer)

add_subdirectory(pointcloud_viewer)
add_subdirectory(shadow_mapping)
add_subdirectory(cel_shading)
add_subdirectory(volume_raycasting)
add_subdirectory(deferred_rendering)
add_subdirectory(reflection_probes)


#-------- copy all assets -------#
add_custom_target( copy_all_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets )
