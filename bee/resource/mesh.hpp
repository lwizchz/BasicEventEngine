/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_MESH_H
#define BEE_MESH_H 1

#include <string> // Include the required library headers
#include <map>

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/scene.h> // Include the required Assimp headers
#include <assimp/mesh.h>

#include "resource.hpp"

#include "../data/variant.hpp"

#include "../render/rgba.hpp"

namespace bee {
	/// Used to draw all 3D items
	class Mesh: public Resource {
		static std::map<int,Mesh*> list;
		static int next_id;

		int id; ///< The unique Mesh identifier
		std::string name; ///< An arbitrary resource name
		std::string path; ///< The path of the object file

		bool is_loaded; ///< Whether the object file was successfully loaded into the buffers
		bool has_draw_failed; ///< Whether the draw function has previously failed, this prevents continuous warning outputs
		bool has_texture; ///< Whether the mesh has an associated texture
		int vertex_amount; ///< The number of vertices contained in the mesh

		const aiScene* scene; ///< The scene containing all of the meshes in the object file
		const aiMesh* mesh; ///< The primary mesh from the scene, the one we're interested in
		const aiMaterial* material; ///< The material that the mesh uses
		float* vertices; ///< An array of the mesh's vertices
		float* normals; ///< An array of the mesh's normals
		float* uv_array; ///< An array of the mesh's UVs
		unsigned int* indices; ///< An array of the mesh's indices

		GLuint vao; ///< The Vertex Array Object which contains most of the following data
		GLuint vbo_vertices; ///< The Vertex Buffer Object which contains the vertices of the faces
		GLuint vbo_normals; ///< The buffer object which contains the normals of the faces
		GLuint vbo_texcoords;  ///< The buffer object which contains the subimage texture coordinates
		GLuint ibo; ///< The buffer object which contains the order of the vertices for each element
		GLuint gl_texture; ///< The internal texture storage

		// See bee/resources/mesh.cpp for function comments
		void free_internal();
	public:
		// See bee/resources/mesh.cpp for function comments
		Mesh();
		Mesh(const std::string&, const std::string&);
		~Mesh();

		static size_t get_amount();
		static Mesh* get(int);
		static Mesh* get_by_name(const std::string&);
		static Mesh* add(const std::string&, const std::string&);

		int add_to_resources();
		int reset();

		std::map<Variant,Variant> serialize() const;
		int deserialize(std::map<Variant,Variant>&);
		void print() const;

		int get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		bool get_is_loaded() const;

		void set_name(const std::string&);
		void set_path(const std::string&);

		int load(int);
		int load();
		int free();

		int draw(glm::vec3, glm::vec3, glm::vec3, RGBA, bool);
		int draw(glm::vec3, glm::vec3, glm::vec3);
		int draw(glm::vec3);
	};
}

#endif // BEE_MESH_H
