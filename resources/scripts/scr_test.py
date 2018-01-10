#!/bin/python

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
