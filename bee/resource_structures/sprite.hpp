/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SPRITE_H
#define _BEE_SPRITE_H 1

#include <vector>

class BEE::RGBA {
	public:
		Uint8 r, g, b, a;
};
BEE::RGBA c_aqua	= {0, 255, 255, 255};
BEE::RGBA c_black	= {0, 0, 0, 255};
BEE::RGBA c_blue	= {0, 0, 255, 255};
BEE::RGBA c_dkgray	= {64, 64, 64, 255};
BEE::RGBA c_fuchsia	= {255, 0, 255, 255};
BEE::RGBA c_gray	= {128, 128, 128, 255};
BEE::RGBA c_green	= {0, 255, 0, 255};
//BEE::RGBA c_lime	= {255, 255, 255, 255};
BEE::RGBA c_ltgray	= {192, 192, 192, 255};
//BEE::RGBA c_maroon	= {255, 255, 255, 255};
//BEE::RGBA c_navy	= {255, 255, 255, 255};
//BEE::RGBA c_olive	= {255, 255, 255, 255};
BEE::RGBA c_orange	= {255, 128, 0, 255};
BEE::RGBA c_purple	= {128, 0, 255, 255};
BEE::RGBA c_red	= {255, 0, 0, 255};
//BEE::RGBA c_silver	= {255, 255, 255, 255};
//BEE::RGBA c_teal	= {255, 255, 255, 255};
BEE::RGBA c_white	= {255, 255, 255, 255};
BEE::RGBA c_yellow	= {255, 255, 0, 255};

