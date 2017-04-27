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

#include "../game.hpp"

//template <typename ObjType>
class BEE::Object: public Resource {
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

		std::map<int,InstanceData*> instances;
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

		int add_instance(int, InstanceData*);
		int remove_instance(int);
		int clear_instances();
		std::map<int, InstanceData*> get_instances() const;
		size_t get_instance_amount() const;
		InstanceData* get_instance(int) const;
		std::string get_instance_string() const;

		SIDP get_data(int, const std::string&) const;
		int set_data(int, const std::string&, SIDP);

		std::map<bee_event_t,bool> implemented_events;
		virtual void update(InstanceData*) =0;
		virtual void create(InstanceData*) {};
		virtual void destroy(InstanceData*) {};
		virtual void alarm(InstanceData*, int) {};
		virtual void step_begin(InstanceData*) {};
		virtual void step_mid(InstanceData*) {};
		virtual void step_end(InstanceData*) {};
		virtual void keyboard_press(InstanceData*, SDL_Event*) {};
		virtual void mouse_press(InstanceData*, SDL_Event*) {};
		virtual void keyboard_input(InstanceData*, SDL_Event*) {};
		virtual void mouse_input(InstanceData*, SDL_Event*) {};
		virtual void keyboard_release(InstanceData*, SDL_Event*) {};
		virtual void mouse_release(InstanceData*, SDL_Event*) {};
		virtual void controller_axis(InstanceData*, SDL_Event*) {};
		virtual void controller_press(InstanceData*, SDL_Event*) {};
		virtual void controller_release(InstanceData*, SDL_Event*) {};
		virtual void controller_modify(InstanceData*, SDL_Event*) {};
		virtual void commandline_input(InstanceData*, const std::string&) {};
		virtual void path_end(InstanceData*) {};
		virtual void outside_room(InstanceData*) {};
		virtual void intersect_boundary(InstanceData*) {};
		virtual void collision(InstanceData*, InstanceData*) {};
		virtual bool check_collision_list(const InstanceData&, const InstanceData&) const {return true;};
		virtual void draw(InstanceData*) {};
		virtual void animation_end(InstanceData*) {};
		virtual void room_start(InstanceData*) {};
		virtual void room_end(InstanceData*) {};
		virtual void game_start(InstanceData*) {};
		virtual void game_end(InstanceData*) {};
		virtual void window(InstanceData*, SDL_Event*) {};

		/*void update(InstanceData*);
		void create(InstanceData*);
		void destroy(InstanceData*);
		void alarm(InstanceData*, int);
		void step_begin(InstanceData*);
		void step_mid(InstanceData*);
		void step_end(InstanceData*);
		void keyboard_press(InstanceData*, SDL_Event*);
		void mouse_press(InstanceData*, SDL_Event*);
		void keyboard_input(InstanceData*, SDL_Event*);
		void mouse_input(InstanceData*, SDL_Event*);
		void keyboard_release(InstanceData*, SDL_Event*);
		void mouse_release(InstanceData*, SDL_Event*);
		void controller_axis(InstanceData*, SDL_Event*);
		void controller_press(InstanceData*, SDL_Event*);
		void controller_release(InstanceData*, SDL_Event*);
		void controller_modify(InstanceData*, SDL_Event*);
		void commandline_input(InstanceData*, const std::string&);
		void path_end(InstanceData*);
		void outside_room(InstanceData*);
		void intersect_boundary(InstanceData*);
		void collision(InstanceData*, InstanceData*);
		bool check_collision_list(const InstanceData&, const InstanceData&) const;
		void draw(InstanceData*);
		void animation_end(InstanceData*);
		void room_start(InstanceData*);
		void room_end(InstanceData*);
		void game_start(InstanceData*);
		void game_end(InstanceData*);
		void window(InstanceData*, SDL_Event*);*/

