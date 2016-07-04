set(deps_bee debug.cpp resource_structures.cpp util.cpp game.cpp enum.hpp)
set(deps_bee_game game/resources.cpp game/room.cpp game/transition.cpp game/display.cpp game/window.cpp game/input.cpp game/draw.cpp game/network.cpp)
set(deps_bee_resource_structures resource_structures/sprite.cpp resource_structures/sound.cpp resource_structures/background.cpp resource_structures/font.cpp resource_structures/path.cpp resource_structures/timeline.cpp resource_structures/object.cpp resource_structures/room.cpp resource_structures/light.cpp resource_structures/ext/instancedata.cpp resource_structures/ext/collisiontree.cpp resource_structures/ext/particle.cpp resource_structures/ext/soundgroup.cpp)
set(deps_bee_util util.hpp util/platform.hpp util/real.hpp util/string.hpp util/dates.hpp util/collision.hpp util/sound.hpp util/messagebox.hpp util/files.hpp util/network.hpp)
set(deps ${deps_bee} ${deps_bee_game} ${deps_bee_resource_structures} ${deps_bee_util})

if (WIN32)
	set(SRC ${${PROJECT_NAME}_SOURCE_DIR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow \"-L${SRC}/win/lib\" \"-I${SRC}/win/include\" \"-I${SRC}/lib/doctest/doctest\"")
endif()

add_library(BEE ${deps})
