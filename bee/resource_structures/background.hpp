/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_BACKGROUND_H
#define _BEE_BACKGROUND_H 1

class BackgroundData { // Used to pass data to the Room class in bee/resource_structures/room.hpp
	public:
		Background* background;
		bool is_visible;
		bool is_foreground;
		int x, y;
		bool is_horizontal_tile, is_vertical_tile;
		int horizontal_speed, vertical_speed;
		bool is_stretched;
		BackgroundData() {background=NULL;is_visible=false;is_foreground=false;x=0;y=0;is_horizontal_tile=false;is_vertical_tile=false;horizontal_speed=0;vertical_speed=0;is_stretched=false;};
		BackgroundData(Background*, bool, bool, int, int, bool, bool, int, int, bool);
		int init(Background*, bool, bool, int, int, bool, bool, int, int, bool);
};
BackgroundData::BackgroundData(Background* new_background, bool new_is_visible, bool new_is_foreground, int new_x, int new_y, bool new_is_horizontal_tile, bool new_is_vertical_tile, int new_horizontal_speed, int new_vertical_speed, bool new_is_stretched) {
	init(new_background, new_is_visible, new_is_foreground, new_x, new_y, new_is_horizontal_tile, new_is_vertical_tile, new_horizontal_speed, new_vertical_speed, new_is_stretched);
}
int BackgroundData::init(Background* new_background, bool new_is_visible, bool new_is_foreground, int new_x, int new_y, bool new_is_horizontal_tile, bool new_is_vertical_tile, int new_horizontal_speed, int new_vertical_speed, bool new_is_stretched) {
	background = new_background;
	is_visible = new_is_visible;
	is_foreground = new_is_foreground;
	x = new_x;
	y = new_y;
	is_horizontal_tile = new_is_horizontal_tile;
	is_vertical_tile = new_is_vertical_tile;
	horizontal_speed = new_horizontal_speed;
	vertical_speed = new_vertical_speed;
	is_stretched = new_is_stretched;
	return 0;
}

class Background: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string background_path;
		int width, height;
		bool is_tiling;
		int tile_width, tile_height;
		Uint32 animation_time;

		SDL_Texture* texture;
		bool is_loaded;

		int tile_horizontal(SDL_Texture*, SDL_Rect*);
		int tile_vertical(SDL_Texture*, SDL_Rect*);
	public:
		Background();
		Background(std::string, std::string);
		~Background();
		int add_to_resources(std::string);
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
		int set_path(std::string);
		int set_is_tiling(bool);
		int set_tile_width(int);
		int set_tile_height(int);
		int set_time_update();

		int load();
		int free();
		int draw(int, int, BackgroundData*);
};
Background::Background () {
	id = -1;
	reset();
}
Background::Background (std::string new_name, std::string path) {
	id = -1;
	is_loaded = false;
	reset();

	add_to_resources("resources/backgrounds/"+path);
	if (id < 0) {
		std::cerr << "Failed to add background resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
Background::~Background() {
	resource_list.backgrounds.remove_resource(id);
}
int Background::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == background_path) {
			return 1;
		}
		resource_list.backgrounds.remove_resource(id);
		id = -1;
	} else {
		for (auto b : resource_list.backgrounds.resources) {
			if ((b.second != NULL)&&(b.second->get_path() == path)) {
				list_id = b.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = resource_list.backgrounds.add_resource(this);
	}
	resource_list.backgrounds.set_resource(id, this);

	return 0;
}
int Background::reset() {
	if (is_loaded) {
		free();
	}

	name = "";
	background_path = "";
	width = 0;
	height = 0;
	is_tiling = false;
	tile_width = 0;
	tile_height = 0;
	animation_time = 0;

	texture = NULL;
	is_loaded = false;

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
	name = new_name;
	return 0;
}
int Background::set_path(std::string path) {
	add_to_resources("resources/backgrounds/"+path);
	background_path = "resources/backgrounds/"+path;
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
int Background::set_time_update() {
	animation_time = SDL_GetTicks();
	return 0;
}

int Background::load() {
	if (!is_loaded) {
		SDL_Surface* tmp_surface;
		tmp_surface = IMG_Load(background_path.c_str());
		if (tmp_surface == NULL) {
			std::cerr << "Failed to load background " << name << ": " << IMG_GetError() << "\n";
			return 1;
		}

		texture = SDL_CreateTextureFromSurface(game->renderer, tmp_surface);
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
int Background::free() {
	if (is_loaded) {
		SDL_DestroyTexture(texture);
		texture = NULL;
		is_loaded = false;
	}
	return 0;
}
int Background::tile_horizontal(SDL_Texture* t, SDL_Rect* r) {
	if (r->w <= 0) {
		return -1;
	}
	int ox=r->x, i=0;
	while (r->x < game->width) {
		SDL_RenderCopy(game->renderer, t, NULL, r);
		i++;
		r->x += r->w;
	}
	r->x = ox;
	while (r->x+r->w > 0) {
		SDL_RenderCopy(game->renderer, t, NULL, r);
		i++;
		r->x -= r->w;
	}
	r->x = ox;
	return i;
}
int Background::tile_vertical(SDL_Texture* t, SDL_Rect* r) {
	if (r->w <= 0) {
		return -1;
	}
	int oy=r->y, i=0;
	while (r->y < game->height) {
		SDL_RenderCopy(game->renderer, t, NULL, r);
		i++;
		r->y += r->h;
	}
	r->y = oy;
	while (r->y+r->h > 0) {
		SDL_RenderCopy(game->renderer, t, NULL, r);
		i++;
		r->y -= r->h;
	}
	r->y = oy;
	return i;
}
int Background::draw(int x, int y, BackgroundData* b) {
	SDL_Rect rect;
	if (b->is_stretched) {
		rect.x = 0;
		rect.y = 0;
		rect.w = game->width;
		rect.h = game->height;
		SDL_RenderCopy(game->renderer, texture, NULL, &rect);
	} else {
		int dx = b->horizontal_speed*(SDL_GetTicks()-animation_time)/game->fps_goal;
		int dy = b->vertical_speed*(SDL_GetTicks()-animation_time)/game->fps_goal;
		int mx = (width <= 0) ? 0 : game->width - (game->width % width);
		int my = (height <= 0) ? 0 : game->height - (game->height % height);
		if ((mx > 0)&&(my > 0)) {
			dx %= mx;
			dy %= my;
		}
		rect.x = x + dx;
		rect.y = y + dy;
		rect.w = width;
		rect.h = height;

		if (b->is_horizontal_tile && b->is_vertical_tile) {
			for (;rect.y < game->height; rect.y+=rect.h) {
				tile_horizontal(texture, &rect);
			}
			rect.y = y+dy;
			for (;rect.y+height > 0; rect.y-=rect.h) {
				tile_horizontal(texture, &rect);
			}
		} else if (b->is_horizontal_tile) {
			tile_horizontal(texture, &rect);
		} else if (b->is_vertical_tile) {
			tile_vertical(texture, &rect);
		} else {
			SDL_RenderCopy(game->renderer, texture, NULL, &rect);
		}
	}

	return 0;
}

#endif // _BEE_BACKGROUND_H
