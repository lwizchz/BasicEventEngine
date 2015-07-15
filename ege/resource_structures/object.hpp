/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of EGE.
* EGE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _EGE_OBJECT_H
#define _EGE_OBJECT_H 1

class InstanceData; // Defined in ege/resource_structures/room.hpp

class Object: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string object_path;
		Sprite *sprite;
		bool is_solid, is_visible, is_persistent;
		int depth;
		Object *parent, *who;
		Sprite *mask;
	public:
		Object();
		Object(std::string, std::string);
		~Object();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		Sprite* get_sprite();
		int get_sprite_id();
		bool get_is_solid();
		bool get_is_visible();
		bool get_is_persistent();
		int get_depth();
		Object* get_parent();
		int get_parent_id();
		Sprite* get_mask();
		int get_mask_id();

		int set_name(std::string);
		int set_path(std::string);
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

		virtual void create(InstanceData*) {};
		virtual void destroy(InstanceData*) {};
		virtual void alarm(InstanceData*, int) {};
		virtual void step_begin(InstanceData*) {};
		virtual void step_mid(InstanceData*) {};
		virtual void step_end(InstanceData*) {};
		virtual void keyboard(InstanceData*, SDL_Event*) {};
		virtual void mouse(InstanceData*, SDL_Event*) {};
		virtual void keyboard_press(InstanceData*, SDL_Event*) {};
		virtual void mouse_press(InstanceData*, SDL_Event*) {};
		virtual void keyboard_release(InstanceData*, SDL_Event*) {};
		virtual void mouse_release(InstanceData*, SDL_Event*) {};
		virtual void path_end(InstanceData*) {};
		virtual void outside_room(InstanceData*) {};
		virtual void intersect_boundary(InstanceData*) {};
		virtual void collision(InstanceData*, int) {};
		virtual void draw(InstanceData*) {};
		virtual void animation_end(InstanceData*) {};
		virtual void room_start(InstanceData*) {};
		virtual void room_end(InstanceData*) {};
		virtual void game_start(InstanceData*) {};
		virtual void game_end(InstanceData*) {};
};
Object::Object () {
	id = -1;
	reset();
}
Object::Object (std::string new_name, std::string path) {
	id = -1;
	reset();

	add_to_resources("resources/objects/"+path);
	if (id < 0) {
		std::cerr << "Failed to add object resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
Object::~Object() {
	resource_list.objects.remove_resource(id);
}
int Object::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == object_path) {
			return 1;
		}
		resource_list.objects.remove_resource(id);
		id = -1;
	} else {
		for (auto i : resource_list.objects.resources) {
			if ((i.second != NULL)&&(i.second->get_path() == path)) {
				list_id = i.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = resource_list.objects.add_resource(this);
	}
	resource_list.objects.set_resource(id, this);

	return 0;
}
int Object::reset() {
	name = "";
	object_path = "";
	sprite = NULL;
	is_solid = false;
	is_visible = true;
	is_persistent = false;
	depth = 0;
	parent = NULL;
	who = this;
	mask = NULL;

	return 0;
}
int Object::print() {
	std::cout <<
	"Object { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	object_path	" << object_path;
	if (sprite != NULL) {
		std::cout << "\n	sprite		" << sprite->get_id() << ", " << sprite->get_name();
	} else {
		std::cout << "\n	sprite		NULL";
	}
	std::cout <<
	"\n	is_solid	" << is_solid <<
	"\n	is_visible	" << is_visible <<
	"\n	is_persistent	" << is_persistent <<
	"\n	depth		" << depth;
	if (parent != NULL) {
		std::cout << "\n	parent		" << parent->get_id() << ", " << parent->get_name();
	} else {
		std::cout << "\n	parent		NULL";
	}
	if (who != NULL) {
		std::cout << "\n	who		" << who->get_id() << ", " << who->get_name();
	} else {
		std::cout << "\n	who		NULL";
	}
	if (mask != NULL) {
		std::cout << "\n	mask		" << mask->get_id() << ", " << mask->get_name();
	} else {
		std::cout << "\n	mask		NULL";
	}
	std::cout << "\n}\n";

	return 0;
}
int Object::get_id() {
	return id;
}
std::string Object::get_name() {
	return name;
}
std::string Object::get_path() {
	return object_path;
}
Sprite* Object::get_sprite() {
	return sprite;
}
int Object::get_sprite_id() {
	return sprite->get_id();
}
bool Object::get_is_solid() {
	return is_solid;
}
bool Object::get_is_visible() {
	return is_visible;
}
bool Object::get_is_persistent() {
	return is_persistent;
}
int Object::get_depth() {
	return depth;
}
Object* Object::get_parent() {
	return parent;
}
int Object::get_parent_id() {
	return parent->get_id();
}
Sprite* Object::get_mask() {
	return mask;
}
int Object::get_mask_id() {
	return mask->get_id();
}
int Object::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int Object::set_path(std::string path) {
	add_to_resources("resources/objects/"+path);
	object_path = "resources/objects/"+path;
	return 0;
}
int Object::set_sprite(Sprite* new_sprite) {
	sprite = new_sprite;
	return 0;
}
int Object::set_is_solid(bool new_is_solid) {
	is_solid = new_is_solid;
	return 0;
}
int Object::set_is_visible(bool new_is_visible) {
	is_visible = new_is_visible;
	return 0;
}
int Object::set_is_persistent(bool new_is_persistent) {
	is_persistent = new_is_persistent;
	return 0;
}
int Object::set_depth(int new_depth) {
	depth = new_depth;
	return 0;
}
int Object::set_parent(Object* new_parent) {
	parent = new_parent;
	return 0;
}
int Object::set_mask(Sprite* new_mask) {
	mask = new_mask;
	return 0;
}

#endif // _EGE_OBJECT_H
