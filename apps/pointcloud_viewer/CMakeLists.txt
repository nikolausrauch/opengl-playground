add_executable( pointcloud_viewer
        ${CMAKE_CURRENT_SOURCE_DIR}/pointcloud_viewer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/billboard.geom
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/billboard.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/debug.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shader/blinn_phong.frag
        )

target_link_libraries( pointcloud_viewer PRIVATE viewer )

target_compile_definitions( pointcloud_viewer PUBLIC ${VIEWER_DEFINES} )
target_compile_features( pointcloud_viewer PUBLIC cxx_std_20 )
set_target_properties( pointcloud_viewer PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target(pointcloud_viewer_copy_shader ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shader
    $<TARGET_FILE_DIR:pointcloud_viewer>/pointcloud_viewer/shader
    )