class Sprite: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string image_path;
		int width, height;
		int subimage_amount;
		int origin_x, origin_y;
		
		SDL_Texture* texture;
		bool is_loaded;
	public:
		Sprite();
		Sprite(std::string, std::string);
		~Sprite();
		int reset();
		int print();
		
		int get_id();
		std::string get_name();
		std::string get_path();
		int get_width();
		int get_height();
		int get_subimage_amount();
		int get_origin_x();
		int get_origin_y();
		
		int set_name(std::string);
		int set_path(std::string);
		int set_origin_x(int);
		int set_origin_y(int);
		int set_origin_xy(int, int);
		int set_origin_center();
		
		int load();
		int free();
		int draw(int, int);
};
Sprite::Sprite () {
	id = resource_list.sprites.add_resource(this);
	if (id < 0) {
		fprintf(stderr, "Failed to add sprite resource: %d", id);
	}
	
	reset();
}
Sprite::Sprite (std::string new_name, std::string path) {
	id = resource_list.sprites.add_resource(this);
	if (id < 0) {
		fprintf(stderr, "Failed to add sprite resource: %d", id);
	}
	
	reset();
	set_name(new_name);
	set_path(path);
}
Sprite::~Sprite() {
	resource_list.sprites.remove_resource(id);
}
int Sprite::reset() {
	if (is_loaded) {
		free();
	}
	
	name = "";
	image_path = "";
	width = 0;
	height = 0;
	subimage_amount = 0;
	origin_x = 0;
	origin_y = 0;
	
	texture = NULL;
	is_loaded = false;
	
	return 0;
}
int Sprite::print() {
	std::cout <<
	"Sprite { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	image_path	" << image_path <<
	"\n	width		" << width <<
	"\n	height		" << height <<
	"\n	subimage_amount	" << subimage_amount <<
	"\n	origin_x	" << origin_x <<
	"\n	origin_y	" << origin_y <<
	"\n	texture		" << texture <<
	"\n	is_loaded	" << is_loaded <<
	"\n}\n";
	
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
int Sprite::get_width() {
	return width;
}
int Sprite::get_height() {
	return height;
}
int Sprite::get_subimage_amount() {
	return subimage_amount;
}
int Sprite::get_origin_x() {
	return origin_x;
}
int Sprite::get_origin_y() {
	return origin_y;
}
int Sprite::set_name(std::string new_name) {
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int Sprite::set_path(std::string path) {
	image_path = "resources/sprites/"+path;
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

int Sprite::load() {
	if (!is_loaded) {
		SDL_Surface* tmp_surface;
		tmp_surface = IMG_Load(image_path.c_str());
		if (tmp_surface == NULL) {
			std::cerr << "Failed to load sprite " << name << ": " << IMG_GetError() << "\n";
			return 1;
		}
		
		texture = SDL_CreateTextureFromSurface(game.renderer, tmp_surface);
		if (texture == NULL) {
			std::cerr << "Failed to create texture from surface: " << SDL_GetError() << "\n";
			return 1;
		}
		
		SDL_FreeSurface(tmp_surface);
		
		SDL_QueryTexture(texture, NULL, NULL, &width, &height);
		
		is_loaded = true;
	}
	return 0;
}
int Sprite::free() {
	if (is_loaded) {
		SDL_DestroyTexture(texture);
		texture = NULL;
		is_loaded = false;
	}
	return 0;
}
int Sprite::draw(int x, int y) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	
	SDL_RenderCopy(game.renderer, texture, NULL, &rect);
	
	return 0;
}
