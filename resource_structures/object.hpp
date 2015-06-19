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
		int load_path(std::string);
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
		
		virtual void create() {};
		virtual void destroy() {};
		virtual void alarm(int) {};
		virtual void step_begin() {};
		virtual void step_mid() {};
		virtual void step_end() {};
		virtual void keyboard(int) {};
		virtual void mouse(int) {};
		virtual void keyboard_press(int) {};
		virtual void mouse_press(int) {};
		virtual void keyboard_release(int) {};
		virtual void mouse_release(int) {};
		virtual void path_end() {};
		virtual void outside_room() {};
		virtual void intersect_boundary() {};
		virtual void collision(int) {};
		virtual void draw() {};
		virtual void animation_end() {};
		virtual void room_start() {};
		virtual void room_end() {};
		virtual void game_start() {};
		virtual void game_end() {};
};
Object::Object () {
	id = resource_list.objects.add_resource(this);
	if (id < 0) {
		fprintf(stderr, "Failed to add object resource: %d", id);
	}
	
	reset();
}
Object::Object (std::string new_name, std::string object) {
	id = resource_list.objects.add_resource(this);
	if (id < 0) {
		fprintf(stderr, "Failed to add object resource: %d", id);
	}
	
	reset();
	set_name(new_name);
	load_path(object);
}
Object::~Object() {
	resource_list.objects.remove_resource(id);
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
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int Object::load_path(std::string object) {
	object_path = object;
	// Load XML Object data
	/* 
	 * sprite = 
	 * is_solid = 
	 * is_visible = 
	 * is_persistent = 
	 * depth = 
	 * parent = 
	 * mask = 
	 */
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
