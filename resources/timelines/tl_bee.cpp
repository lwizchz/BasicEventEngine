/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_TL_BEE
#define RES_TL_BEE 1

#include "../resources.hpp"

#include "tl_bee.hpp"

void tl_bee_load() {
    auto create_bee_func = [] (double x) {
        return [x] (bee::TimelineIterator* tlit, bee::TimelineAction* action) {
            bee::Instance* inst = bee::get_current_room()->add_instance(-1, obj_bee, x, 0.0, 0.0);
            inst->set_computation_type(bee::E_COMPUTATION::SEMIPLAYER);
            inst->set_mass(1.0);
        };
    };

    tl_bee->add_action(0, "log", create_bee_func(100.0));
    tl_bee->add_action(60, "log", create_bee_func(300.0));
    tl_bee->add_action(120, "log", create_bee_func(500.0));
}

#endif // RES_TL_BEE
