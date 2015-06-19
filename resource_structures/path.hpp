#include <vector>
#include <tuple>
#include <string>

typedef std::tuple<int, int, int> path_coord;

class Path: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string path_path;
		std::vector<path_coord> coordinate_list; // [x, y, speed]
		int connection_type; // Straight vs smooth curve
		bool is_closed;
	public:
		Path();
		Path(std::string, std::string);
		~Path();
		int reset();
		int print();
		
		int get_id();
		std::string get_name();
		std::string get_path();
		std::vector<path_coord> get_coordinate_list();
		std::string get_coordinate_string();
		int set_name(std::string);
		int load_path(std::string);
};
Path::Path () {
	id = resource_list.paths.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add path resource: %d", id);
	}
	
	reset();
}
Path::Path (std::string new_name, std::string path) {
	id = resource_list.paths.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add path resource: %d", id);
	}
	
	reset();
	set_name(new_name);
	load_path(path);
}
Path::~Path() {
	resource_list.paths.remove_resource(id);
}
int Path::reset() {
	name = "";
	path_path = "";
	coordinate_list.clear();
	
	return 0;
}
int Path::print() {
	std::string coordinate_string = get_coordinate_string();
	
	std::cout <<
	"Path { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	path_path	" << path_path <<
	"\n	coordinate_list \n" << coordinate_string <<
	"\n}\n";
	
	return 0;
}
int Path::get_id() {
	return id;
}
std::string Path::get_name() {
	return name;
}
std::string Path::get_path() {
	return path_path;
}
std::vector<path_coord> Path::get_coordinate_list() {
	return coordinate_list;
}
std::string Path::get_coordinate_string() {
	std::string coordinate_string;
	for (std::vector<path_coord>::iterator it = coordinate_list.begin(); it != coordinate_list.end(); ++it) {
		coordinate_string.append("\t\t");
		coordinate_string.append(std::to_string(std::get<0>(*it)));
		coordinate_string.append("\t");
		coordinate_string.append(std::to_string(std::get<1>(*it)));
		coordinate_string.append("\t");
		coordinate_string.append(std::to_string(std::get<2>(*it)));
	}
	
	return coordinate_string;
}
int Path::set_name(std::string new_name) {
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int Path::load_path(std::string path) {
	path_path = path;
	// Load XML Path data
	/* 
	 * coordinate_list.push_back(x, y, speed);
	 */
	return 0;
}
