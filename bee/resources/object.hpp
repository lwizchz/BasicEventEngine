/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_OBJECT_H
#define _BEE_OBJECT_H 1

#include <string>
#include <map>

#include "../engine.hpp"

namespace bee {
	//template <typename ObjType>
	class Object: public Resource {
			// Add new variables to the print() debugging method
			int id = -1;
			std::string name;
			std::string path;
			Sprite* sprite;
			bool is_solid, is_visible, is_persistent;
			int depth;
			Object* parent;
			Sprite* mask;
			int xoffset, yoffset;

			std::map<int,Instance*> instances;
		protected:
			std::map<int,std::map<std::string,SIDP>> instance_data;
			std::map<std::string,SIDP>* s;

			Object();
			Object(const std::string&, const std::string&);
		public:
			virtual ~Object();
			int add_to_resources();
			int reset();
			int print() const;

			int get_id() const;
			std::string get_name() const;
			std::string get_path() const;
			Sprite* get_sprite() const;
			int get_sprite_id() const;
			bool get_is_solid() const;
			bool get_is_visible() const;
			bool get_is_persistent() const;
			int get_depth() const;
			Object* get_parent() const;
			int get_parent_id() const;
			Sprite* get_mask() const;
			int get_mask_id() const;
			std::pair<int,int> get_mask_offset() const;

			int set_name(const std::string&);
			int set_path(const std::string&);
			int set_sprite(Sprite*);
			int set_sprite_id(int);
			int set_is_solid(bool);
			int set_is_visible(bool);
			int set_is_persistent(bool);
			int set_depth(int);
			int set_parent(Object*);
			int set_parent_id(int);
			int set_mask(Sprite*);
			int set_mask_id(int);
			int set_mask_offset(std::pair<int,int>);
			int set_mask_offset(int, int);

			int add_instance(int, Instance*);
			int remove_instance(int);
			int clear_instances();
			std::map<int, Instance*> get_instances() const;
			size_t get_instance_amount() const;
			Instance* get_instance(int) const;
			std::string get_instance_string() const;

			SIDP get_data(int, const std::string&) const;
			int set_data(int, const std::string&, SIDP);

			std::map<E_EVENT,bool> implemented_events;
			virtual void update(Instance*) =0;
			virtual void create(Instance*) {};
			virtual void destroy(Instance*) {};
			virtual void alarm(Instance*, size_t) {};
			virtual void step_begin(Instance*) {};
			virtual void step_mid(Instance*) {};
			virtual void step_end(Instance*) {};
			virtual void keyboard_press(Instance*, SDL_Event*) {};
			virtual void mouse_press(Instance*, SDL_Event*) {};
			virtual void keyboard_input(Instance*, SDL_Event*) {};
			virtual void mouse_input(Instance*, SDL_Event*) {};
			virtual void keyboard_release(Instance*, SDL_Event*) {};
			virtual void mouse_release(Instance*, SDL_Event*) {};
			virtual void controller_axis(Instance*, SDL_Event*) {};
			virtual void controller_press(Instance*, SDL_Event*) {};
			virtual void controller_release(Instance*, SDL_Event*) {};
			virtual void controller_modify(Instance*, SDL_Event*) {};
			virtual void commandline_input(Instance*, const std::string&) {};
			virtual void path_end(Instance*) {};
			virtual void outside_room(Instance*) {};
			virtual void intersect_boundary(Instance*) {};
			virtual void collision(Instance*, Instance*) {};
			virtual bool check_collision_list(const Instance&, const Instance&) const {return true;};
			virtual void draw(Instance*) {};
			virtual void animation_end(Instance*) {};
			virtual void room_start(Instance*) {};
			virtual void room_end(Instance*) {};
			virtual void game_start(Instance*) {};
			virtual void game_end(Instance*) {};
			virtual void window(Instance*, SDL_Event*) {};

			/*void update(Instance*);
			void create(Instance*);
			void destroy(Instance*);
			void alarm(Instance*, size_t);
			void step_begin(Instance*);
			void step_mid(Instance*);
			void step_end(Instance*);
			void keyboard_press(Instance*, SDL_Event*);
			void mouse_press(Instance*, SDL_Event*);
			void keyboard_input(Instance*, SDL_Event*);
			void mouse_input(Instance*, SDL_Event*);
			void keyboard_release(Instance*, SDL_Event*);
			void mouse_release(Instance*, SDL_Event*);
			void controller_axis(Instance*, SDL_Event*);
			void controller_press(Instance*, SDL_Event*);
			void controller_release(Instance*, SDL_Event*);
			void controller_modify(Instance*, SDL_Event*);
			void commandline_input(Instance*, const std::string&);
			void path_end(Instance*);
			void outside_room(Instance*);
			void intersect_boundary(Instance*);
			void collision(Instance*, Instance*);
			bool check_collision_list(const Instance&, const Instance&) const;
			void draw(Instance*);
			void animation_end(Instance*);
			void room_start(Instance*);
			void room_end(Instance*);
			void game_start(Instance*);
			void game_end(Instance*);
			void window(Instance*, SDL_Event*);*/

