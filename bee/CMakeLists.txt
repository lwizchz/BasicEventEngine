set(deps_bee debug.cpp util.cpp engine.cpp enum.hpp)
set(deps_bee_util util/platform.cpp util/real.cpp util/string.cpp util/dates.cpp util/collision.cpp util/sound.cpp util/messagebox.cpp util/files.cpp util/network.cpp)

set(deps_bee_init init/info.cpp init/gameoptions.cpp init/programflags.cpp)

set(deps_bee_messenger messenger/messenger.cpp messenger/messagecontents.cpp messenger/messagerecipient.cpp)

set(deps_bee_core core/console.cpp core/display.cpp core/enginestate.cpp core/input.cpp core/instance.cpp core/resources.cpp core/room.cpp core/sidp.cpp core/window.cpp ${deps_bee_core_network})

set(deps_bee_network network/network.cpp network/networkclient.cpp network/networkdata.cpp)

set(deps_bee_render_particle render/particle/attractor.cpp render/particle/changer.cpp render/particle/deflector.cpp render/particle/destroyer.cpp render/particle/emitter.cpp render/particle/particle.cpp render/particle/particledata.cpp render/particle/system.cpp)
set(deps_bee_render render/camera.cpp render/drawing.cpp render/render.cpp render/renderer.cpp render/rgba.cpp render/transition.cpp render/viewdata.cpp ${deps_bee_render_particle})

set(deps_bee_physics physics/body.cpp physics/draw.cpp physics/world.cpp)

set(deps_bee_resource resource/sprite.cpp resource/sound.cpp resource/background.cpp resource/font.cpp resource/path.cpp resource/timeline.cpp resource/mesh.cpp resource/light.cpp resource/object.cpp resource/room.cpp)

set(deps_bee_ui ui/ui.cpp)

set(deps ${deps_bee} ${deps_bee_util} ${deps_bee_init} ${deps_bee_messenger} ${deps_bee_core} ${deps_bee_network} ${deps_bee_render} ${deps_bee_physics} ${deps_bee_resource} ${deps_bee_ui})

if (WIN32)
	set(SRC ${${PROJECT_NAME}_SOURCE_DIR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow \"-L${SRC}/win/lib\" \"-I${SRC}/win/include\" \"-I${SRC}/lib/doctest/doctest\"")
endif()

add_library(BEE ${deps})
