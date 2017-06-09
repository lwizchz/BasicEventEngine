/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#include "engine.hpp"

#include "init/gameoptions.hpp"
#include "init/programflags.hpp"

#include "core/console.hpp"
#include "core/enginestate.hpp"
#include "core/instance.hpp"
#include "core/sidp.hpp"
#include "core/messenger/messagecontents.hpp"
#include "core/messenger/messagerecipient.hpp"
#include "core/network/networkdata.hpp"
#include "core/network/networkclient.hpp"

#include "render/camera.hpp"
#include "render/renderer.hpp"
#include "render/rgba.hpp"
#include "render/viewdata.hpp"
#include "render/particle/particle.hpp"
#include "render/particle/particledata.hpp"
#include "render/particle/emitter.hpp"
#include "render/particle/attractor.hpp"
#include "render/particle/destroyer.hpp"
#include "render/particle/deflector.hpp"
#include "render/particle/changer.hpp"
#include "render/particle/system.hpp"

#include "physics/world.hpp"
#include "physics/draw.hpp"
#include "physics/body.hpp"

#include "resources/sprite.hpp"
#include "resources/sound.hpp"
#include "resources/background.hpp"
#include "resources/font.hpp"
#include "resources/path.hpp"
#include "resources/timeline.hpp"
#include "resources/mesh.hpp"
#include "resources/light.hpp"
#include "resources/object.hpp"
#include "resources/room.hpp"
