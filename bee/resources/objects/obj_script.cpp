/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJ_SCRIPT
#define BEE_OBJ_SCRIPT 1

#include "../../util.hpp"
#include "../../all.hpp"

#include "obj_script.hpp"

namespace bee {
	namespace internal {
		const std::unordered_map<std::string, bee::E_EVENT> event_strings = {
			{"update", bee::E_EVENT::UPDATE},
			{"create", bee::E_EVENT::CREATE},
			{"destroy", bee::E_EVENT::DESTROY},
			{"alarm", bee::E_EVENT::ALARM},
			{"step_begin", bee::E_EVENT::STEP_BEGIN},
			{"step_mid", bee::E_EVENT::STEP_MID},
			{"step_end", bee::E_EVENT::STEP_END},
			{"keyboard_press", bee::E_EVENT::KEYBOARD_PRESS},
			{"mouse_press", bee::E_EVENT::MOUSE_PRESS},
			{"keyboard_input", bee::E_EVENT::KEYBOARD_INPUT},
			{"mouse_input", bee::E_EVENT::MOUSE_INPUT},
			{"keyboard_release", bee::E_EVENT::KEYBOARD_RELEASE},
			{"mouse_release", bee::E_EVENT::MOUSE_RELEASE},
			{"controller_axis", bee::E_EVENT::CONTROLLER_AXIS},
			{"controller_press", bee::E_EVENT::CONTROLLER_PRESS},
			{"controller_release", bee::E_EVENT::CONTROLLER_RELEASE},
			{"controller_modify", bee::E_EVENT::CONTROLLER_MODIFY},
			{"commandline_input", bee::E_EVENT::COMMANDLINE_INPUT},
			{"path_end", bee::E_EVENT::PATH_END},
			{"outside_room", bee::E_EVENT::OUTSIDE_ROOM},
			{"intersect_boundary", bee::E_EVENT::INTERSECT_BOUNDARY},
			{"collision", bee::E_EVENT::COLLISION},
			{"check_collision_filter", bee::E_EVENT::CHECK_COLLISION_FILTER},
			{"draw", bee::E_EVENT::DRAW},
			{"animation_end", bee::E_EVENT::ANIMATION_END},
			{"room_start", bee::E_EVENT::ROOM_START},
			{"room_end", bee::E_EVENT::ROOM_END},
			{"game_start", bee::E_EVENT::GAME_START},
			{"game_end", bee::E_EVENT::GAME_END},
			{"window", bee::E_EVENT::WINDOW},
			{"network", bee::E_EVENT::NETWORK}
		};
	}

	ObjScript::ObjScript(const std::string& _scriptfile) :
		Object("__obj_script", _scriptfile),

		scriptfile(_scriptfile),
		script(nullptr),
		events(),

		is_loaded(false)
	{
		set_name(Script::get_type(scriptfile).second);
	}
	ObjScript::~ObjScript() {
		this->free();
	}

	/**
	* Load the Object from its scriptfile.
	*
	* @retval 0 success
	* @retval 1 failed to load since it's already loaded
	* @retval 2 failed to load the Script
	*/
	int ObjScript::load() {
		if (is_loaded) { // If the Object has already been loaded, output a warning
			messenger::send({"engine", "object", "obj_script"}, E_MESSAGE::WARNING, "Failed to load Object \"" + scriptfile + "\" because it has already been loaded");
			return 1;
		}

		std::string filename (scriptfile);
		if ((!filename.empty())&&(filename.front() != '/')) {
			filename = "/resources/objects/"+filename;
		}

		script = Script::add("__scr_obj_script:" + scriptfile, filename);
		if ((script == nullptr)||(!script->get_is_loaded())) {
			return 2;
		}

		ScriptInterface* si = script->get_interface();
		for (auto& event : internal::event_strings) {
			if (si->has_var(event.first)) {
				events.insert(event.second);
				implemented_events.insert(event.second);
			}
		}

		is_loaded = true;

		return 0;
	}
	/**
	* Free the Object and its internal Script.
	*
	* @retval 0 success
	*/
	int ObjScript::free() {
		if (!is_loaded) {
			return 0;
		}

		events.clear();
		implemented_events.clear();

		if (script != nullptr) {
			delete script;
			script = nullptr;
		}

		is_loaded = false;

		return 0;
	}

