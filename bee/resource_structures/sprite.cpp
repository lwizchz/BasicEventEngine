/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SPRITE
#define _BEE_SPRITE 1

#include "sprite.hpp"

BEE::Sprite::Sprite () {
	if (BEE::resource_list->sprites.game != NULL) {
		game = BEE::resource_list->sprites.game;
	}

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
	BEE::resource_list->sprites.remove_resource(id);
}
int BEE::Sprite::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == image_path) {
			return 1;
		}
		BEE::resource_list->sprites.remove_resource(id);
		id = -1;
	} else {
		for (auto s : BEE::resource_list->sprites.resources) {
			if ((s.second != NULL)&&(s.second->get_path() == path)) {
				list_id = s.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = BEE::resource_list->sprites.add_resource(this);
	}
	BEE::resource_list->sprites.set_resource(id, this);

	if (BEE::resource_list->sprites.game != NULL) {
		game = BEE::resource_list->sprites.game;
	}

	return 0;
}
int BEE::Sprite::reset() {
	free();

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
	has_draw_failed = false;

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
bool BEE::Sprite::get_is_animated() {
	return is_animated;
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
	subimages.clear();
	subimage_amount = new_subimage_amount;
	subimage_width = new_subimage_width;

	for (int i=0; i<subimage_amount; i++) {
		subimages.push_back({i*subimage_width, 0, subimage_width, 0});
	}

	if (game->options->is_opengl) {
		if (!vbo_texcoords.empty()) {
			for (auto& t : vbo_texcoords) {
				glDeleteBuffers(1, &t);
			}
			vbo_texcoords.clear();
		}

		GLfloat w = subimage_width;
		if (width > 0) {
			w /= width;
		}
		for (int i=0; i<subimage_amount; i++) {
			GLuint new_texcoord;
			GLfloat texcoords[] = {
				w*i, 0.0,
				w*(i+1), 0.0,
				w*(i+1), 1.0,
				w*i, 1.0,
			};
			glGenBuffers(1, &new_texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, new_texcoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

			vbo_texcoords.push_back(new_texcoord);
		}
	}

	return 0;
}
int BEE::Sprite::set_speed(double new_speed) {
	speed = new_speed;
	return 0;
}
int BEE::Sprite::set_alpha(double new_alpha) {
	alpha = new_alpha;
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

int BEE::Sprite::load_from_surface(SDL_Surface* tmp_surface) {
	if (!is_loaded) {
		if (game->options->is_opengl) {
			width = tmp_surface->w;
			height = tmp_surface->h;
			if (subimage_amount <= 1) {
				set_subimage_amount(1, width);
			} else {
				set_subimage_amount(subimage_amount, width/subimage_amount);
			}

			GLfloat vertices[] = {
				0.0, 0.0,
				(GLfloat)subimage_width, 0.0,
				(GLfloat)subimage_width, (GLfloat)height,
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
			if (subimage_amount <= 1) {
				set_subimage_amount(1, width);
			}

			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(texture, alpha*255);

			is_loaded = true;
			has_draw_failed = false;
		}
	} else {
		std::cerr << "Failed to load sprite from surface because it has already been loaded\n";
		return 1;
	}

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

		load_from_surface(tmp_surface);
		SDL_FreeSurface(tmp_surface);
	}
	return 0;
}
int BEE::Sprite::free() {
	if (is_loaded) {
		if (game->options->is_opengl) {
			glDeleteBuffers(1, &vbo_vertices);

			for (auto& t : vbo_texcoords) {
				glDeleteBuffers(1, &t);
			}
			vbo_texcoords.clear();

			glDeleteBuffers(1, &ibo);
			glDeleteTextures(1, &gl_texture);
			glDeleteFramebuffers(1, &framebuffer);
		} else {
			SDL_DestroyTexture(texture);
			texture = NULL;
		}
		is_loaded = false;
	}
	return 0;
}
int BEE::Sprite::draw_subimage(int x, int y, int current_subimage, int w, int h, double angle, RGBA new_color, SDL_RendererFlip flip, bool is_hud) {
	if (!is_loaded) {
		if (!has_draw_failed) {
			std::cerr << "Failed to draw sprite \"" << name << "\" because it is not loaded\n";
			has_draw_failed = true;
		}
		return 1;
	}

	drect.x = x;
	drect.y = y;
	if ((game->get_current_room()->get_is_views_enabled())&&(!is_hud)) {
		if (game->get_current_room()->get_current_view() != NULL) {
			drect.x += game->get_current_room()->get_current_view()->view_x;
			drect.y += game->get_current_room()->get_current_view()->view_y;
		}
	}

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

	if (game->is_on_screen(drect)) {
		if (game->options->is_opengl) {
			if (w <= 0) {
				w = width;
			}
			if (h <= 0) {
				h = height;
			}

			glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(w/width, h/height, 1.0));
			model = glm::translate(model, glm::vec3(drect.x, drect.y, 0.0));
			if (angle != 0.0) {
				model = glm::translate(model, glm::vec3((1+sin(degtorad(angle)))*width, -cos(degtorad(angle))*height, 0.0)); // rotational translation
			}
			model = glm::rotate(model, (float)degtorad(angle), glm::vec3(0.0, 0.0, 1.0));
			glUniformMatrix4fv(game->model_location, 1, GL_FALSE, glm::value_ptr(model));

			glActiveTexture(GL_TEXTURE0);
			glUniform1i(game->texture_location, 0);
			glBindTexture(GL_TEXTURE_2D, gl_texture);

			glm::vec4 color = glm::vec4(new_color.r/255.0, new_color.g/255.0, new_color.b/255.0, new_color.a/255.0);
			glUniform4fv(game->colorize_location, 1, glm::value_ptr(color));

			int f = 0;
			if (flip & SDL_FLIP_HORIZONTAL) {
				f += 1;
			}
			if (flip & SDL_FLIP_VERTICAL) {
				f += 2;
			}
			glUniform1i(game->flip_location, f);

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
			glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords[current_subimage]);
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
			SDL_SetTextureColorMod(texture, new_color.r, new_color.g, new_color.b);
			if (new_color.a == 0) {
				SDL_SetTextureAlphaMod(texture, alpha*255);
			} else {
				SDL_SetTextureAlphaMod(texture, new_color.a);
			}

			if (!subimages.empty()) {
				srect.x = subimages[current_subimage].x;
				srect.y = 0;
				srect.w = subimages[current_subimage].w;
				srect.h = height;

				SDL_RenderCopyEx(game->renderer, texture, &srect, &drect, angle, NULL, flip);
			} else {
				SDL_RenderCopyEx(game->renderer, texture, NULL, &drect, angle, NULL, flip);
			}
		}
	}

	if ((is_animated)&&(current_subimage == subimage_amount-1)) {
		is_animated = false;
	}

	return 0;
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, int w, int h, double angle, RGBA new_color, SDL_RendererFlip flip, bool is_hud) {
	int current_subimage = (int)round(speed*(game->get_ticks()-subimage_time)/game->fps_goal) % subimage_amount;
	if (current_subimage == 0) {
		is_animated = true;
	}

	return draw_subimage(x, y, current_subimage, w, h, angle, new_color, flip, is_hud);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time) {
	return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255}, SDL_FLIP_NONE, false);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, int w, int h) {
	return draw(x, y, subimage_time, w, h, 0.0, {255, 255, 255, 255}, SDL_FLIP_NONE, false);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, double angle) {
	return draw(x, y, subimage_time, -1, -1, angle, {255, 255, 255, 255}, SDL_FLIP_NONE, false);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, RGBA color) {
	return draw(x, y, subimage_time, -1, -1, 0.0, color, SDL_FLIP_NONE, false);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, SDL_RendererFlip flip) {
	return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255}, flip, false);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, bool is_hud) {
	return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255}, SDL_FLIP_NONE, is_hud);
}
int BEE::Sprite::draw_simple(SDL_Rect* source, SDL_Rect* dest) {
	if (!is_loaded) {
		return 1;
	}
	if (game->options->is_opengl) {
		return 2;
	}
	return SDL_RenderCopy(game->renderer, texture, source, dest);
}
int BEE::Sprite::set_as_target(int w, int h) {
	if (is_loaded) {
		free();
	}

	if (game->options->is_opengl) {
		width = w;
		height = h;
		set_subimage_amount(1, width);

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

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

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
			0
		);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gl_texture, 0);
		GLenum buffer[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, buffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			free();
			return 0;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		is_loaded = true;
		has_draw_failed = false;

		return (int)framebuffer;
	} else {
		texture = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
		if (texture == NULL) {
			std::cerr << "Failed to create a blank texture: " << SDL_GetError() << "\n";
			return 1;
		}

		width = w;
		height = h;
		set_subimage_amount(1, width);

		SDL_SetRenderTarget(game->renderer, texture);

		is_loaded = true;
		has_draw_failed = false;
	}

	return 0;
}
int BEE::Sprite::set_as_target() {
	return set_as_target(game->get_width(), game->get_height());
}

#endif // _BEE_SPRITE
