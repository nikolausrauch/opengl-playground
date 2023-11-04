add_subdirectory(00_demo_viewer)





#-------- copy all assets -------#
add_custom_target( copy_all_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets )
