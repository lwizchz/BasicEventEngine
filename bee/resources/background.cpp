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
	if (BEE::resource_list->backgrounds.game != nullptr) {
		game = BEE::resource_list->backgrounds.game;
	}

	reset();
}
BEE::Background::Background (std::string new_name, std::string new_path) {
	reset();

	add_to_resources();
	if (id < 0) {
		game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to add background resource: \"" + new_name + "\" from " + new_path);
		throw(-1);
	}

	set_name(new_name);
	set_path(new_path);
}
BEE::Background::~Background() {
	this->free();
	BEE::resource_list->backgrounds.remove_resource(id);
}
int BEE::Background::add_to_resources() {
	if (id < 0) { // If the resource needs to be added to the resource list
		id = BEE::resource_list->backgrounds.add_resource(this); // Add the resource and get the new id
	}

	// Get the list's engine pointer if it's not nullptr
	if (BEE::resource_list->backgrounds.game != nullptr) {
		game = BEE::resource_list->backgrounds.game;
	}

	return 0; // Return 0 on success
}
int BEE::Background::reset() {
	this->free();

	name = "";
	path = "";
	width = 0;
	height = 0;
	animation_time = 0;

	texture = nullptr;
	is_loaded = false;

	return 0;
}
int BEE::Background::print() {
	std::stringstream s;
	s <<
	"Background { "
	"\n	id              " << id <<
	"\n	name            " << name <<
	"\n	path            " << path <<
	"\n	width           " << width <<
	"\n	height          " << height <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

	return 0;
}
int BEE::Background::get_id() {
	return id;
}
std::string BEE::Background::get_name() {
	return name;
}
std::string BEE::Background::get_path() {
	return path;
}
int BEE::Background::get_width() {
	return width;
}
int BEE::Background::get_height() {
	return height;
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
int BEE::Background::set_path(std::string new_path) {
	path = "resources/backgrounds/"+new_path;
	return 0;
}
int BEE::Background::set_time_update() {
	animation_time = SDL_GetTicks();
	return 0;
}

int BEE::Background::load_from_surface(SDL_Surface* tmp_surface) {
	if (!is_loaded) {
		if (game->options->renderer_type != BEE_RENDERER_SDL) {
			width = tmp_surface->w;
			height = tmp_surface->h;

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			GLfloat texcoords[] = {
				0.0, 0.0,
				1.0, 0.0,
				1.0, 1.0,
				0.0, 1.0,
			};
			glGenBuffers(1, &vbo_texcoords);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
			glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

			GLfloat vertices[] = {
				0.0,            0.0,
				(GLfloat)width, 0.0,
				(GLfloat)width, (GLfloat)height,
				0.0,            (GLfloat)height,
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

			glBindVertexArray(0);

			is_loaded = true;
			has_draw_failed = false;
		} else {
			texture = SDL_CreateTextureFromSurface(game->renderer, tmp_surface);
			if (texture == nullptr) {
				game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to create texture from surface: " + get_sdl_error());
				return 1;
			}

			SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

			is_loaded = true;
			has_draw_failed = false;
		}
	} else {
		game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to load background from surface because it has already been loaded");
		return 1;
	}

	return 0;
}
int BEE::Background::load() {
	if (!is_loaded) {
		SDL_Surface* tmp_surface;
		tmp_surface = IMG_Load(path.c_str());
		if (tmp_surface == nullptr) {
			game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to load background " + name + ": " + IMG_GetError());
			return 1;
		}

		load_from_surface(tmp_surface);
		SDL_FreeSurface(tmp_surface);
	}
	return 0;
}
int BEE::Background::free() {
	if (is_loaded) {
		if (game->options->renderer_type != BEE_RENDERER_SDL) {
			glDeleteBuffers(1, &vbo_vertices);
			glDeleteBuffers(1, &vbo_texcoords);

			glDeleteBuffers(1, &ibo);
			glDeleteTextures(1, &gl_texture);

			glDeleteVertexArrays(1, &vao);
		} else {
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}
		is_loaded = false;
	}
	return 0;
}
int BEE::Background::draw_internal(const SDL_Rect* src, const SDL_Rect* dest) const {
	if (game->options->renderer_type != BEE_RENDERER_SDL) {
		glBindVertexArray(vao);

		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3((float)dest->w/width, (float)dest->h/height, 1.0f));
		model = glm::translate(model, glm::vec3((float)dest->x, (float)dest->y, 0.0f));
		glUniformMatrix4fv(game->model_location, 1, GL_FALSE, glm::value_ptr(model));

		glUniform1i(game->texture_location, 0);
		glBindTexture(GL_TEXTURE_2D, gl_texture);

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

		glDisableVertexAttribArray(game->fragment_location);

		glUniformMatrix4fv(game->model_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(game->rotation_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(game->flip_location, 0);

		glBindVertexArray(0);
	} else {
		SDL_RenderCopy(game->renderer, texture, src, dest);
	}
	return 0;
}
int BEE::Background::tile_horizontal(const SDL_Rect* r) const {
	if (r->w <= 0) {
		return -1;
	}

	SDL_Rect src = {0, 0, r->w, r->h};
	SDL_Rect dest = {r->x, r->y, r->w, r->h};
	int i=0;

	while (dest.x < game->get_room_width()) {
		draw_internal(&src, &dest);
		i++;
		src.w = r->w;
		dest.w = r->w;
		dest.x += dest.w;
	}
	dest.x = r->x;
	while (dest.x + dest.w > 0) {
		draw_internal(&src, &dest);
		i++;
		dest.x -= dest.w;
	}

	return i;
}
int BEE::Background::tile_vertical(const SDL_Rect* r) const {
	if (r->w <= 0) {
		return -1;
	}

	SDL_Rect src = {0, 0, r->w, r->h};
	SDL_Rect dest = {r->x, r->y, r->w, r->h};
	int i=0;

	while (dest.y < game->get_room_height()) {
		draw_internal(&src, &dest);
		i++;
		src.h = r->h;
		dest.h = r->h;
		dest.y += r->h;
	}
	dest.y = r->y - dest.h;
	while (dest.y + dest.h > 0) {
		draw_internal(&src, &dest);
		i++;
		dest.y -= r->h;
	}

	return i;
}
int BEE::Background::draw(int x, int y, BackgroundData* b) {
	SDL_Rect rect;
	if (b->is_stretched) {
		rect.x = 0;
		rect.y = 0;
		rect.w = game->get_room_width();
		rect.h = game->get_room_height();
		draw_internal(nullptr, &rect);
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

		if ((rect.w > 0)&&(rect.h > 0)) {
			if (b->is_horizontal_tile && b->is_vertical_tile) {
				while (rect.y-rect.h < game->get_room_height()) {
					tile_horizontal(&rect);
					rect.y += rect.h;
				}
				rect.y = y + dy - rect.h;
				while (rect.y+rect.h > 0) {
					tile_horizontal(&rect);
					rect.y -= rect.h;
				}
			} else if (b->is_horizontal_tile) {
				tile_horizontal(&rect);
			} else if (b->is_vertical_tile) {
				tile_vertical(&rect);
			} else {
				draw_internal(nullptr, &rect);
			}
		}
	}

	return 0;
}
int BEE::Background::set_as_target(int w, int h) {
	if (is_loaded) {
		this->free();
	}

	texture = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
	if (texture == nullptr) {
		game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to create a blank texture: " + get_sdl_error());
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
