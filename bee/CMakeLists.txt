set(deps_bee debug.cpp resource_structures.cpp util.cpp game.cpp)
set(deps_bee_game game/resources.cpp game/room.cpp game/transition.cpp game/display.cpp game/window.cpp game/input.cpp game/draw.cpp game/network.cpp)
set(deps_bee_resource_structures resource_structures/sprite.cpp resource_structures/sound.cpp resource_structures/background.cpp resource_structures/font.cpp resource_structures/path.cpp resource_structures/timeline.cpp resource_structures/object.cpp resource_structures/room.cpp resource_structures/ext/instancedata.cpp resource_structures/ext/collisiontree.cpp resource_structures/ext/particle.cpp resource_structures/ext/soundgroup.cpp)
set(deps ${deps_bee} ${deps_bee_game} ${deps_bee_resource_structures})

add_library(BEE ${deps})
