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
	/// Used to transfer vertex bone data to the geometry shader
	struct MeshBoneData {
		unsigned int indices[4]; ///< The indices of the attached bones
		float weights[4]; ///< The weight of the attached bone
	};

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

		std::vector<float> vertices; ///< A vector of the mesh's vertices
		std::vector<float> normals; ///< A vector of the mesh's normals
		std::vector<float> uv_array; ///< A vector of the mesh's UVs
		std::vector<unsigned int> indices; ///< A vector of the mesh's indices

		std::map<std::string,std::pair<size_t,const aiBone*>> bones; ///< The map of skeletal mesh bones
		std::map<std::string,const aiAnimation*> animations; ///< The map of skeletal animations
		std::vector<MeshBoneData> bone_data; ///< The vector of the bone vertex data
		std::map<const aiAnimation*, std::map<const aiNode*, const aiNodeAnim*>> node_animations; ///< The map of nodes and their associated animations

		GLuint vao; ///< The Vertex Array Object which contains most of the following data
		GLuint vbo_vertices; ///< The Vertex Buffer Object which contains the vertices of the faces
		GLuint vbo_normals; ///< The buffer object which contains the normals of the faces
		GLuint vbo_texcoords;  ///< The buffer object which contains the subimage texture coordinates
		GLuint ibo; ///< The buffer object which contains the order of the vertices for each element
		GLuint vbo_bones; ///< The buffer object which contains the vertex bone data
		GLuint gl_texture; ///< The internal texture storage

		// See bee/resources/mesh.cpp for function comments
		void free_internal();

		void gen_node_animations(const aiAnimation*);
		void calc_transforms(const aiAnimation*, float, const aiNode*, const glm::mat4&, std::vector<glm::mat4>*);

		static glm::vec3 interp_scale(const aiNodeAnim*, float);
		static glm::mat4 interp_rotate(const aiNodeAnim*, float);
		static glm::vec3 interp_translate(const aiNodeAnim*, float);
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
		bool has_animation(const std::string&) const;

		void set_name(const std::string&);
		void set_path(const std::string&);

		int load(int);
		int load();
		int free();

		int draw(const std::string&, Uint32, glm::vec3, glm::vec3, glm::vec3, RGBA, bool);
		int draw(glm::vec3, glm::vec3, glm::vec3, RGBA, bool);
		int draw(glm::vec3, glm::vec3, glm::vec3);
	};
}

#endif // BEE_MESH_H
