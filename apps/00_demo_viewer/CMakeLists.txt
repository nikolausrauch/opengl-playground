add_executable( 00_demo_viewer ${CMAKE_CURRENT_SOURCE_DIR}/demo_viewer.cpp)
target_link_libraries( 00_demo_viewer PRIVATE viewer )

target_compile_definitions( 00_demo_viewer PUBLIC ${VIEWER_DEFINES} )
target_compile_features( 00_demo_viewer PUBLIC cxx_std_20 )
set_target_properties( 00_demo_viewer PROPERTIES CXX_EXTENSIONS OFF )
