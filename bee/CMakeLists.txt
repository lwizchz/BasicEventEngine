set(deps_bee engine.cpp enum.hpp)
set(deps_bee_util util/platform.cpp util/real.cpp util/string.cpp util/dates.cpp util/collision.cpp util/soundeffects.cpp util/messagebox.cpp util/files.cpp util/networking.cpp util/debug.cpp)

set(deps_bee_init init/info.cpp init/gameoptions.cpp init/programflags.cpp)

set(deps_bee_messenger messenger/messenger.cpp messenger/messagecontents.cpp messenger/messagerecipient.cpp)

set(deps_bee_core core/console.cpp core/display.cpp core/enginestate.cpp core/input.cpp core/instance.cpp core/keybind.cpp core/loader.cpp core/resources.cpp core/rooms.cpp core/window.cpp)
set(deps_bee_data data/sidp.cpp data/serialdata.cpp data/statemachine.cpp)

set(deps_bee_network network/network.cpp network/client.cpp network/connection.cpp network/packet.cpp network/data.cpp network/event.cpp)

set(deps_bee_render_particle render/particle/attractor.cpp render/particle/changer.cpp render/particle/deflector.cpp render/particle/destroyer.cpp render/particle/emitter.cpp render/particle/particle.cpp render/particle/particledata.cpp render/particle/system.cpp)
set(deps_bee_render render/camera.cpp render/drawing.cpp render/render.cpp render/renderer.cpp render/rgba.cpp render/shader.cpp render/transition.cpp render/viewport.cpp ${deps_bee_render_particle})

set(deps_bee_physics physics/body.cpp physics/filter.cpp physics/draw.cpp physics/world.cpp)

set(deps_bee_resource resource/texture.cpp resource/sound.cpp resource/font.cpp resource/path.cpp resource/timeline.cpp resource/mesh.cpp resource/light.cpp resource/script.cpp resource/object.cpp resource/room.cpp)

set(deps_bee_scripting python/python.cpp)

set(deps_bee_ui ui/ui.cpp ui/objects/element.cpp ui/objects/button.cpp ui/objects/handle.cpp ui/objects/text_entry.cpp ui/objects/gauge.cpp ui/objects/slider.cpp ui/objects/optionbox.cpp)

set(deps ${deps_bee} ${deps_bee_util} ${deps_bee_init} ${deps_bee_messenger} ${deps_bee_core} ${deps_bee_data} ${deps_bee_network} ${deps_bee_render} ${deps_bee_physics} ${deps_bee_resource} ${deps_bee_scripting} ${deps_bee_ui})

if (WIN32)
	set(SRC ${${PROJECT_NAME}_SOURCE_DIR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \"-I${SRC}/win/include\" \"-I${SRC}/lib/doctest/doctest\" /Qpar /Fo$(IntDir)%(Directory)")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /MT")
endif()

add_library(BEE ${deps})
