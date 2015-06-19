class Object: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string object_path;
		Sprite sprite;
		bool is_solid, is_visible;
		int depth;
		// finish list of vars
	public:
		Object();
		Object(std::string, std::string);
		~Object();
		int reset();
		int print();
		
		int get_id();
		std::string get_name();
		
		int set_name(std::string);
		int load_object(std::string);
};
Object::Object () {
	id = resource_list.objects.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add object resource: %d", id);
	}
	
	reset();
}
Object::Object (std::string new_name, std::string object) {
	id = resource_list.objects.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add object resource: %d", id);
	}
	
	reset();
	set_name(new_name);
	load_object(object);
}
Object::~Object() {
	resource_list.objects.remove_resource(id);
}
int Object::reset() {
	name = "";
	object_path = "";
	// Reset sprite
	
	return 0;
}
int Object::print() {
	std::cout <<
	"Object { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	object_path	" << object_path <<
	"\n}\n";
	
	return 0;
}
int Object::get_id() {
	return id;
}
std::string Object::get_name() {
	return name;
}
int Object::set_name(std::string new_name) {
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int Object::load_object(std::string object) {
	object_path = object;
	// Load XML Object data
	/* 
	 * coordinate_list.push_back(x, y, speed);
	 */
	return 0;
}
