set(timelines timelines/tl_bee.cpp)
set(objects objects/obj_control.cpp objects/obj_bee.cpp)
set(rooms rooms/rm_test.cpp)
set(deps resources.cpp ${timelines} ${objects} ${rooms})

if (WIN32)
	set(SRC ${${PROJECT_NAME}_SOURCE_DIR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \"-I${SRC}/win/include\" \"-I${SRC}/lib/doctest/doctest\" /Qpar /Fo$(IntDir)%(Directory)")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /MT")
endif()

add_library(Resources ${deps})
target_link_libraries(Resources BEE)
