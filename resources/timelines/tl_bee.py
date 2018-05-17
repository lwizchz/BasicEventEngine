import bee

def create_bee_func(x):
    def f(name):
        print("{}: {}".format(name, x))

        #inst = bee.get_current_room().add_instance("obj_bee", x, 0.0, 0.0)
        #inst.set_computation_type(bee.E_COMPUTATION["SEMIPLAYER"])
        #inst.set_mass(1.0)
    return f

tl = bee.Timeline("tl_bee")

tl.add_action(0, "log_0", create_bee_func(100.0))
tl.add_action(60, "log_60", create_bee_func(300.0))
tl.add_action(120, "log_120", create_bee_func(500.0))
