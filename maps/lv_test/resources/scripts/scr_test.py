###############################################################################
# Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
#
# This file is part of BEE.
# BEE is free software and comes with ABSOLUTELY NO WARRANTY
# See LICENSE for more details.
###############################################################################

import bee

import random

def step():
    bee.set_window_pos(random.randint(0, 20), random.randint(0, 20))

def main():
    def log(tickstamp, tags, type, descr):
        print("{}: {}".format(type, descr))
    bee.messenger.register_recipient("pylog", ["python", "log"], True, log)

    bee.messenger.log("python logging...")
    bee.messenger.send(["python", "log"], bee.E_MESSAGE["WARNING"], "warning!")
    bee.messenger.send(["python", "log"], bee.E_MESSAGE["INTERNAL"], "internal python message")

    def pyflag(arg):
        import sys
        bee.messenger.log("python version: {0.major}.{0.minor}".format(sys.version_info))
    bee.add_flag("python", "p", bee.E_FLAGARG["NONE"], pyflag)

    def pyopt(value):
        print("changing pyopt to: {}".format(value))
    bee.set_option("pyopt", "a string value", pyopt)

    inst = bee.Instance("obj_bee").at(0)
    inst.set_data("py", [1, 2, 3])
    print(inst.get_data("py"))
