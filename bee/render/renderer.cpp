/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDERER
#define _BEE_RENDERER 1

#include "renderer.hpp" // Include the engine headers

BEE::Renderer::Renderer(BEE* new_game) :
	game(new_game),

	window(nullptr),
	sdl_renderer(nullptr),
	context(nullptr),

	render_is_3d(false),
	render_camera(nullptr),
	projection_cache(nullptr)
{}
BEE::Renderer::~Renderer() {
	if (render_camera != nullptr) {
		delete render_camera;
		render_camera = nullptr;
	}
	if (projection_cache != nullptr) {
		delete projection_cache;
		projection_cache = nullptr;
	}

	if (context != nullptr) {
		opengl_close();
	}
	if (sdl_renderer != nullptr) {
		sdl_renderer_close();
	}
	if (window != nullptr) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

int BEE::Renderer::opengl_init() {
	context = SDL_GL_CreateContext(window);
	if (context == nullptr) {
		throw "Couldn't create OpenGL context: " + get_sdl_error() + "\n";
	}

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK) {
		throw "Couldn't initialize GLEW: " + std::string((const char*)glewGetErrorString(glew_error)) + "\n";
	}

	if (game->options->is_vsync_enabled) {
		SDL_GL_SetSwapInterval(1);
	} else {
		SDL_GL_SetSwapInterval(0);
	}

	program = glCreateProgram();

	// Get shader filenames
	const std::string vs_fn_default = "bee/render/shader/default.vertex.glsl";
	const std::string vs_fn_user = "resources/vertex.glsl";
	std::string vs_fn (vs_fn_default);
	if (file_exists(vs_fn_user)) {
		vs_fn = vs_fn_user;
	}
	const std::string gs_fn_default = "bee/render/shader/default.geometry.glsl";
	const std::string gs_fn_user = "resources/geometry.glsl";
	std::string gs_fn (gs_fn_default);
	if (file_exists(gs_fn_user)) {
		gs_fn = gs_fn_user;
	}
	const std::string fs_fn_default = "bee/render/shader/default.fragment.glsl";
	const std::string fs_fn_user = "resources/fragment.glsl";
	const std::string fs_fn_basic_default = "bee/render/shader/basic.fragment.glsl";
	const std::string fs_fn_basic_user = "resources/basic.fragment.glsl";
	std::string fs_fn (fs_fn_default);
	if (game->options->is_basic_shaders_enabled == true) {
		if (file_exists(fs_fn_basic_user)) {
			fs_fn = fs_fn_basic_user;
		}
	} else {
		if (file_exists(fs_fn_user)) {
			fs_fn = fs_fn_user;
		}
	}

	// Compile vertex shader
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vs = file_get_contents(vs_fn);
	vs = opengl_prepend_version(vs);
	const GLchar* vertex_shader_source[] = {vs.c_str()};
	glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);
	GLint is_shader_compiled = GL_FALSE;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (is_shader_compiled != GL_TRUE) {
		game->messenger_send({"engine", "init"}, bee::E_MESSAGE::ERROR, get_shader_error(vertex_shader));
		glDeleteShader(vertex_shader);
		throw std::string("Couldn't compile OpenGL vertex shader: ") + bee_itos(vertex_shader) + "\n";
	}
	glAttachShader(program, vertex_shader);

	// Compile geometry shader
	GLuint geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	std::string gs = file_get_contents(gs_fn);
	gs = opengl_prepend_version(gs);
	const GLchar* geometry_shader_source[] = {gs.c_str()};
	glShaderSource(geometry_shader, 1, geometry_shader_source, nullptr);
	glCompileShader(geometry_shader);
	is_shader_compiled = GL_FALSE;
	glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (is_shader_compiled != GL_TRUE) {
		game->messenger_send({"engine", "init"}, bee::E_MESSAGE::ERROR, get_shader_error(geometry_shader));
		game->handle_messages();
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		throw std::string("Couldn't compile OpenGL geometry shader: ") + bee_itos(geometry_shader) + "\n";
	}
	glAttachShader(program, geometry_shader);

	// Compile fragment shader
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fs = file_get_contents(fs_fn);
	fs = opengl_prepend_version(fs);
	const GLchar* fragment_shader_source[] = {fs.c_str()};
	glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);
	is_shader_compiled = GL_FALSE;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (is_shader_compiled != GL_TRUE) {
		game->messenger_send({"engine", "init"}, bee::E_MESSAGE::ERROR, get_shader_error(fragment_shader));
		game->handle_messages();
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't compile OpenGL fragment shader: ") + bee_itos(fragment_shader) + "\n";
	}
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);
	GLint is_program_linked = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &is_program_linked);
	if (is_program_linked != GL_TRUE) {
		game->messenger_send({"engine", "init"}, bee::E_MESSAGE::ERROR, get_program_error(program));
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't link OpenGL program: ") + bee_itos(program) + "\n";
	}

	vertex_location = glGetAttribLocation(program, "v_position");
	if (vertex_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'v_position' in the vertex shader\n");
	}
	/*normal_location = glGetAttribLocation(program, "v_normal");
	if (normal_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'v_normal' in the vertex shader\n");
	}*/
	fragment_location = glGetAttribLocation(program, "v_texcoord");
	if (fragment_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'v_texcoord' in the vertex shader\n");
	}

	projection_location = glGetUniformLocation(program, "projection");
	if (projection_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'projection' in the vertex shader\n");
	}
	view_location = glGetUniformLocation(program, "view");
	if (view_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'view' in the vertex shader\n");
	}
	model_location = glGetUniformLocation(program, "model");
	if (model_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'model' in the vertex shader\n");
	}
	port_location = glGetUniformLocation(program, "port");
	if (port_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'port' in the vertex shader\n");
	}

	rotation_location = glGetUniformLocation(program, "rotation");
	if (rotation_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'rotation' in the geometry shader\n");
	}

	texture_location = glGetUniformLocation(program, "f_texture");
	if ((texture_location == -1)&&(!game->options->is_basic_shaders_enabled)) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'f_texture' in the fragment shader\n");
	}
	colorize_location = glGetUniformLocation(program, "colorize");
	if ((colorize_location == -1)&&(!game->options->is_basic_shaders_enabled)) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'colorize' in the fragment shader\n");
	}
	primitive_location = glGetUniformLocation(program, "is_primitive");
	if ((primitive_location == -1)&&(!game->options->is_basic_shaders_enabled)) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'is_primitive' in the fragment shader\n");
	}
	flip_location = glGetUniformLocation(program, "flip");
	if ((flip_location == -1)&&(!game->options->is_basic_shaders_enabled)) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'flip' in the fragment shader\n");
	}

	is_lightable_location = glGetUniformLocation(program, "is_lightable");
	if ((is_lightable_location == -1)&&(!game->options->is_basic_shaders_enabled)) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'is_lightable' in the fragment shader\n");
	}
	light_amount_location = glGetUniformLocation(program, "light_amount");
	if ((light_amount_location == -1)&&(!game->options->is_basic_shaders_enabled)) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'light_amount' in the fragment shader\n");
	}
	for (size_t i=0; i<BEE_MAX_LIGHTS; i++) {
		lighting_location[i].type = glGetUniformLocation(program, std::string("lighting[" + bee_itos(i) + "].type").c_str());
		lighting_location[i].position = glGetUniformLocation(program, std::string("lighting[" + bee_itos(i) + "].position").c_str());
		lighting_location[i].direction = glGetUniformLocation(program, std::string("lighting[" + bee_itos(i) + "].direction").c_str());
		lighting_location[i].attenuation = glGetUniformLocation(program, std::string("lighting[" + bee_itos(i) + "].attenuation").c_str());
		lighting_location[i].color = glGetUniformLocation(program, std::string("lighting[" + bee_itos(i) + "].color").c_str());
	}

	lightable_amount_location = glGetUniformLocation(program, "lightable_amount");
	if ((lightable_amount_location == -1)&&(!game->options->is_basic_shaders_enabled)) {
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'lightable_amount' in the fragment shader\n");
	}
	for (size_t i=0; i<BEE_MAX_LIGHTABLES; i++) {
		lightable_location[i].position = glGetUniformLocation(program, std::string("lightables[" + bee_itos(i) + "].position").c_str());
		lightable_location[i].vertex_amount = glGetUniformLocation(program, std::string("lightables[" + bee_itos(i) + "].vertex_amount").c_str());
		for (size_t e=0; e<BEE_MAX_MASK_VERTICES; e++) {
			lightable_location[i].mask[e] = glGetUniformLocation(program, std::string("lightables[" + bee_itos(i) + "].mask[" + bee_itos(e) + "]").c_str());
		}
	}

	game->draw_set_color({255, 255, 255, 255});
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDeleteShader(vertex_shader);
	glDeleteShader(geometry_shader);
	glDeleteShader(fragment_shader);

	if (game->options->renderer_type == bee::E_RENDERER::OPENGL4) {
		game->messenger_send({"engine", "init"}, bee::E_MESSAGE::INFO, "Now rendering with OpenGL 4.1");
	} else if (game->options->renderer_type == bee::E_RENDERER::OPENGL3) {
		game->messenger_send({"engine", "init"}, bee::E_MESSAGE::INFO, "Now rendering with OpenGL 3.3");
	}

	int va = 0, vi = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &va);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &vi);
	game->messenger_send({"engine", "init"}, bee::E_MESSAGE::INFO, "GLversion: " + bee_itos(va) + "." + bee_itos(vi));

	projection_cache = new glm::mat4(1.0f);

	// Generate the triangle buffers for primitive drawing
	glGenBuffers(1, &triangle_vao);
	glGenBuffers(1, &triangle_vbo);

	GLushort elements[] = {
		0, 1, 2,
	};
	glGenBuffers(1, &triangle_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	return 0;
}
int BEE::Renderer::opengl_close() {
	glDeleteBuffers(1, &triangle_vao);
	glDeleteBuffers(1, &triangle_vbo);
	glDeleteBuffers(1, &triangle_ibo);

	delete projection_cache;

	if (program != 0) {
		glDeleteProgram(program);
		program = 0;
	}

	if (context != nullptr) {
		SDL_GL_DeleteContext(context);
		context = nullptr;
	}

	return 0;
}
std::string BEE::Renderer::opengl_prepend_version(const std::string& shader) {
	switch (game->options->renderer_type) {
		case bee::E_RENDERER::OPENGL4: {
			if (GL_VERSION_4_1) {
				return "#version 410 core\n" + shader;
			}
		}
		case bee::E_RENDERER::OPENGL3: {
			if (GL_VERSION_3_3) {
				return "#version 330 core\n" + shader;
			}
		}
		case bee::E_RENDERER::SDL:
		default: {
			game->options->renderer_type = bee::E_RENDERER::SDL;
			return shader;
		}
	}
}
int BEE::Renderer::sdl_renderer_init() {
	int renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
	if (game->options->is_vsync_enabled) {
		renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	sdl_renderer = SDL_CreateRenderer(window, -1, renderer_flags);
	if (sdl_renderer == nullptr) {
		throw "Couldn't create SDL renderer: " + get_sdl_error() + "\n";
	}

	SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
	SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);

	game->messenger_send({"engine", "init"}, bee::E_MESSAGE::INFO, "Now rendering with SDL2");

	return 0;
}
int BEE::Renderer::sdl_renderer_close() {
	if (sdl_renderer != nullptr) {
		SDL_DestroyRenderer(sdl_renderer);
		sdl_renderer = nullptr;
	}

	return 0;
}

