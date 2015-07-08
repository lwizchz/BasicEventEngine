#include <iostream>
#include <map>
#include <tuple>

typedef std::tuple<int,int,int> rgb;

class BackgroundData {
	public:
		Background* background;
		bool is_visible;
		bool is_foreground;
		int x, y;
		bool is_horizontal_tile, is_vertical_tile;
		int horizontal_speed, vertical_speed;
		bool is_stretched;
};
class ViewData {
	public:
		bool is_visible;
		int view_x, view_y, view_width, view_height;
		int port_x, port_y, port_width, port_height;
		Object* object;
		int object_horizontal_border, object_vertical_border;
		int horizontal_speed, vertical_speed;
};
class InstanceData {
	public:
		int id;
		Object* object;
		int x, y;
		InstanceData() {id=-1;object=0;x=0;y=0;};
		int init(int, Object*, int, int);
};
int InstanceData::init(int new_id, Object* new_object, int new_x, int new_y) {
	id = new_id;
	object = new_object;
	x = new_x;
	y = new_y;
	return 0;
}

class Room: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string room_path;
		int width, height;
		bool is_isometric;
		int speed;
		bool is_persistent;
		rgb background_color;
		bool is_background_color_enabled;
		std::map<int,BackgroundData*> backgrounds;
		bool is_views_enabled;
		std::map<int,ViewData*> views;
		std::map<int,InstanceData*> instances;
	public:
		Room();
		Room(std::string, std::string);
		~Room();
		int reset();
		int print();
		
		int get_id();
		std::string get_name();
		std::string get_path();
		int get_width();
		int get_height();
		bool get_is_isometric();
		int get_speed();
		bool get_is_persistent();
		rgb get_background_color();
		bool get_is_background_color_enabled();
		std::map<int,BackgroundData*> get_backgrounds();
		std::string get_background_string();
		bool get_is_views_enabled();
		std::map<int,ViewData*> get_views();
		std::string get_view_string();
		std::map<int,InstanceData*> get_instances();
		std::string get_instance_string();
		
		int set_name(std::string);
		int set_path(std::string);
		int set_width(int);
		int set_height(int);
		int set_is_isometric(bool);
		int set_speed(int);
		int set_is_persistent(bool);
		int set_background_color(rgb);
		int set_background_color(int, int, int);
		int set_is_background_color_enabled(bool);
		int set_background(int, BackgroundData*);
		int set_is_views_enabled(bool);
		int set_view(int, ViewData*);
		int set_instance(int, InstanceData*);
		int add_instance(InstanceData*);
		int remove_instance(int);
		
		int load_media();
		int free_media();
		
		int create();
		int destroy();
		int alarm(int);
		int step_begin();
		int step_mid();
		int step_end();
		int keyboard(SDL_Event*);
		int mouse(SDL_Event*);
		int keyboard_press(SDL_Event*);
		int mouse_press(SDL_Event*);
		int keyboard_release(SDL_Event*);
		int mouse_release(SDL_Event*);
		int path_end();
		int outside_room();
		int intersect_boundary();
		int collision();
		int draw();
		int animation_end();
		int room_start();
		int room_end();
		int game_start();
		int game_end();
};
Room::Room () {
	id = resource_list.rooms.add_resource(this);
	if (id < 0) {
		fprintf(stderr, "Failed to add room resource: %d", id);
	}
	
	reset();
}
Room::Room (std::string new_name, std::string path) {
	id = resource_list.rooms.add_resource(this);
	if (id < 0) {
		fprintf(stderr, "Failed to add room resource: %d", id);
	}
	
	reset();
	set_name(new_name);
	set_path(path);
}
Room::~Room() {
	resource_list.rooms.remove_resource(id);
}
int Room::reset() {
	name = "";
	room_path = "";
	width = 1280;
	height = 720;
	is_isometric = false;
	speed = 60;
	is_persistent = false;
	background_color = std::make_tuple(192, 192, 192);
	is_background_color_enabled = true;
	backgrounds.empty();
	is_views_enabled = false;
	views.empty();
	instances.empty();
	
	return 0;
}
int Room::print() {
	std::string background_string = get_background_string();
	std::string view_string = get_view_string();
	std::string instance_string = get_instance_string();
	
	std::cout <<
	"Room { "
	"\n	id				" << id <<
	"\n	name				" << name <<
	"\n	room_path			" << room_path <<
	"\n	width				" << width <<
	"\n	height				" << height <<
	"\n	is_isometric			" << is_isometric <<
	"\n	speed				" << speed <<
	"\n	is_persistent			" << is_persistent <<
	"\n	background_color		" << std::get<0>(background_color) << ", " << std::get<1>(background_color) << ", " << std::get<0>(background_color) <<
	"\n	is_background_color_enabled	" << is_background_color_enabled <<
	"\n	backgrounds			\n" << debug_indent(background_string, 2) <<
	"	is_views_enabled		" << is_views_enabled <<
	"\n	views				\n" << debug_indent(view_string, 2) <<
	"	instances			\n" << debug_indent(instance_string, 2) <<
	"}\n";
	
	return 0;
}
int Room::get_id() {
	return id;
}
std::string Room::get_name() {
	return name;
}
std::string Room::get_path() {
	return room_path;
}
int Room::get_width() {
	return width;
}
int Room::get_height() {
	return height;
}
bool Room::get_is_isometric() {
	return is_isometric;
}
int Room::get_speed() {
	return speed;
}
bool Room::get_is_persistent() {
	return is_persistent;
}
rgb Room::get_background_color() {
	return background_color;
}
bool Room::get_is_background_color_enabled() {
	return is_background_color_enabled;
}
std::map<int, BackgroundData*> Room::get_backgrounds() {
	return backgrounds;
}
std::string Room::get_background_string() {
	if (backgrounds.size() > 0) {
		std::ostringstream background_string;
		background_string << "(name	visible	fore	x	y	htile	vtile	hspeed	vspeed	stretch)\n";
		for (unsigned int i=0; i<backgrounds.size(); i++) {
			background_string <<
			backgrounds[i]->background->get_name() << "\t" <<
			backgrounds[i]->is_visible << "\t" <<
			backgrounds[i]->is_foreground << "\t" <<
			backgrounds[i]->x << "\t" <<
			backgrounds[i]->y << "\t" <<
			backgrounds[i]->is_horizontal_tile << "\t" <<
			backgrounds[i]->is_vertical_tile << "\t" <<
			backgrounds[i]->horizontal_speed << "\t" <<
			backgrounds[i]->vertical_speed << "\t" <<
			backgrounds[i]->is_stretched << "\n";
		}
		
		return background_string.str();
	}
	return "none\n";
}
bool Room::get_is_views_enabled() {
	return is_views_enabled;
}
std::map<int, ViewData*> Room::get_views() {
	return views;
}
std::string Room::get_view_string() {
	if (views.size() > 0) {
		std::ostringstream view_string;
		view_string << "(visible	vx, vy	vwidth	vheight	px, py	pwidth	pheight	object	hborder	vborder	hspeed	vspeed)\n";
		for (unsigned int i=0; i<views.size(); i++) {
			view_string <<
			views[i]->is_visible << "\t" <<
			views[i]->view_x << ", " << views[i]->view_y <<"\t" <<
			views[i]->view_width << "\t" <<
			views[i]->view_height << "\t" <<
			views[i]->port_x << ", " << views[i]->port_y << "\t" <<
			views[i]->port_width << "\t" <<
			views[i]->port_height << "\t" <<
			views[i]->object->get_name() << "\t" <<
			views[i]->object_horizontal_border << "\t" <<
			views[i]->object_vertical_border << "\t" <<
			views[i]->horizontal_speed << "\t" <<
			views[i]->vertical_speed << "\n";
		}
		
		return view_string.str();
	}
	return "none\n";
}
std::map<int, InstanceData*> Room::get_instances() {
	return instances;
}
std::string Room::get_instance_string() {
	if (instances.size() > 0) {
		std::ostringstream instance_string;
		instance_string << "(id	object	x	y)\n";
		for (unsigned int i=0; i<instances.size(); i++) {
			instance_string <<
			instances[i]->id << "\t" <<
			instances[i]->object->get_name() << "\t" <<
			instances[i]->x << "\t" <<
			instances[i]->y << "\n";
		}
		
		return instance_string.str();
	}
	return "none\n";
}
int Room::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int Room::set_path(std::string path) {
	room_path = "resources/rooms/"+path;
	
	// Load XML Room data
	/*
	 * 
	 */
	
	return 0;
}
int Room::set_width(int new_width) {
	width = new_width;
	return 0;
}
int Room::set_height(int new_height) {
	height = new_height;
	return 0;
}
int Room::set_is_isometric(bool new_is_isometric) {
	is_isometric = new_is_isometric;
	return 0;
}
int Room::set_speed(int new_speed) {
	speed = new_speed;
	return 0;
}
int Room::set_is_persistent(bool new_is_persistent) {
	is_persistent = new_is_persistent;
	return 0;
}
int Room::set_background_color(rgb new_background_color) {
	background_color = new_background_color;
	return 0;
}
int Room::set_background_color(int r, int g, int b) {
	background_color = std::make_tuple(r, g, b);
	return 0;
}
int Room::set_is_background_color_enabled(bool new_is_background_color_enabled) {
	is_background_color_enabled = new_is_background_color_enabled;
	return 0;
}
int Room::set_background(int index, BackgroundData* new_background) {
	if (backgrounds.find(index) != backgrounds.end()) { // if the background exists, overwrite it
		backgrounds.erase(index);
	}
	backgrounds.insert(std::pair<int,BackgroundData*>(index,new_background));
	return 0;
}
int Room::set_is_views_enabled(bool new_is_views_enabled) {
	is_views_enabled = new_is_views_enabled;
	return 0;
}
int Room::set_view(int index, ViewData* new_view) {
	if (views.find(index) != views.end()) { // if the view exists, overwrite it
		views.erase(index);
	}
	views.insert(std::pair<int,ViewData*>(index, new_view));
	return 0;
}
int Room::set_instance(int index, InstanceData* new_instance) {
	if (instances.find(index) != instances.end()) { //  if the instance exists, overwrite it
		instances.erase(index);
	}
	instances.insert(std::pair<int,InstanceData*>(index, new_instance));
	return 0;
}
int Room::add_instance(InstanceData* new_instance) {
	int index = new_instance->id;
	if (index < 0) {
		index = instances.size();
		new_instance->id = index;
	}
	return set_instance(index, new_instance);
}
int Room::remove_instance(int index) {
	instances.erase(index);
	return 0;
}