		/*void update(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->update(self);
		}
		void create(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->create(self);
		}
		void destroy(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->destroy(self);
		}
		void alarm(InstanceData* self, int a) {
			reinterpret_cast<const ObjType*>(this)->alarm(a);
		}
		void step_begin(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->step_begin(self);
		}
		void step_mid(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->step_mid(self);
		}
		void step_end(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->step_end(self);
		}
		void keyboard_press(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->keyboard_press(self, e);
		}
		void mouse_press(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->mouse_press(self, e);
		}
		void keyboard_input(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->keyboard_input(self, e);
		}
		void mouse_input(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->mouse_input(self, e);
		}
		void keyboard_release(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->keyboard_release(self, e);
		}
		void mouse_release(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->mouse_release(self, e);
		}
		void controller_axis(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->controller_axis(self, e);
		}
		void controller_press(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->controller_press(self, e);
		}
		void controller_release(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->controller_release(self, e);
		}
		void controller_modify(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->controller_modify(self, e);
		}
		void commandline_input(InstanceData* self, const std::string& str) {
			reinterpret_cast<const ObjType*>(this)->commandline_input(self, str);
		}
		void path_end(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->path_end(self);
		}
		void outside_room(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->outside_room(self);
		}
		void intersect_boundary(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->intersect_boundary(self);
		}
		void collision(InstanceData* self, InstanceData* other) {
			reinterpret_cast<const ObjType*>(this)->collision(self, other);
		}
		bool check_collision_list(const InstanceData& self, const InstanceData& other) const {
			reinterpret_cast<const ObjType*>(this)->check_collision_list(self, other);
		}
		void draw(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->draw(self);
		}
		void animation_end(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->animation_end(self);
		}
		void room_start(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->room_start(self);
		}
		void room_end(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->room_end(self);
		}
		void game_start(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->game_start(self);
		}
		void game_end(InstanceData* self) {
			reinterpret_cast<const ObjType*>(this)->game_end(self);
		}
		void window(InstanceData* self, SDL_Event* e) {
			reinterpret_cast<const ObjType*>(this)->window(self, e);
		}*/
};

/*
void BEE::Object::update(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->update(self);
}
void BEE::Object::create(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->create(self);
}
void BEE::Object::destroy(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->destroy(self);
}
void BEE::Object::alarm(InstanceData* self, int a) {
	reinterpret_cast<const ObjType*>(this)->alarm(a);
}
void BEE::Object::step_begin(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->step_begin(self);
}
void BEE::Object::step_mid(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->step_mid(self);
}
void BEE::Object::step_end(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->step_end(self);
}
void BEE::Object::keyboard_press(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->keyboard_press(self, e);
}
void BEE::Object::mouse_press(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->mouse_press(self, e);
}
void BEE::Object::keyboard_input(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->keyboard_input(self, e);
}
void BEE::Object::mouse_input(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->mouse_input(self, e);
}
void BEE::Object::keyboard_release(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->keyboard_release(self, e);
}
void BEE::Object::mouse_release(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->mouse_release(self, e);
}
void BEE::Object::controller_axis(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->controller_axis(self, e);
}
void BEE::Object::controller_press(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->controller_press(self, e);
}
void BEE::Object::controller_release(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->controller_release(self, e);
}
void BEE::Object::controller_modify(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->controller_modify(self, e);
}
void BEE::Object::commandline_input(InstanceData* self, const std::string& str) {
	reinterpret_cast<const ObjType*>(this)->commandline_input(self, str);
}
void BEE::Object::path_end(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->path_end(self);
}
void BEE::Object::outside_room(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->outside_room(self);
}
void BEE::Object::intersect_boundary(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->intersect_boundary(self);
}
void BEE::Object::collision(InstanceData* self, InstanceData* other) {
	reinterpret_cast<const ObjType*>(this)->collision(self, other);
}
bool BEE::Object::check_collision_list(const InstanceData& self, const InstanceData& other) const {
	reinterpret_cast<const ObjType*>(this)->check_collision_list(self, other);
}
void BEE::Object::draw(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->draw(self);
}
void BEE::Object::animation_end(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->animation_end(self);
}
void BEE::Object::room_start(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->room_start(self);
}
void BEE::Object::room_end(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->room_end(self);
}
void BEE::Object::game_start(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->game_start(self);
}
void BEE::Object::game_end(InstanceData* self) {
	reinterpret_cast<const ObjType*>(this)->game_end(self);
}
void BEE::Object::window(InstanceData* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->window(self, e);
}*/

#endif // _BEE_OBJECT_H
