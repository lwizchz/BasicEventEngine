/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_BACKGROUND
#define _BEE_BACKGROUND 1

#include "background.hpp"

BEE::BackgroundData::BackgroundData(BEE::Background* new_background, bool new_is_visible, bool new_is_foreground, int new_x, int new_y, bool new_is_horizontal_tile, bool new_is_vertical_tile, int new_horizontal_speed, int new_vertical_speed, bool new_is_stretched) {
	init(new_background, new_is_visible, new_is_foreground, new_x, new_y, new_is_horizontal_tile, new_is_vertical_tile, new_horizontal_speed, new_vertical_speed, new_is_stretched);
}
int BEE::BackgroundData::init(BEE::Background* new_background, bool new_is_visible, bool new_is_foreground, int new_x, int new_y, bool new_is_horizontal_tile, bool new_is_vertical_tile, int new_horizontal_speed, int new_vertical_speed, bool new_is_stretched) {
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

BEE::Background::Background () {
	if (BEE::resource_list->backgrounds.game != NULL) {
		game = BEE::resource_list->backgrounds.game;
	}

	reset();
}
BEE::Background::Background (std::string new_name, std::string path) {
	reset();

	add_to_resources("resources/backgrounds/"+path);
	if (id < 0) {
		std::cerr << "Failed to add background resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
BEE::Background::~Background() {
	free();
	BEE::resource_list->backgrounds.remove_resource(id);
}
int BEE::Background::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == background_path) {
			return 1;
		}
		BEE::resource_list->backgrounds.remove_resource(id);
		id = -1;
	} else {
		for (auto b : BEE::resource_list->backgrounds.resources) {
			if ((b.second != NULL)&&(b.second->get_path() == path)) {
				list_id = b.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = BEE::resource_list->backgrounds.add_resource(this);
	}
	BEE::resource_list->backgrounds.set_resource(id, this);

	if (BEE::resource_list->backgrounds.game != NULL) {
		game = BEE::resource_list->backgrounds.game;
	}

	return 0;
}
int BEE::Background::reset() {
	free();

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
int BEE::Background::print() {
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
int BEE::Background::get_id() {
	return id;
}
std::string BEE::Background::get_name() {
	return name;
}
std::string BEE::Background::get_path() {
	return background_path;
}
int BEE::Background::get_width() {
	return width;
}
int BEE::Background::get_height() {
	return height;
}
bool BEE::Background::get_is_tiling() {
	return is_tiling;
}
int BEE::Background::get_tile_width() {
	return tile_width;
}
int BEE::Background::get_tile_height() {
	return tile_height;
}
bool BEE::Background::get_is_loaded() {
	return is_loaded;
}
SDL_Texture* BEE::Background::get_texture() {
	return texture;
}

int BEE::Background::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int BEE::Background::set_path(std::string path) {
	add_to_resources("resources/backgrounds/"+path);
	background_path = "resources/backgrounds/"+path;
	return 0;
}
int BEE::Background::set_is_tiling(bool new_is_tiling) {
	is_tiling = new_is_tiling;
	return 0;
}
int BEE::Background::set_tile_width(int new_tile_width) {
	tile_width = new_tile_width;
	return 0;
}
int BEE::Background::set_tile_height(int new_tile_height) {
	tile_height = new_tile_height;
	return 0;
}
int BEE::Background::set_time_update() {
	animation_time = SDL_GetTicks();
	return 0;
}

int BEE::Background::load_from_surface(SDL_Surface* tmp_surface) {
	if (!is_loaded) {
		if (game->options->is_opengl) {
			width = tmp_surface->w;
			height = tmp_surface->h;

			GLfloat texcoords[] = {
				0.0, 0.0,
				(GLfloat)width, 0.0,
				(GLfloat)width, 1.0,
				0.0, 1.0,
			};
			glGenBuffers(1, &vbo_texcoords);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
			glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

			GLfloat vertices[] = {
				0.0, 0.0,
				(GLfloat)width, 0.0,
				(GLfloat)width, (GLfloat)height,
				0.0, (GLfloat)height,
			};
			glGenBuffers(1, &vbo_vertices);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			GLushort elements[] = {
				0, 1, 2,
				2, 3, 0,
			};
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

			glGenTextures(1, &gl_texture);
			glBindTexture(GL_TEXTURE_2D, gl_texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				tmp_surface->pixels
			);

			is_loaded = true;
			has_draw_failed = false;
		} else {
			texture = SDL_CreateTextureFromSurface(game->renderer, tmp_surface);
			if (texture == NULL) {
				std::cerr << "Failed to create texture from surface: " << SDL_GetError() << "\n";
				return 1;
			}

			SDL_QueryTexture(texture, NULL, NULL, &width, &height);

			is_loaded = true;
			has_draw_failed = false;
		}
	} else {
		std::cerr << "Failed to load background from surface because it has already been loaded\n";
		return 1;
	}

	return 0;
}
int BEE::Background::load() {
	if (!is_loaded) {
		SDL_Surface* tmp_surface;
		tmp_surface = IMG_Load(background_path.c_str());
		if (tmp_surface == NULL) {
			std::cerr << "Failed to load background " << name << ": " << IMG_GetError() << "\n";
			return 1;
		}

		load_from_surface(tmp_surface);
		SDL_FreeSurface(tmp_surface);
	}
	return 0;
}
int BEE::Background::free() {
	if (is_loaded) {
		if (game->options->is_opengl) {
			glDeleteBuffers(1, &vbo_vertices);
			glDeleteBuffers(1, &vbo_texcoords);
			glDeleteBuffers(1, &ibo);
			glDeleteTextures(1, &gl_texture);
		} else {
			SDL_DestroyTexture(texture);
			texture = NULL;
		}
		is_loaded = false;
	}
	return 0;
}
int BEE::Background::draw_internal(SDL_Rect* src, SDL_Rect* dest) {
	if (game->options->is_opengl) {
		glm::mat4 model = glm::translate(model, glm::vec3(dest->x, dest->y, 0.0));
		glUniformMatrix4fv(game->model_location, 1, GL_FALSE, glm::value_ptr(model));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(game->texture_location, 0);
		glBindTexture(GL_TEXTURE_2D, gl_texture);

		glEnableVertexAttribArray(game->vertex_location);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(
			game->vertex_location,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);

		glEnableVertexAttribArray(game->fragment_location);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glVertexAttribPointer(
			game->fragment_location,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(game->vertex_location);
		glDisableVertexAttribArray(game->fragment_location);
	} else {
		SDL_RenderCopy(game->renderer, texture, src, dest);
	}
	return 0;
}
int BEE::Background::tile_horizontal(SDL_Rect* r) {
	if (r->w <= 0) {
		return -1;
	}

	SDL_Rect src = {0, 0, r->w, r->h};
	int ox=r->x, ow=r->w, oh=r->h, i=0;

	if (r->y + r->h > game->get_room_height()) {
		src.h = game->get_room_height() - r->y;
		r->h = game->get_room_height() - r->y;
	}

	while (r->x < game->get_room_width()) {
		if (r->x + src.w > game->get_room_width()) {
			src.w = game->get_room_width() - r->x;
			r->w = game->get_room_width() - r->x;
		}

		draw_internal(&src, r);
		i++;
		src.w = ow;
		r->w = ow;
		r->x += r->w;
	}
	r->x = ox;
	while (r->x+r->w > 0) {
		draw_internal(&src, r);
		i++;
		r->x -= r->w;
	}
	r->x = ox;
	r->h = oh;
	src.h = oh;

	return i;
}
int BEE::Background::tile_vertical(SDL_Rect* r) {
	if (r->w <= 0) {
		return -1;
	}

	SDL_Rect src = {0, 0, r->w, r->h};
	int oy=r->y, oh=r->h, ow=r->w, i=0;

	if (r->x + src.w > game->get_room_width()) {
		src.w = game->get_room_width() - r->x;
		r->w = game->get_room_width() - r->x;
	}

	while (r->y < game->get_room_height()) {
		if (r->y + src.h > game->get_room_height()) {
			src.h = game->get_room_height() - r->y;
			r->h = game->get_room_height() - r->y;
		}

		draw_internal(&src, r);
		i++;
		src.h = oh;
		r->h = oh;
		r->y += oh;
	}
	r->y = oy - r->h;
	while (r->y+r->h > 0) {
		draw_internal(&src, r);
		i++;
		r->y -= oh;
	}
	r->y = oy;
	r->w = ow;
	src.w = ow;

	return i;
}
int BEE::Background::draw(int x, int y, BackgroundData* b) {
	SDL_Rect rect;
	if (b->is_stretched) {
		rect.x = 0;
		rect.y = 0;
		rect.w = game->get_room_width();
		rect.h = game->get_room_height();
		draw_internal(NULL, &rect);
	} else {
		int dx = b->horizontal_speed*(SDL_GetTicks()-animation_time)/game->fps_goal;
		int dy = b->vertical_speed*(SDL_GetTicks()-animation_time)/game->fps_goal;
		int mx = (width <= 0) ? 0 : game->get_room_width() - (game->get_room_width() % width);
		int my = (height <= 0) ? 0 : game->get_room_height() - (game->get_room_height() % height);
		if ((mx > 0)&&(my > 0)) {
			dx %= mx;
			dy %= my;
		}
		rect.x = x + dx;
		rect.y = y + dy;
		rect.w = width;
		rect.h = height;

		if (b->is_horizontal_tile && b->is_vertical_tile) {
			for (;rect.y < game->get_room_height(); rect.y+=rect.h) {
				tile_horizontal(&rect);
			}
			rect.y = y+dy;
			for (;rect.y+height > 0; rect.y-=rect.h) {
				tile_horizontal(&rect);
			}
		} else if (b->is_horizontal_tile) {
			tile_horizontal(&rect);
		} else if (b->is_vertical_tile) {
			tile_vertical(&rect);
		} else {
			draw_internal(NULL, &rect);
		}
	}

	return 0;
}
int BEE::Background::set_as_target(int w, int h) {
	if (is_loaded) {
		free();
	}

	texture = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
	if (texture == NULL) {
		std::cerr << "Failed to create a blank texture: " << SDL_GetError() << "\n";
		return 1;
	}

	width = w;
	height = h;

	SDL_SetRenderTarget(game->renderer, texture);

	is_loaded = true;

	return 0;
}
int BEE::Background::set_as_target() {
	return set_as_target(game->get_width(), game->get_height());
}

#endif // _BEE_BACKGROUND