int BEE::Renderer::render_clear() {
	game->draw_set_color(*(game->color));
	if (game->options->renderer_type != bee::E_RENDERER::SDL) {
		if (target > 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, target);
		}

		glUseProgram(program);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	} else {
		SDL_RenderClear(sdl_renderer);
	}
	return 0;
}
int BEE::Renderer::render() const {
	if (game->options->renderer_type != bee::E_RENDERER::SDL) {
		SDL_GL_SwapWindow(window);
	} else {
		SDL_RenderPresent(sdl_renderer);
	}
	return 0;
}
int BEE::Renderer::render_reset() {
	if (game->options->renderer_type != bee::E_RENDERER::SDL) {
		opengl_close();
		opengl_init();
	} else {
		sdl_renderer_close();
		sdl_renderer_init();
	}

	// Reload sprite and background textures
	Sprite* s;
	for (size_t i=0; i<resource_list->sprites.get_amount(); i++) {
		if (get_sprite(i) != nullptr) {
			s = get_sprite(i);
			if (s->get_is_loaded()) {
				s->free();
				s->load();
			}
		}
	}
	Background* b;
	for (size_t i=0; i<resource_list->backgrounds.get_amount(); i++) {
		if (get_background(i) != nullptr) {
			b = get_background(i);
			if (b->get_is_loaded()) {
				b->free();
				b->load();
			}
		}
	}

	return 0;
}

#endif // _BEE_RENDERER