	bool ObjScript::get_is_loaded() const {
		return is_loaded;
	}

	void ObjScript::update(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Object::update(self);
		if (events.find(bee::E_EVENT::UPDATE) == events.end()) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("update", args, nullptr);
	}
	void ObjScript::create(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("create", args, nullptr);
	}
	void ObjScript::destroy(Instance* self) {
		if (!is_loaded) {
			return;
		}

		if (events.find(bee::E_EVENT::DESTROY) != events.end()) {
			Variant args (std::vector<Variant>{
				Variant(self)
			});
			script->run_func("destroy", args, nullptr);
		}

		Object::destroy(self);
	}

	void ObjScript::alarm(Instance* self, const std::string& alarm) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(alarm)
		});
		script->run_func("alarm", args, nullptr);
	}

	void ObjScript::step_begin(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Object::step_begin(self);
		if (events.find(bee::E_EVENT::STEP_BEGIN) == events.end()) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("step_begin", args, nullptr);
	}
	void ObjScript::step_mid(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("step_mid", args, nullptr);
	}
	void ObjScript::step_end(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("step_end", args, nullptr);
	}

	void ObjScript::keyboard_press(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e->key.type))},
				{"timestamp", Variant(static_cast<int>(e->key.timestamp))},
				{"windowID", Variant(static_cast<int>(e->key.windowID))},
				{"state", Variant(static_cast<int>(e->key.state))},
				{"repeat", Variant(static_cast<int>(e->key.repeat))},
				{"keysym", Variant(std::map<Variant,Variant>{
					{"scancode", Variant(static_cast<int>(e->key.keysym.scancode))},
					{"sym", Variant(static_cast<int>(e->key.keysym.sym))},
					{"mod", Variant(static_cast<int>(e->key.keysym.mod))}
				})}
			})
		});
		script->run_func("keyboard_press", args, nullptr);
	}
	void ObjScript::mouse_press(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e->button.type))},
				{"timestamp", Variant(static_cast<int>(e->button.timestamp))},
				{"windowID", Variant(static_cast<int>(e->button.windowID))},
				{"which", Variant(static_cast<int>(e->button.which))},
				{"button", Variant(static_cast<int>(e->button.button))},
				{"state", Variant(static_cast<int>(e->button.state))},
				{"clicks", Variant(static_cast<int>(e->button.clicks))},
				{"x", Variant(static_cast<int>(e->button.x))},
				{"y", Variant(static_cast<int>(e->button.y))}
			})
		});
		script->run_func("mouse_press", args, nullptr);
	}
	void ObjScript::keyboard_input(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e->key.type))},
				{"timestamp", Variant(static_cast<int>(e->key.timestamp))},
				{"windowID", Variant(static_cast<int>(e->key.windowID))},
				{"state", Variant(static_cast<int>(e->key.state))},
				{"repeat", Variant(static_cast<int>(e->key.repeat))},
				{"keysym", Variant(std::map<Variant,Variant>{
					{"scancode", Variant(static_cast<int>(e->key.keysym.scancode))},
					{"sym", Variant(static_cast<int>(e->key.keysym.sym))},
					{"mod", Variant(static_cast<int>(e->key.keysym.mod))}
				})}
			})
		});
		script->run_func("keyboard_input", args, nullptr);
	}
	void ObjScript::mouse_input(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant event;
		if (e->type == SDL_MOUSEMOTION) {
			event = {
				{"type", Variant(static_cast<int>(e->motion.type))},
				{"timestamp", Variant(static_cast<int>(e->motion.timestamp))},
				{"windowID", Variant(static_cast<int>(e->motion.windowID))},
				{"which", Variant(static_cast<int>(e->motion.which))},
				{"state", Variant(static_cast<int>(e->motion.state))},
				{"x", Variant(static_cast<int>(e->motion.x))},
				{"y", Variant(static_cast<int>(e->motion.y))},
				{"xrel", Variant(static_cast<int>(e->motion.xrel))},
				{"yrel", Variant(static_cast<int>(e->motion.yrel))}
			};
		} else if (e->type == SDL_MOUSEWHEEL) {
			event = {
				{"type", Variant(static_cast<int>(e->wheel.type))},
				{"timestamp", Variant(static_cast<int>(e->wheel.timestamp))},
				{"windowID", Variant(static_cast<int>(e->wheel.windowID))},
				{"which", Variant(static_cast<int>(e->wheel.which))},
				{"x", Variant(static_cast<int>(e->wheel.x))},
				{"y", Variant(static_cast<int>(e->wheel.y))}
			#if SDL_VERSION_ATLEAST(2, 0, 4)
				, {"direction", Variant(static_cast<int>(e->wheel.direction))}
			#endif
			};
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			event
		});
		script->run_func("mouse_input", args, nullptr);
	}
	void ObjScript::keyboard_release(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e->key.type))},
				{"timestamp", Variant(static_cast<int>(e->key.timestamp))},
				{"windowID", Variant(static_cast<int>(e->key.windowID))},
				{"state", Variant(static_cast<int>(e->key.state))},
				{"repeat", Variant(static_cast<int>(e->key.repeat))},
				{"keysym", Variant(std::map<Variant,Variant>{
					{"scancode", Variant(static_cast<int>(e->key.keysym.scancode))},
					{"sym", Variant(static_cast<int>(e->key.keysym.sym))},
					{"mod", Variant(static_cast<int>(e->key.keysym.mod))}
				})}
			})
		});
		script->run_func("keyboard_release", args, nullptr);
	}
	void ObjScript::mouse_release(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e->button.type))},
				{"timestamp", Variant(static_cast<int>(e->button.timestamp))},
				{"windowID", Variant(static_cast<int>(e->button.windowID))},
				{"which", Variant(static_cast<int>(e->button.which))},
				{"button", Variant(static_cast<int>(e->button.button))},
				{"state", Variant(static_cast<int>(e->button.state))},
				{"clicks", Variant(static_cast<int>(e->button.clicks))},
				{"x", Variant(static_cast<int>(e->button.x))},
				{"y", Variant(static_cast<int>(e->button.y))}
			})
		});
		script->run_func("mouse_release", args, nullptr);
	}
	void ObjScript::controller_axis(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e->caxis.type))},
				{"timestamp", Variant(static_cast<int>(e->caxis.timestamp))},
				{"which", Variant(static_cast<int>(e->caxis.which))},
				{"axis", Variant(static_cast<int>(e->caxis.axis))},
				{"value", Variant(static_cast<int>(e->caxis.value))}
			})
		});
		script->run_func("controller_axis", args, nullptr);
	}
	void ObjScript::controller_press(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e->cbutton.type))},
				{"timestamp", Variant(static_cast<int>(e->cbutton.timestamp))},
				{"which", Variant(static_cast<int>(e->cbutton.which))},
				{"button", Variant(static_cast<int>(e->cbutton.button))},
				{"state", Variant(static_cast<int>(e->cbutton.state))}
			})
		});
		script->run_func("controller_press", args, nullptr);
	}
	void ObjScript::controller_release(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e->cbutton.type))},
				{"timestamp", Variant(static_cast<int>(e->cbutton.timestamp))},
				{"which", Variant(static_cast<int>(e->cbutton.which))},
				{"button", Variant(static_cast<int>(e->cbutton.button))},
				{"state", Variant(static_cast<int>(e->cbutton.state))}
			})
		});
		script->run_func("controller_release", args, nullptr);
	}
	void ObjScript::controller_modify(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e->cdevice.type))},
				{"timestamp", Variant(static_cast<int>(e->cdevice.timestamp))},
				{"which", Variant(static_cast<int>(e->cdevice.which))}
			})
		});
		script->run_func("controller_modify", args, nullptr);
	}

	void ObjScript::commandline_input(Instance* self, const std::string& str) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(str)
		});
		script->run_func("commandline_input", args, nullptr);
	}

	void ObjScript::path_end(Instance* self, const PathFollower& pf) {
		if (!is_loaded) {
			return;
		}

		PathFollower* _pf = const_cast<PathFollower*>(&pf);

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(_pf)
		});
		script->run_func("path_end", args, nullptr);
	}
	void ObjScript::outside_room(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("outside_room", args, nullptr);
	}
	void ObjScript::intersect_boundary(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("intersect_boundary", args, nullptr);
	}
	void ObjScript::collision(Instance* self, Instance* other) {
		if (!is_loaded) {
			return;
		}

		if (events.find(bee::E_EVENT::COLLISION) == events.end()) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(other)
		});
		script->run_func("collision", args, nullptr);
	}
	bool ObjScript::check_collision_filter(const Instance* self, const Instance* other) const {
		if (!is_loaded) {
			return Object::check_collision_filter(self, other);
		}

		if (events.find(bee::E_EVENT::CHECK_COLLISION_FILTER) == events.end()) {
			return Object::check_collision_filter(self, other);
		}

		Variant result;

		Instance* _self = const_cast<Instance*>(self);
		Instance* _other = const_cast<Instance*>(other);

		Variant args (std::vector<Variant>{
			Variant(_self),
			Variant(_other)
		});
		script->run_func("check_collision_filter", args, &result);

		return result.i;
	}

	void ObjScript::draw(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("draw", args, nullptr);
	}
	void ObjScript::animation_end(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("animation_end", args, nullptr);
	}

	void ObjScript::room_start(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("room_start", args, nullptr);
	}
	void ObjScript::room_end(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("room_end", args, nullptr);
	}
	void ObjScript::game_start(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("game_start", args, nullptr);
	}
	void ObjScript::game_end(Instance* self) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self)
		});
		script->run_func("game_end", args, nullptr);
	}

	void ObjScript::window(Instance* self, SDL_Event* e) {
		if (!is_loaded) {
			return;
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e->window.type))},
				{"timestamp", Variant(static_cast<int>(e->window.timestamp))},
				{"windowID", Variant(static_cast<int>(e->window.windowID))},
				{"event", Variant(static_cast<int>(e->window.event))},
				{"data1", Variant(static_cast<int>(e->window.data1))},
				{"data2", Variant(static_cast<int>(e->window.data2))}
			})
		});
		script->run_func("window", args, nullptr);
	}

	void ObjScript::network(Instance* self, const NetworkEvent& e) {
		if (!is_loaded) {
			return;
		}

		std::map<Variant,Variant> data;
		for (auto& d : e.data) {
			data.emplace(Variant(d.first), d.second);
		}

		std::map<Variant,Variant> insts;
		for (auto& inst : e.instances) {
			std::vector<Variant> _data;
			_data.reserve(inst.second.size());
			for (auto& d : inst.second) {
				_data.push_back(Variant(static_cast<int>(d)));
			}

			insts.emplace(Variant(inst.first), _data);
		}

		Variant args (std::vector<Variant>{
			Variant(self),
			Variant(std::map<Variant,Variant>{
				{"type", Variant(static_cast<int>(e.type))},
				{"id", Variant(e.id)},
				{"data", Variant(data)},
				{"instances", Variant(insts)},
			})
		});
		script->run_func("network", args, nullptr);
	}
}

#endif // BEE_OBJ_SCRIPT
