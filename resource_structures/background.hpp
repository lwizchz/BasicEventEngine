class Background: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string background_path;
		int width, height;
		bool is_tiling;
		int tile_width, tile_height;
	public:
		Background();
		Background(std::string, std::string);
		~Background();
		int reset();
		int print();
		
		int get_id();
		std::string get_name();
		std::string get_path();
		int get_width();
		int get_height();
		bool get_is_tiling();
		int get_tile_width();
		int get_tile_height();
		int set_name(std::string);
		int load_path(std::string);
		int set_is_tiling(bool);
		int set_tile_width(int);
		int set_tile_height(int);
};
Background::Background () {
	id = resource_list.backgrounds.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add background resource: %d", id);
	}
	
	reset();
}
Background::Background (std::string new_name, std::string path) {
	id = resource_list.backgrounds.add_resource(*this);
	if (id < 0) {
		fprintf(stderr, "Failed to add background resource: %d", id);
	}
	
	reset();
	set_name(new_name);
	load_path(path);
}
Background::~Background() {
	resource_list.backgrounds.remove_resource(id);
}
int Background::reset() {
	name = "";
	background_path = "";
	is_tiling = false;
	
	return 0;
}
int Background::print() {
	std::cout <<
	"Background { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	background_path	" << background_path <<
	"\n	width		" << width <<
	"\n	height		" << height <<
	"\n	is_tiling	" << is_tiling <<
	"\n}\n";
	
	return 0;
}
int Background::get_id() {
	return id;
}
std::string Background::get_name() {
	return name;
}
std::string Background::get_path() {
	return background_path;
}
int Background::get_width() {
	return width;
}
int Background::get_height() {
	return height;
}
bool Background::get_is_tiling() {
	return is_tiling;
}
int Background::get_tile_width() {
	return tile_width;
}
int Background::get_tile_height() {
	return tile_height;
}
int Background::set_name(std::string new_name) {
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int Background::load_path(std::string path) {
	background_path = path;
	// Load OpenGL texture
	/* texture = 
	 * width = 
	 * height = 
	 */
	return 0;
}
int Background::set_is_tiling(bool new_is_tiling) {
	is_tiling = new_is_tiling;
	return 0;
}
int Background::set_tile_width(int new_tile_width) {
	tile_width = new_tile_width;
	return 0;
}
int Background::set_tile_height(int new_tile_height) {
	tile_height = new_tile_height;
	return 0;
}
