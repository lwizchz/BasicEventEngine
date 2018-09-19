###############################################################################
# Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
#
# This file is part of BEE.
# BEE is free software and comes with ABSOLUTELY NO WARRANTY
# See LICENSE for more details.
###############################################################################

import bee

def create(self):
    self.set_data("camx", 1920.0/2.0);
    self.set_data("camy", 1080.0/2.0);
    self.set_data("camz", 0.0);
    self.set_data("camspeed", 2.0);

    bee.mouse.set_pos(1920//2, 1080//2);

    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "RoomRestart",
        "func": lambda e: (
            bee.render.set_transition_type(bee.render.get_transition_type()+1),
            bee.restart_room()
        )
    })

    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "MoveForward",
        "is_repeatable": True,
        "func": lambda e: (
            self.set_data("camz", self.get_data("camz") + self.get_data("camspeed"))
        )
    })
    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "MoveBackward",
        "is_repeatable": True,
        "func": lambda e: (
            self.set_data("camz", self.get_data("camz") - self.get_data("camspeed"))
        )
    })
    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "MoveLeft",
        "is_repeatable": True,
        "func": lambda e: (
            self.set_data("camx", self.get_data("camx") - self.get_data("camspeed"))
        )
    })
    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "MoveRight",
        "is_repeatable": True,
        "func": lambda e: (
            self.set_data("camx", self.get_data("camx") + self.get_data("camspeed"))
        )
    })
    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "MoveDown",
        "is_repeatable": True,
        "func": lambda e: (
            self.set_data("camy", self.get_data("camy") + self.get_data("camspeed"))
        )
    })
    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "MoveUp",
        "is_repeatable": True,
        "func": lambda e: (
            self.set_data("camy", self.get_data("camy") - self.get_data("camspeed"))
        )
    })

    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "NetworkStartSession",
        "func": lambda e: (
            bee.net.session_start("test_session", 4, "hostplayer")
        )
    })
    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "NetworkJoinSession",
        "func": lambda e: (
            bee.net.session_join("127.0.0.1", "clientplayer")
        )
    })
    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "NetworkFindSessions",
        "func": lambda e: (
            print("Available servers: {}".format(bee.net.session_find()))
        )
    })

    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "StartSound",
        "func": lambda e: (
            bee.Sound("snd_chirp").stop(),
            bee.Sound("snd_chirp").effect_remove_all(),
            bee.Sound("snd_chirp").play()
        )
    })
    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "StartSoundEcho",
        "func": lambda e: (
            bee.Sound("snd_chirp").stop(),
            bee.Sound("snd_chirp").effect_remove_all(),
            bee.Sound("snd_chirp").effect_add(
                "echo",
                bee.E_SOUNDEFFECT["ECHO"],
                {}
            ),
            bee.Sound("snd_chirp").play()
        )
    })

    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "Start3D",
        "func": lambda e: (
            bee.render.set_3d(True)
        )
    })
    bee.kb.bind("SDLK_UNKNOWN", {
        "name": "End3D",
        "func": lambda e: (
            bee.render.set_3d(False)
        )
    })
def destroy(self):
    if bee.net.get_is_connected():
        bee.net.session_end()

def step_mid(self):
    if bee.render.get_3d():
        mpos = bee.mouse.get_pos()
        bee.render.set_camera((
            (self.get_data("camx"), self.get_data("camy"), -540.0 + self.get_data("camz")),
            ((-1920.0/2.0 + mpos[0])/1920.0*2.0, (-1080.0/2.0 + mpos[1])/1080.0*2.0, 1.0),
            (0.0, -1.0, 0.0)
        ))
def mouse_press(self, e):
    if bee.console.get_is_open():
        return

    if e["button"] == bee.E_SDL_BUTTON["LEFT"]:
        if self.is_place_empty(e["x"], e["y"]):
            inst_bee = bee.get_current_room().add_instance(bee.Object("obj_bee"), (e["x"], e["y"], 0.0))
            inst_bee.set_computation_type(bee.E_COMPUTATION["SEMIPLAYER"])
            inst_bee.set_mass(1.0)

def draw(self):
    bee.Light("lt_ambient").queue()

    bee.Font("font_liberation").draw_fast(0, 0, "FPS: {}".format(bee.get_fps_stable()))

def room_start(self):
    bee.get_current_room().add_instance(bee.Object("obj_mesh"), (0.0, 0.0, 0.0))
def game_start(self):
    bee.console.run("execfile(\"cfg/config.py\")") # Configure default binds

    bee.Script("scr_test").run_func("main", ())
