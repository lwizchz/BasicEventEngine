###############################################################################
# Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
#
# This file is part of BEE.
# BEE is free software and comes with ABSOLUTELY NO WARRANTY
# See LICENSE for more details.
###############################################################################

import bee

import math

def create(self):
    self.set_data("mesh", bee.Mesh("mesh_spider"))

    bee.console.set_var("anim_spider", "Spider_Armature|warte_pose")

def draw(self):
    t = bee.get_ticks()/1000.0 + self.get_pos()[0]
    a = 180.0 + math.degrees(math.sin(t))
    pos = (1000.0+500.0*math.cos(t), 500.0+300.0*math.sin(t), 0.0)
    rot = (0.0, a, 180.0)

    mesh = self.get_data("mesh")
    if mesh.name == "mesh_monkey": # Monkey
        #mesh.draw(pos, (1.0, 1.0, 1.0), rot, (255, 255, 0, 255), False)
        pass
    elif mesh.name == "mesh_spider": # Spider
        mesh.draw(pos, (50.0, 50.0, 50.0), rot, (255, 255, 255, 255), False)
        #mesh.draw(pos, (50.0, 50.0, 50.0), rot, (255, 255, 255, 255), False, bee.console.get_var("anim_spider"), 0)
