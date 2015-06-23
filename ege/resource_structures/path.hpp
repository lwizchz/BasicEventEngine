#include <iostream>
#include <vector>
#include <tuple>

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
		int get_connection_type();
		bool get_is_closed();
		
		int set_name(std::string);
		int load_path(std::string);
		int add_coordinate(path_coord);
		int add_coordinate(int, int, int);
		int remove_last_coordinate();
		int remove_coordinate(unsigned int);
		int set_connection_type(int);
		int set_is_closed(bool);
};
Path::Path () {
	id = resource_list.paths.add_resource(this);
	if (id < 0) {
		fprintf(stderr, "Failed to add path resource: %d", id);
	}
	
	reset();
}
Path::Path (std::string new_name, std::string path) {
	id = resource_list.paths.add_resource(this);
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
	connection_type = 0;
	is_closed = true;
	
	return 0;
}
int Path::print() {
	std::string coordinate_string = get_coordinate_string();
	
	std::cout <<
	"Path { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	path_path	" << path_path <<
	"\n	coordinate_list \n" << debug_indent(coordinate_string, 2) <<
	"	connection_type	" << connection_type <<
	"\n	is_closed	" << is_closed <<
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
	if (coordinate_list.size() > 0) {
		std::ostringstream coordinate_string;
		coordinate_string << "(x	y	speed)\n";
		for (std::vector<path_coord>::iterator it = coordinate_list.begin(); it != coordinate_list.end(); ++it) {
			coordinate_string <<
			std::get<0>(*it) << "\t" <<
			std::get<1>(*it) << "\t" <<
			std::get<2>(*it) << "\n";
		}
		
		return coordinate_string.str();
	}
	return "none\n";
}
int Path::get_connection_type() {
	return connection_type;
}
bool Path::get_is_closed() {
	return is_closed;
}
int Path::set_name(std::string new_name) {
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int Path::load_path(std::string path) {
	path_path = "resources/paths/"+path;
	// Load XML Path data
	/* 
	 * coordinate_list.push_back(x, y, speed);
	 */
	return 0;
}
int Path::add_coordinate(path_coord new_coordinate) {
	coordinate_list.push_back(new_coordinate);
	return 0;
}
int Path::add_coordinate(int x, int y, int speed) {
	path_coord new_coordinate (x, y, speed);
	coordinate_list.push_back(new_coordinate);
	return 0;
}
int Path::remove_last_coordinate() {
	if (!coordinate_list.empty()) {
		coordinate_list.pop_back();
		return 0;
	}
	return 1;
}
int Path::remove_coordinate(unsigned int index) {
	if (index < coordinate_list.size()) {
		coordinate_list.erase(coordinate_list.begin()+index);
		return 0;
	}
	return 1;
}
int Path::set_connection_type(int new_conneciton_type) {
	connection_type = new_conneciton_type;
	return 0;
}
int Path::set_is_closed(bool new_is_closed) {
	is_closed = new_is_closed;
	return 0;
}
