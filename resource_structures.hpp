#include <string>
#include <map>

class Resource {
		//int global_id;
	public:
		Resource();
		~Resource();
		//int get_global_id();
};
class ResourceList {
		std::map<int,Resource> resources;
		int next_index;
	public:
		ResourceList();
		int add_resource(Resource);
		Resource get_resource(int);
		int get_amount();
		int remove_resource(int);
};
class MetaResourceList {
	public:
		ResourceList sprites; //, all;
} resource_list;

Resource::Resource() {
	/*global_id = resource_list.all.add_resource(*this);
	if (global_id < 0) {
		fprintf(stderr, "Failed to add global resource: %d\n", global_id);
	}*/
}
Resource::~Resource() {
	//resource_list.all.remove_resource(global_id);
}
/*int Resource::get_global_id() {
	return global_id;
}*/

ResourceList::ResourceList() {
	next_index = 0;
}
int ResourceList::add_resource(Resource new_resource) {
	int index = next_index;
	std::pair<std::map<int,Resource>::iterator,bool> ret;
	ret = resources.insert(std::pair<int,Resource>(index, new_resource));
	if (ret.second) {
		next_index++;
		return index;
	}
	return -1;
}
Resource ResourceList::get_resource(int id) {
	return resources[id];
}
int ResourceList::get_amount() {
	return resources.size();
}
int ResourceList::remove_resource(int id) {
	resources.erase(id);
	return 0;
}

class Sprite: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string image_path;
		//Texture texture;
		int width, height;
		int subimage_amount;
		int origin_x, origin_y;
	public:
		Sprite();
		Sprite(std::string, std::string);
		~Sprite();
		int reset();
		int get_id();
		std::string get_name();
		std::string get_path();
		int set_name(std::string);
		int load_image_path(std::string);
		int set_origin_x(int);
		int set_origin_y(int);
		int set_origin_xy(int, int);
		int set_origin_center();
		int print();
};
Sprite::Sprite () {
	id = resource_list.sprites.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add sprite resource: %d", id);
	}
	
	reset();
}
Sprite::Sprite (std::string new_name, std::string path) {
	id = resource_list.sprites.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add sprite resource: %d", id);
	}
	
	reset();
	set_name(new_name);
	load_image_path(path);
}
Sprite::~Sprite() {
	resource_list.sprites.remove_resource(id);
}
int Sprite::reset() {
	name = "";
	image_path = "";
	//texture = 
	width = 0;
	height = 0;
	subimage_amount = 0;
	origin_x = 0;
	origin_y = 0;
	
	return 0;
}
int Sprite::get_id() {
	return id;
}
std::string Sprite::get_name() {
	return name;
}
std::string Sprite::get_path() {
	return image_path;
}
int Sprite::set_name(std::string new_name) {
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int Sprite::load_image_path(std::string path) {
	image_path = path;
	// Load OpenGL texture
	/* texture = 
	 * width = 
	 * height = 
	 * subimage_amount = 
	 */
	return 0;
}
int Sprite::set_origin_x(int new_origin_x) {
	origin_x = new_origin_x;
	return 0;
}
int Sprite::set_origin_y(int new_origin_y) {
	origin_y = new_origin_y;
	return 0;
}
int Sprite::set_origin_xy(int new_origin_x, int new_origin_y) {
	set_origin_x(new_origin_x);
	set_origin_y(new_origin_y);
	return 0;
}
int Sprite::set_origin_center() {
	set_origin_x(width/2);
	set_origin_y(height/2);
	return 0;
}
int Sprite::print() {
	std::cout <<
	"Sprite { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	image_path	" << image_path <<
	"\n	texture		" << "//" <<
	"\n	width		" << width <<
	"\n	height		" << height <<
	"\n	subimage_amount	" << subimage_amount <<
	"\n	origin_x	" << origin_x <<
	"\n	origin_y	" << origin_y <<
	"\n}\n";
	
	return 0;
}
