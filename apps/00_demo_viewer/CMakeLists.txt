add_executable( 00_demo_viewer ${CMAKE_CURRENT_SOURCE_DIR}/demo_viewer.cpp)
target_link_libraries( 00_demo_viewer PRIVATE viewer )

target_compile_definitions( 00_demo_viewer PUBLIC ${VIEWER_DEFINES} )
target_compile_features( 00_demo_viewer PUBLIC cxx_std_20 )
set_target_properties( 00_demo_viewer PROPERTIES CXX_EXTENSIONS OFF )



add_executable( 00_demo_triangle ${CMAKE_CURRENT_SOURCE_DIR}/demo_triangle.cpp)
target_link_libraries( 00_demo_triangle PRIVATE viewer )

target_compile_definitions( 00_demo_triangle PUBLIC ${VIEWER_DEFINES} )
target_compile_features( 00_demo_triangle PUBLIC cxx_std_20 )
set_target_properties( 00_demo_triangle PROPERTIES CXX_EXTENSIONS OFF )



add_executable( 00_demo_cube ${CMAKE_CURRENT_SOURCE_DIR}/demo_cube.cpp)
target_link_libraries( 00_demo_cube PRIVATE viewer )

target_compile_definitions( 00_demo_cube PUBLIC ${VIEWER_DEFINES} )
target_compile_features( 00_demo_cube PUBLIC cxx_std_20 )
set_target_properties( 00_demo_cube PROPERTIES CXX_EXTENSIONS OFF )



add_executable( 00_demo_model ${CMAKE_CURRENT_SOURCE_DIR}/demo_model.cpp)
target_link_libraries( 00_demo_model PRIVATE viewer )

target_compile_definitions( 00_demo_model PUBLIC ${VIEWER_DEFINES} )
target_compile_features( 00_demo_model PUBLIC cxx_std_20 )
set_target_properties( 00_demo_model PROPERTIES CXX_EXTENSIONS OFF )