class BEE::Sprite: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string image_path;
		int width, height;
		int subimage_amount, subimage_width;
		double speed, alpha;
		bool is_animated;
		int origin_x, origin_y;

		SDL_Texture* texture;
		bool is_loaded = false;
		std::vector<SDL_Rect> subimages;
		SDL_Rect srect, drect;
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
		double get_speed();
		double get_alpha();
		int get_origin_x();
		int get_origin_y();
		SDL_Texture* get_texture();
		bool get_is_loaded();

		int set_name(std::string);
		int set_path(std::string);
		int set_subimage_amount(int, int);
		int set_speed(double);
		int set_alpha(double);
		int set_origin_x(int);
		int set_origin_y(int);
		int set_origin_xy(int, int);
		int set_origin_center();

		int load();
		int free();
		int draw(int, int, Uint32, int, int, double, RGBA);
		int draw(int, int, Uint32);
		int draw(int, int, Uint32, int, int);
		int draw(int, int, Uint32, double);
		int draw(int, int, Uint32, RGBA);
		int set_as_target();
};
BEE::Sprite::Sprite () {
	reset();
}
BEE::Sprite::Sprite (std::string new_name, std::string path) {
	reset();

	add_to_resources("resources/sprites/"+path);
	if (id < 0) {
		std::cerr << "Failed to add sprite resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
BEE::Sprite::~Sprite() {
	free();
	BEE::resource_list.sprites.remove_resource(id);
}
int BEE::Sprite::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == image_path) {
			return 1;
		}
		BEE::resource_list.sprites.remove_resource(id);
		id = -1;
	} else {
		for (auto s : BEE::resource_list.sprites.resources) {
			if ((s.second != NULL)&&(s.second->get_path() == path)) {
				list_id = s.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = BEE::resource_list.sprites.add_resource(this);
	}
	BEE::resource_list.sprites.set_resource(id, this);

	return 0;
}
int BEE::Sprite::reset() {
	if (is_loaded) {
		free();
	}

	name = "";
	image_path = "";
	width = 0;
	height = 0;
	subimage_amount = 1;
	subimage_width = 0;
	speed = 0.0;
	alpha = 1.0;
	is_animated = false;
	origin_x = 0;
	origin_y = 0;

	texture = NULL;
	is_loaded = false;
	subimages.clear();

	return 0;
}
int BEE::Sprite::print() {
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
int BEE::Sprite::get_id() {
	return id;
}
std::string BEE::Sprite::get_name() {
	return name;
}
std::string BEE::Sprite::get_path() {
	return image_path;
}
int BEE::Sprite::get_width() {
	return width;
}
int BEE::Sprite::get_height() {
	return height;
}
int BEE::Sprite::get_subimage_amount() {
	return subimage_amount;
}
int BEE::Sprite::get_subimage_width() {
	return subimage_width;
}
double BEE::Sprite::get_speed() {
	return speed;
}
double BEE::Sprite::get_alpha() {
	return alpha;
}
int BEE::Sprite::get_origin_x() {
	return origin_x;
}
int BEE::Sprite::get_origin_y() {
	return origin_y;
}
SDL_Texture* BEE::Sprite::get_texture() {
	return texture;
}
bool BEE::Sprite::get_is_loaded() {
	return is_loaded;
}
int BEE::Sprite::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int BEE::Sprite::set_path(std::string path) {
	add_to_resources("resources/sprites/"+path);
	image_path = "resources/sprites/"+path;
	return 0;
}
int BEE::Sprite::set_subimage_amount(int new_subimage_amount, int new_subimage_width) {
	subimage_amount = new_subimage_amount;
	subimage_width = new_subimage_width;
	for (int i=0; i<subimage_amount; i++) {
		subimages.push_back({i*subimage_width, 0, subimage_width, 0});
	}
	return 0;
}
int BEE::Sprite::set_speed(double new_speed) {
	speed = new_speed;
	return 0;
}
int BEE::Sprite::set_alpha(double new_alpha) {
	alpha = new_alpha;
	SDL_SetTextureAlphaMod(texture, alpha*255);
	return 0;
}
int BEE::Sprite::set_origin_x(int new_origin_x) {
	origin_x = new_origin_x;
	return 0;
}
int BEE::Sprite::set_origin_y(int new_origin_y) {
	origin_y = new_origin_y;
	return 0;
}
int BEE::Sprite::set_origin_xy(int new_origin_x, int new_origin_y) {
	set_origin_x(new_origin_x);
	set_origin_y(new_origin_y);
	return 0;
}
int BEE::Sprite::set_origin_center() {
	set_origin_x(width/2);
	set_origin_y(height/2);
	return 0;
}

int BEE::Sprite::load() {
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
int BEE::Sprite::free() {
	if (is_loaded) {
		SDL_DestroyTexture(texture);
		texture = NULL;
		is_loaded = false;
	}
	return 0;
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, int w, int h, double angle, RGBA new_color) {
	int current_subimage = (int)round(speed*(SDL_GetTicks()-subimage_time)/game->fps_goal) % subimage_amount;
	if (current_subimage == 0) {
		is_animated = true;
	}

	SDL_SetTextureColorMod(texture, new_color.r, new_color.g, new_color.b);

	drect.x = x;
	drect.y = y;
	if ((w >= 0)&&(h >= 0)) {
		drect.w = w;
		drect.h = h;
	} else if (!subimages.empty()) {
		drect.w = subimage_width;
		drect.h = height;
	} else {
		drect.w = width;
		drect.h = height;
	}

	if (!subimages.empty()) {
		srect.x = subimages[current_subimage].x;
		srect.y = 0;
		srect.w = subimages[current_subimage].w;
		srect.h = height;

		SDL_RenderCopyEx(game->renderer, texture, &srect, &drect, angle, NULL, SDL_FLIP_NONE);
	} else {
		SDL_RenderCopyEx(game->renderer, texture, NULL, &drect, angle, NULL, SDL_FLIP_NONE);
	}

	if ((is_animated)&&(current_subimage == subimage_amount-1)) {
		game->animation_end(this);
		is_animated = false;
	}

	return 0;
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time) {
	return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255});
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, int w, int h) {
	return draw(x, y, subimage_time, w, h, 0.0, {255, 255, 255, 255});
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, double angle) {
	return draw(x, y, subimage_time, -1, -1, angle, {255, 255, 255, 255});
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, RGBA color) {
	return draw(x, y, subimage_time, -1, -1, 0.0, color);
}
int BEE::Sprite::set_as_target() {
	if (is_loaded) {
		free();
	}

	texture = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, game->get_width(), game->get_height());
	if (texture == NULL) {
		std::cerr << "Failed to create a blank texture: " << SDL_GetError() << "\n";
		return 1;
	}

	width = game->get_width();
	height = game->get_height();
	set_subimage_amount(1, width);

	SDL_SetRenderTarget(game->renderer, texture);

	is_loaded = true;

	return 0;
}

#endif // _BEE_SPRITE_H
