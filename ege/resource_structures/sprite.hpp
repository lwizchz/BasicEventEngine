#define MAX_SPRITE_IMAGES 8

class Sprite: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string image_path;
		int width, height;
		int subimage_amount, subimage_width, current_subimage;
		Uint32 subimage_time;
		float speed, alpha;
		bool is_animated;
		int origin_x, origin_y;
		
		SDL_Texture* texture;
		bool is_loaded;
		SDL_Rect subimages[MAX_SPRITE_IMAGES];
	public:
		Sprite();
		Sprite(std::string, std::string);
		~Sprite();
		int add_to_resources(std::string);
		int reset();
		int print();
		
		int get_id();
		std::string get_name();
		std::string get_path();
		int get_width();
		int get_height();
		int get_subimage_amount();
		int get_subimage_width();
		float get_speed();
		float get_alpha();
		int get_origin_x();
		int get_origin_y();
		
		int set_name(std::string);
		int set_path(std::string);
		int set_subimage_amount(int, int);
		int set_speed(float);
		int set_alpha(float);
		int set_origin_x(int);
		int set_origin_y(int);
		int set_origin_xy(int, int);
		int set_origin_center();
		
		int load();
		int free();
		int draw(int, int);
};
Sprite::Sprite () {
	id = -1;
	reset();
}
Sprite::Sprite (std::string new_name, std::string path) {
	id = -1;
	is_loaded = false;
	reset();
	
	add_to_resources("resources/sprites/"+path);
	if (id < 0) {
		std::cerr << "Failed to add sprite resource: " << path << "\n";
		throw(-1);
	}
	
	set_name(new_name);
	set_path(path);
}
Sprite::~Sprite() {
	free();
}
int Sprite::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == image_path) {
			return 1;
		}
		resource_list.sprites.remove_resource(id);
		id = -1;
	} else {
		for (auto s : resource_list.sprites.resources) {
			if ((s.second != NULL)&&(s.second->get_path() == path)) {
				list_id = s.first;
				break;
			}
		}
	}
	
	if (list_id >= 0) {
		id = list_id;
	} else {
		id = resource_list.sprites.add_resource(this);
	}
	resource_list.sprites.set_resource(id, this);
	
	return 0;
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
	subimage_width = 0;
	current_subimage = 0;
	subimage_time = 0;
	speed = 0.0;
	alpha = 1.0;
	is_animated = false;
	origin_x = 0;
	origin_y = 0;
	
	texture = NULL;
	is_loaded = false;
	for (int i=0; i<MAX_SPRITE_IMAGES; i++) {
		subimages[i].x = 0;
		subimages[i].y = 0;
		subimages[i].w = 0;
		subimages[i].h = 0;
	}
	
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
	"\n	subimage_width	" << subimage_width <<
	"\n	speed		" << speed <<
	"\n	alpha		" << alpha <<
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
int Sprite::get_subimage_width() {
	return subimage_width;
}
float Sprite::get_speed() {
	return speed;
}
float Sprite::get_alpha() {
	return alpha;
}
int Sprite::get_origin_x() {
	return origin_x;
}
int Sprite::get_origin_y() {
	return origin_y;
}
int Sprite::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int Sprite::set_path(std::string path) {
	add_to_resources("resources/sprites/"+path);
	image_path = "resources/sprites/"+path;
	return 0;
}
int Sprite::set_subimage_amount(int new_subimage_amount, int new_subimage_width) {
	subimage_amount = new_subimage_amount;
	subimage_width = new_subimage_width;
	for (int i=0; i<subimage_amount; i++) {
		subimages[i].x = i*subimage_width;
		subimages[i].w = subimage_width;
	}
	subimage_time = SDL_GetTicks();
	return 0;
}
int Sprite::set_speed(float new_speed) {
	speed = new_speed;
	return 0;
}
int Sprite::set_alpha(float new_alpha) {
	alpha = new_alpha;
	SDL_SetTextureAlphaMod(texture, alpha*255);
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
		
		texture = SDL_CreateTextureFromSurface(game->renderer, tmp_surface);
		if (texture == NULL) {
			std::cerr << "Failed to create texture from surface: " << SDL_GetError() << "\n";
			return 1;
		}
		
		SDL_FreeSurface(tmp_surface);
		
		SDL_QueryTexture(texture, NULL, NULL, &width, &height);
		if (subimage_amount == 0) {
			set_subimage_amount(1, width);
		}
		
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(texture, alpha*255);
		
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
	current_subimage = (int)round(speed*(SDL_GetTicks()-subimage_time)/game->fps) % subimage_amount;
	if (current_subimage == 0) {
		is_animated = true;
	}
	
	SDL_Rect srect, drect;
	
	srect.x = subimages[current_subimage].x;
	srect.y = 0;
	srect.w = subimages[current_subimage].w;
	srect.h = height;
	
	drect.x = x;
	drect.y = y;
	drect.w = subimage_width;
	drect.h = height;
	
	SDL_RenderCopy(game->renderer, texture, &srect, &drect);
	
	if ((is_animated)&&(current_subimage == subimage_amount-1)) {
		game->animation_end(this);
		is_animated = false;
	}
	
	return 0;
}
