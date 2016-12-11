/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_MESH_H
#define _BEE_MESH_H 1

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include "../game.hpp"

class BEE::Mesh: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string path;

		bool is_loaded = false;
		bool has_draw_failed = false;
		bool has_texture = false;
		int vertex_amount = 0;

		const aiScene* scene = nullptr;
		aiMesh* mesh = nullptr;
		float* vertices;
		float* normals;
		float* uv_array;
		unsigned int* indices;

		GLuint vao; // The Vertex Array Object which contains most of the following data
		GLuint vbo_vertices; // The Vertex Buffer Object which contains the vertices of the faces
		GLuint vbo_normals; // The buffer object which contains the normals of the faces
		GLuint vbo_texcoords;  // The buffer object which contains the subimage texture coordinates
		GLuint ibo; // The buffer object which contains the order of the vertices for each element
		GLuint gl_texture; // The internal texture storage for OpenGL mode
	public:
		Mesh();
		Mesh(std::string, std::string);
		~Mesh();
		int add_to_resources();
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();

		int set_name(std::string);
		int set_path(std::string);

		int load();
		int free();
		int draw(glm::vec3, glm::vec3, glm::vec3, RGBA, bool);
		int draw(glm::vec3, glm::vec3, glm::vec3);
		int draw(glm::vec3);
};

#endif // _BEE_MESH_H