			/*void update(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->update(self);
			}
			void create(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->create(self);
			}
			void destroy(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->destroy(self);
			}
			void alarm(Instance* self, size_t a) {
				reinterpret_cast<const ObjType*>(this)->alarm(a);
			}
			void step_begin(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->step_begin(self);
			}
			void step_mid(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->step_mid(self);
			}
			void step_end(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->step_end(self);
			}
			void keyboard_press(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->keyboard_press(self, e);
			}
			void mouse_press(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->mouse_press(self, e);
			}
			void keyboard_input(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->keyboard_input(self, e);
			}
			void mouse_input(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->mouse_input(self, e);
			}
			void keyboard_release(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->keyboard_release(self, e);
			}
			void mouse_release(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->mouse_release(self, e);
			}
			void controller_axis(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->controller_axis(self, e);
			}
			void controller_press(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->controller_press(self, e);
			}
			void controller_release(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->controller_release(self, e);
			}
			void controller_modify(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->controller_modify(self, e);
			}
			void commandline_input(Instance* self, const std::string& str) {
				reinterpret_cast<const ObjType*>(this)->commandline_input(self, str);
			}
			void path_end(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->path_end(self);
			}
			void outside_room(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->outside_room(self);
			}
			void intersect_boundary(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->intersect_boundary(self);
			}
			void collision(Instance* self, Instance* other) {
				reinterpret_cast<const ObjType*>(this)->collision(self, other);
			}
			bool check_collision_list(const Instance& self, const Instance& other) const {
				reinterpret_cast<const ObjType*>(this)->check_collision_list(self, other);
			}
			void draw(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->draw(self);
			}
			void animation_end(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->animation_end(self);
			}
			void room_start(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->room_start(self);
			}
			void room_end(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->room_end(self);
			}
			void game_start(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->game_start(self);
			}
			void game_end(Instance* self) {
				reinterpret_cast<const ObjType*>(this)->game_end(self);
			}
			void window(Instance* self, SDL_Event* e) {
				reinterpret_cast<const ObjType*>(this)->window(self, e);
			}*/
	};

	/*
	void Object::update(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->update(self);
	}
	void Object::create(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->create(self);
	}
	void Object::destroy(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->destroy(self);
	}
	void Object::alarm(Instance* self, size_t a) {
		reinterpret_cast<const ObjType*>(this)->alarm(a);
	}
	void Object::step_begin(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->step_begin(self);
	}
	void Object::step_mid(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->step_mid(self);
	}
	void Object::step_end(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->step_end(self);
	}
	void Object::keyboard_press(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->keyboard_press(self, e);
	}
	void Object::mouse_press(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->mouse_press(self, e);
	}
	void Object::keyboard_input(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->keyboard_input(self, e);
	}
	void Object::mouse_input(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->mouse_input(self, e);
	}
	void Object::keyboard_release(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->keyboard_release(self, e);
	}
	void Object::mouse_release(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->mouse_release(self, e);
	}
	void Object::controller_axis(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->controller_axis(self, e);
	}
	void Object::controller_press(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->controller_press(self, e);
	}
	void Object::controller_release(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->controller_release(self, e);
	}
	void Object::controller_modify(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->controller_modify(self, e);
	}
	void Object::commandline_input(Instance* self, const std::string& str) {
		reinterpret_cast<const ObjType*>(this)->commandline_input(self, str);
	}
	void Object::path_end(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->path_end(self);
	}
	void Object::outside_room(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->outside_room(self);
	}
	void Object::intersect_boundary(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->intersect_boundary(self);
	}
	void Object::collision(Instance* self, Instance* other) {
		reinterpret_cast<const ObjType*>(this)->collision(self, other);
	}
	bool Object::check_collision_list(const Instance& self, const Instance& other) const {
		reinterpret_cast<const ObjType*>(this)->check_collision_list(self, other);
	}
	void Object::draw(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->draw(self);
	}
	void Object::animation_end(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->animation_end(self);
	}
	void Object::room_start(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->room_start(self);
	}
	void Object::room_end(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->room_end(self);
	}
	void Object::game_start(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->game_start(self);
	}
	void Object::game_end(Instance* self) {
		reinterpret_cast<const ObjType*>(this)->game_end(self);
	}
	void Object::window(Instance* self, SDL_Event* e) {
		reinterpret_cast<const ObjType*>(this)->window(self, e);
	}*/
}

#endif // _BEE_OBJECT_H
