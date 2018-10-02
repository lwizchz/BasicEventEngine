###############################################################################
# Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
#
# This file is part of BEE.
# BEE is free software and comes with ABSOLUTELY NO WARRANTY
# See LICENSE for more details.
###############################################################################

import bee

def create(self):
    self.set_computation_type(bee.E_COMPUTATION["SEMIPLAYER"])
    pb = self.get_physbody()
    pb.set_shape(bee.E_PHYS_SHAPE["BOX"], (100.0, 100.0, 100.0))
    pb.set_mass(0.0)
    pb.add_constraint(bee.E_PHYS_CONSTRAINT["FLAT"], ())

    print("u r a b {}".format(self.get_id()))

    ui_handle = bee.ui.create_handle(int(self.get_corner()[0]), int(self.get_corner()[1]), 100, 100, self)
    ui_handle.set_data("is_visible", False)
    self.set_data("ui_handle", ui_handle)

    if self == bee.Instance("obj_bee").at(0):
        bee.console.set_var("inst_bee_serialdata", self.serialize())

        bee.kb.bind("SDLK_UNKNOWN", {
            "name": "StartPath",
            "func": lambda e: (
                self.set_mass(1.0),
                bee.get_current_room().automate_path(self, {
                    "path": bee.Path("path_bee"),
                    "offset": (100.0, 100.0, 0.0),
                    "speed": 100,
                    "is_curved": True
                })
            )
        })
        bee.kb.bind("SDLK_UNKNOWN", {
            "name": "StartTimeline",
            "func": lambda e: (
                bee.get_current_room().automate_timeline({
                    "timeline": bee.Timeline("tl_bee"),
                    "start_offset": 0,
                    "is_looping": False,
                    "is_pausable": False
                })
            )
        })

        bee.kb.bind("SDLK_UNKNOWN", {
            "name": "StartSerialize",
            "func": lambda e: (
                bee.console.set_var("inst_bee_serialdata", self.serialize()),
                bee.messenger.send(["obj_bee"], bee.E_MESSAGE["INFO"], str(bee.console.get_var("inst_bee_serialdata")))
            )
        })
        bee.kb.bind("SDLK_UNKNOWN", {
            "name": "StartDeserialize",
            "func": lambda e: (
                self.deserialize(bee.console.get_var("inst_bee_serialdata"))
            )
        })

def destroy(self):
    if self == bee.Instance("obj_bee").at(0):
        bee.console.set_var("inst_bee_serialdata", None)

    bee.ui.destroy_parent(self)

def commandline_input(self, input):
    print("bee{}:~~~{}~~~".format(self.get_id(), input))
def path_end(self, pf):
    bee.messenger.log("path end")
    bee.get_current_room().automate_path(self, None)
def outside_room(self):
    bee.get_current_room().destroy(self)

def draw(self):
    import math
    size = 100
    r = math.degrees(self.get_physbody().get_rotation()[2])
    self.draw(size, size, r, bee.E_RGBA["WHITE"])

    paths = bee.get_current_room().get_paths()
    if self in paths:
        pf = paths[self]
        if pf["path"]:
            pf["path"].draw(pf)

    bee.Font("font_liberation").draw_fast(int(self.get_corner()[0]), int(self.get_corner()[1]), str(self.get_id()))

    l = bee.Light("lt_bee")
    c = l.get_color()
    l.set_position((self.get_pos()[0], self.get_pos()[1], 0.0, 1.0))
    l.set_color((self.get_id()*50, self.get_id()*20, c[2], c[3]))
    l.queue()
