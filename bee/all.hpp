/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#include <iostream>

#include "engine.hpp"

#include "init/gameoptions.hpp"
#include "init/info.hpp"
#include "init/programflags.hpp"

#include "messenger/messenger.hpp"
#include "messenger/messagecontents.hpp"
#include "messenger/messagerecipient.hpp"

#include "core/console.hpp"
#include "core/display.hpp"
#include "core/enginestate.hpp"
#include "core/instance.hpp"
#include "core/loader.hpp"
#include "core/rooms.hpp"
#include "core/window.hpp"

#include "input/mouse.hpp"
#include "input/kb.hpp"
#include "input/keybind.hpp"

#include "data/serialdata.hpp"
#include "data/statemachine.hpp"
#include "data/variant.hpp"

#include "network/network.hpp"
#include "network/connection.hpp"
#include "network/client.hpp"
#include "network/packet.hpp"
#include "network/event.hpp"

#include "render/background.hpp"
#include "render/camera.hpp"
#include "render/drawing.hpp"
#include "render/render.hpp"
#include "render/renderer.hpp"
#include "render/rgba.hpp"
#include "render/shader.hpp"
#include "render/transition.hpp"
#include "render/viewport.hpp"

#include "physics/world.hpp"
#include "physics/draw.hpp"
#include "physics/body.hpp"

#include "resource/resource.hpp"
#include "resource/texture.hpp"
#include "resource/sound.hpp"
#include "resource/font.hpp"
#include "resource/path.hpp"
#include "resource/timeline.hpp"
#include "resource/mesh.hpp"
#include "resource/light.hpp"
#include "resource/script.hpp"
#include "resource/object.hpp"
#include "resource/room.hpp"

#include "python/python.hpp"

#include "ui/ui.hpp"

#include "resources/headers.hpp"

// Does not include the util headers, use ./util.hpp
