set (CMAKE_CXX_STANDARD 11)

find_package( Boost 1.40 COMPONENTS program_options REQUIRED )

include_directories( inc ${Boost_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS}) 

FILE(GLOB MainSrc src/*.cpp)

add_executable( videoRebuilder ${MainSrc})
target_compile_features(videoRebuilder PRIVATE cxx_range_for)
target_link_libraries( videoRebuilder ${Boost_LIBRARIES} LibAvWrapper )

add_custom_command(TARGET videoRebuilder POST_BUILD COMMAND cp videoRebuilder ..)
