###############################################################################
# Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
#
# This file is part of BEE.
# BEE is free software and comes with ABSOLUTELY NO WARRANTY
# See LICENSE for more details.
###############################################################################

import bee

def init(self):
    # Set Room properties
    self.set_width(1920)
    self.set_height(1080)

    # Setup physics
    pw = self.get_phys_world()
    pw.set_gravity((0.0, 10.0, 0.0))
    pw.set_scale(100.0)

    # Set Backgrounds
    self.add_background("bk_green", {
        "texture": "bk_green",
        "is_visible": True,
        "is_foreground": False,
        "transform": {
            "x": 0,
            "y": 0,
            "is_horizontal_tile": True,
            "is_vertical_tile": True,
            "horizontal_speed": 10,
            "vertical_speed": 10,
            "is_stretched": False,
        }
    })

    # Setup Instances
    self.add_instance(bee.Object("obj_control"), (0.0, 0.0, 0.0))
    self.add_instance(bee.Object("obj_bee"), (300.0, 300.0, 0.0))
    self.add_instance(bee.Object("obj_bee"), (800.0, 300.0, 0.0))
    self.add_instance(bee.Object("obj_bee"), (300.0, 600.0, 0.0))
    self.add_instance(bee.Object("obj_bee"), (800.0, 600.0, 0.0))

    # Define an easy queue func to handle any potentially invalid Resources
    def queue(r, f=None):
        try:
            bee.loader.queue(r, f)
        except (ValueError, TypeError) as err:
            import traceback
            print("Traceback (most recent call last):")
            traceback.print_stack()
            print("{}: {}".format(type(err).__name__, err))
            print()

    # Load the required Resources for this Room
    queue(bee.Texture("spr_bee"))
    queue(bee.Texture("bk_green"))
    queue(bee.Sound("snd_chirp"))
    queue(bee.Font("font_liberation"))
    queue(bee.Path("path_bee"))
    queue(bee.Timeline("tl_bee"))
    queue(bee.Mesh("mesh_monkey"))
    queue(bee.Mesh("mesh_spider"), lambda res: res.load(2))
    queue(bee.Light("lt_ambient"))
    queue(bee.Light("lt_bee"))
    queue(bee.Script("scr_test"))
    queue(bee.Object("obj_mesh"))

    bee.loader.load()
