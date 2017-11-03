set(objects objects/obj_menu.cpp objects/obj_player.cpp)
set(rooms rooms/rm_menu.cpp rooms/rm_level_1.cpp)
set(deps resources.cpp ${objects} ${rooms})

if (WIN32)
	set(SRC ${${PROJECT_NAME}_SOURCE_DIR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \"-I${SRC}/win/include\" \"-I${SRC}/lib/doctest/doctest\" /Qpar /Fo$(IntDir)%(Directory)")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /MT")
endif()

add_library(Resources ${deps})
target_link_libraries(Resources BEE)