int Room::load_media() {
	for (unsigned int i=0; i<instances.size(); i++) {
		instances[i]->object->get_sprite()->load();
	}
	
	return 0;
}
int Room::free_media() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->get_sprite()->free();
	}
	
	return 0;
}

int Room::create() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->create(instances[i]);
	}
	
	return 0;
}
int Room::destroy() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->destroy(instances[i]);
	}
	
	return 0;
}
int Room::alarm(int alarm) {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->alarm(instances[i], alarm);
	}
	
	return 0;
}
int Room::step_begin() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->step_begin(instances[i]);
	}
	
	return 0;
}
int Room::step_mid() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->step_mid(instances[i]);
	}
	
	return 0;
}
int Room::step_end() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->step_end(instances[i]);
	}
	
	return 0;
}
int Room::keyboard(SDL_Event* e) {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->keyboard(instances[i], e);
	}
	
	return 0;
}
int Room::mouse(SDL_Event* e) {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->mouse(instances[i], e);
	}
	
	return 0;
}
int Room::keyboard_press(SDL_Event* e) {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->keyboard_press(instances[i], e);
	}
	
	return 0;
}
int Room::mouse_press(SDL_Event* e) {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->mouse_press(instances[i], e);
	}
	
	return 0;
}
int Room::keyboard_release(SDL_Event* e) {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->keyboard_release(instances[i], e);
	}
	
	return 0;
}
int Room::mouse_release(SDL_Event* e) {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->mouse_release(instances[i], e);
	}
	
	return 0;
}
int Room::path_end() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->path_end(instances[i]);
	}
	
	return 0;
}
int Room::outside_room() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->outside_room(instances[i]);
	}
	
	return 0;
}
int Room::intersect_boundary() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->intersect_boundary(instances[i]);
	}
	
	return 0;
}
int Room::collision() {
	int otherid = -1;
	
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->collision(instances[i], otherid);
	}
	
	return 0;
}
int Room::draw() {
	SDL_RenderClear(game.renderer);
	
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->draw(instances[i]);
	}
	
	SDL_RenderPresent(game.renderer);
	
	return 0;
}
int Room::animation_end() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->animation_end(instances[i]);
	}
	
	return 0;
}
int Room::room_start() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->room_start(instances[i]);
	}
	
	return 0;
}
int Room::room_end() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->room_end(instances[i]);
	}
	
	return 0;
}
int Room::game_start() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->game_start(instances[i]);
	}
	
	return 0;
}
int Room::game_end() {
	for (unsigned int i=0; i < instances.size(); i++) {
		instances[i]->object->game_end(instances[i]);
	}
	
	return 0;
}
