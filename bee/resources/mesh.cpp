/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_MESH
#define _BEE_MESH 1

#include "mesh.hpp"

BEE::Mesh::Mesh () {
	if (BEE::resource_list->meshes.game != nullptr) {
		game = BEE::resource_list->meshes.game;
	}

	reset();
}
BEE::Mesh::Mesh (const std::string& new_name, const std::string& new_path) {
	// Get the list's engine pointer if it's not nullptr
	if (BEE::resource_list->meshes.game != nullptr) {
		game = BEE::resource_list->meshes.game;
	}

	reset();

	add_to_resources();
	if (id < 0) {
		game->messenger_send({"engine", "resource"}, bee::E_MESSAGE::WARNING, "Failed to add mesh resource: \"" + new_name + "\" from " + new_path);
		throw(-1);
	}

	set_name(new_name);
	set_path(new_path);
}
BEE::Mesh::~Mesh() {
	this->free();
	BEE::resource_list->meshes.remove_resource(id);
}
int BEE::Mesh::add_to_resources() {
	if (id < 0) { // If the resource needs to be added to the resource list
		id = BEE::resource_list->meshes.add_resource(this); // Add the resource and get the new id
	}

	return 0;
}
int BEE::Mesh::reset() {
	this->free();

	name = "";
	path = "";

	is_loaded = false;
	has_draw_failed = false;
	vertex_amount = 0;

	return 0;
}
int BEE::Mesh::print() const {
	std::stringstream s;
	s <<
	"Mesh { "
	"\n	id              " << id <<
	"\n	name            " << name <<
	"\n	path            " << path <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, bee::E_MESSAGE::INFO, s.str());

	return 0;
}
int BEE::Mesh::get_id() const {
	return id;
}
std::string BEE::Mesh::get_name() const {
	return name;
}
std::string BEE::Mesh::get_path() const {
	return path;
}

int BEE::Mesh::set_name(const std::string& new_name) {
	name = new_name;
	return 0;
}
int BEE::Mesh::set_path(const std::string& new_path) {
	path = "resources/meshes/"+new_path;
	return 0;
}

