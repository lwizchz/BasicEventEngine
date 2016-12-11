set(deps_bee debug.cpp resources.cpp util.cpp game.cpp enum.hpp)
#set(deps_bee_game game/resources.cpp game/room.cpp game/transition.cpp game/display.cpp game/window.cpp game/input.cpp game/draw.cpp game/messenger.cpp game/network.cpp)
set(deps_bee_resources resources/sprite.cpp resources/sound.cpp resources/background.cpp resources/font.cpp resources/path.cpp resources/timeline.cpp resources/mesh.cpp resources/object.cpp resources/room.cpp resources/light.cpp resources/ext/instancedata.cpp resources/ext/collisiontree.cpp resources/ext/particle.cpp resources/ext/soundgroup.cpp)
set(deps_bee_util util.hpp util/platform.hpp util/real.hpp util/string.hpp util/dates.hpp util/collision.hpp util/sound.hpp util/messagebox.hpp util/files.hpp util/network.hpp)
#set(deps ${deps_bee} ${deps_bee_game} ${deps_bee_resources} ${deps_bee_util})
set(deps ${deps_bee} ${deps_bee_resources} ${deps_bee_util})

if (WIN32)
	set(SRC ${${PROJECT_NAME}_SOURCE_DIR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow \"-L${SRC}/win/lib\" \"-I${SRC}/win/include\" \"-I${SRC}/lib/doctest/doctest\"")
endif()

add_library(BEE ${deps})