int BEE::Mesh::load() {
	if (!is_loaded) {
		if (game->options->renderer_type != bee::E_RENDERER::SDL) {
			scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
			if (scene == nullptr) {
				game->messenger_send({"engine", "mesh"}, bee::E_MESSAGE::WARNING, "Failed to load mesh \"" + name + "\": " + aiGetErrorString());
				return 1;
			}

			mesh = scene->mMeshes[0];
			vertex_amount = mesh->mNumVertices;
			vertices = new float[vertex_amount*3];
			normals = new float[vertex_amount*3];
			uv_array = new float[vertex_amount*2];
			indices = new unsigned int[mesh->mNumFaces*3];

			for (size_t i=0; i<mesh->mNumFaces; i++) {
				const aiFace &face = mesh->mFaces[i];
				if (face.mNumIndices < 3) {
					continue;
				}

				for (size_t e=0; e<3; e++) {
					if (mesh->mTextureCoords[0] != nullptr) {
						aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[e]];
						memcpy(uv_array+face.mIndices[e]*2, &uv, sizeof(float)*2);
					} else {
						uv_array[face.mIndices[e]] = 0;
						uv_array[face.mIndices[e]+1] = 0;
					}

					aiVector3D norm = mesh->mNormals[face.mIndices[e]];
					memcpy(normals+face.mIndices[e]*3, &norm, sizeof(float)*3);

					aiVector3D vert = mesh->mVertices[face.mIndices[e]];
					memcpy(vertices+face.mIndices[e]*3, &vert, sizeof(float)*3);
				}

				//memcpy(indices+i*3, &face.mIndices, sizeof(unsigned int)*3);
				indices[i*3] = face.mIndices[0];
				indices[i*3+1] = face.mIndices[1];
				indices[i*3+2] = face.mIndices[2];
			}

			// Convert the data into an OpenGL format
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glGenBuffers(1, &vbo_vertices);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
			glBufferData(GL_ARRAY_BUFFER, 3 * vertex_amount * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(game->renderer->vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(game->renderer->vertex_location);

			glGenBuffers(1, &vbo_normals);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
			glBufferData(GL_ARRAY_BUFFER, 3 * vertex_amount * sizeof(GLfloat), normals, GL_STATIC_DRAW);
			glVertexAttribPointer(game->renderer->normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(game->renderer->normal_location);

			glGenBuffers(1, &vbo_texcoords);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
			glBufferData(GL_ARRAY_BUFFER, 2 * vertex_amount * sizeof(GLfloat), uv_array, GL_STATIC_DRAW);
			glVertexAttribPointer(game->renderer->fragment_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(game->renderer->fragment_location);

			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			// Load mesh texture
			if (mesh->HasTextureCoords(0)) {
				const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
				aiString tex_path;
				if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
					char* fullpath = new char[strlen("resources/meshes/") + tex_path.length + 1];
					strcpy(fullpath, "resources/meshes/");
					strcat(fullpath, tex_path.C_Str());

					SDL_Surface* tmp_surface;
					tmp_surface = IMG_Load(fullpath);
					delete[] fullpath;
					if (tmp_surface == nullptr) { // If the surface could not be loaded
						game->messenger_send({"engine", "sprite"}, bee::E_MESSAGE::WARNING, "Failed to load the texture for mesh \"" + name + "\": " + IMG_GetError());
						return 2; // Return 2 on failure
					}

					// Generate the texture from the surface pixels
					glGenTextures(1, &gl_texture);
					glBindTexture(GL_TEXTURE_2D, gl_texture);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_RGBA,
						tmp_surface->w,
						tmp_surface->h,
						0,
						GL_RGBA,
						GL_UNSIGNED_BYTE,
						tmp_surface->pixels
					);

					SDL_FreeSurface(tmp_surface); // Free the temporary surface

					has_texture = true;
				} else {
					game->messenger_send({"engine", "mesh"}, bee::E_MESSAGE::WARNING, "Failed to load the texture for mesh \"" + name + "\", the material did not report any textures");
				}
			}

			is_loaded = true;
		} else {
			game->messenger_send({"engine", "mesh"}, bee::E_MESSAGE::WARNING, "Failed to load mesh because SDL rendering is currently enabled");
			return 1;
		}
	}
	return 0;
}
int BEE::Mesh::free() {
	if (is_loaded) {
		delete[] vertices;
		delete[] normals;
		delete[] uv_array;
		delete[] indices;

		glDeleteBuffers(1, &vbo_vertices);
		glDeleteBuffers(1, &vbo_normals);
		glDeleteBuffers(1, &vbo_texcoords);
		glDeleteBuffers(1, &ibo);
		glDeleteVertexArrays(1, &vao);
		glDeleteTextures(1, &gl_texture);

		aiReleaseImport(scene);

		has_texture = false;
		is_loaded = false;
	}
	return 0;
}
int BEE::Mesh::draw(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate, RGBA color, bool is_wireframe) {
	if (!is_loaded) {
		if (!has_draw_failed) {
			game->messenger_send({"engine", "mesh"}, bee::E_MESSAGE::WARNING, "Failed to draw mesh \"" + name + "\" because it is not loaded");
			has_draw_failed = true;
		}
		return 1;
	}

	glBindVertexArray(vao);

	if (has_texture) {
		glUniform1i(game->renderer->texture_location, 0);
		glBindTexture(GL_TEXTURE_2D, gl_texture);
	} else {
		glUniform1i(game->renderer->primitive_location, 1);
	}

	glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
	model = glm::scale(model, scale);
	glUniformMatrix4fv(game->renderer->model_location, 1, GL_FALSE, glm::value_ptr(model));
	glm::mat4 rotation = glm::mat4(1.0f);
	if (rotate.x != 0.0) {
		rotation = glm::rotate(rotation, (float)degtorad(rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (rotate.y != 0.0) {
		rotation = glm::rotate(rotation, (float)degtorad(rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (rotate.z != 0.0) {
		rotation = glm::rotate(rotation, (float)degtorad(rotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	glUniformMatrix4fv(game->renderer->rotation_location, 1, GL_FALSE, glm::value_ptr(rotation));

	glm::vec4 c = glm::vec4((float)color.r/255.0f, (float)color.g/255.0f, (float)color.b/255.0f, (float)color.a/255.0f);
	glUniform4fv(game->renderer->colorize_location, 1, glm::value_ptr(c));

	if (is_wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Bind the vertices to fix disruption from primitive drawing
	glEnableVertexAttribArray(game->renderer->vertex_location);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glVertexAttribPointer(game->renderer->vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);

	/*for (size_t i=0; i<mesh->mNumFaces; i++) {
		const aiFace &face = mesh->mFaces[i];
		if (face.mNumIndices < 3) {
			continue;
		}

		aiVector3D vert1 = mesh->mVertices[face.mIndices[0]];
		aiVector3D vert2 = mesh->mVertices[face.mIndices[1]];
		aiVector3D vert3 = mesh->mVertices[face.mIndices[2]];
		game->draw_triangle(glm::vec3(vert1[0], vert1[1], vert1[2]), glm::vec3(vert2[0], vert2[1], vert2[2]), glm::vec3(vert3[0], vert3[1], vert3[2]), {255, 0, 0, 255}, !is_wireframe);
	}*/

	glUniformMatrix4fv(game->renderer->model_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(game->renderer->rotation_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(game->renderer->primitive_location, 0);

	glBindVertexArray(0);

	return 0;
}
int BEE::Mesh::draw(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate) {
	return draw(pos, scale, rotate, {255, 255, 255, 255}, false);
}
int BEE::Mesh::draw(glm::vec3 pos) {
	return draw(pos, glm::vec3(1.0f), glm::vec3(1.0), {255, 255, 255, 255}, false);
}

#endif // _BEE_MESH
