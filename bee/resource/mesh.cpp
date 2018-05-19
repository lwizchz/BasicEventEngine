/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_MESH
#define BEE_MESH 1

#include "../defines.hpp"

#include <sstream> // Include the required library headers

#include <SDL2/SDL_image.h> // Include the required SDL headers

#include <glm/gtc/type_ptr.hpp>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#include "mesh.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/real.hpp"
#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"

#include "../render/render.hpp"
#include "../render/shader.hpp"

namespace bee {
	std::map<int,Mesh*> Mesh::list;
	int Mesh::next_id = 0;

	/**
	* Default construct the Mesh.
	* @note This constructor should only be used for temporary Meshes, the other constructor should be used for all other cases.
	*/
	Mesh::Mesh() :
		Resource(),

		id(-1),
		name(),
		path(),

		is_loaded(false),
		has_draw_failed(false),
		has_texture(false),
		vertex_amount(0),

		scene(nullptr),
		mesh(nullptr),
		material(nullptr),
		vertices(nullptr),
		normals(nullptr),
		uv_array(nullptr),
		indices(nullptr),

		vao(-1),
		vbo_vertices(-1),
		vbo_normals(-1),
		vbo_texcoords(-1),
		ibo(-1),
		gl_texture(-1)
	{}
	/**
	* Construct the Mesh, add it to the Mesh resource list, and set the new name and path.
	* @param _name the name for the Mesh
	* @param _path the path for the Mesh
	*/
	Mesh::Mesh(const std::string& _name, const std::string& _path) :
		Mesh() // Default initialize all variables
	{
		if (add_to_resources() < 0) { // Attempt to add the Mesh to its resource list
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add Mesh resource: \"" + _name + "\" from " + _path);
			throw(-1); // Throw an exception
		}

		set_name(_name);
		set_path(_path);
	}
	/**
	* Free the Mesh data and remove it from the resource list.
	*/
	Mesh::~Mesh() {
		this->free();
		list.erase(id);
	}

	/**
	* @returns the number of Mesh resources
	*/
	size_t Mesh::get_amount() {
		return list.size();
	}
	/**
	* @param id the resource to get
	*
	* @returns the resource with the given id
	*/
	Mesh* Mesh::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/**
	* @param name the name of the desired Mesh
	*
	* @returns the Mesh resource with the given name
	*/
	Mesh* Mesh::get_by_name(const std::string& name) {
		for (auto& mesh : list) { // Iterate over the Meshes in order to find the first one with the given name
			Mesh* m = mesh.second;
			if (m != nullptr) {
				if (m->get_name() == name) {
					return m; // Return the desired Mesh on success
				}
			}
		}
		return nullptr; // Return nullptr on failure
	}
	/**
	* Initiliaze, load, and return a newly created Mesh resource.
	* @param name the name to initialize the Mesh with
	* @param path the path to initialize the Mesh with
	*
	* @returns the newly loaded Mesh
	*/
	Mesh* Mesh::add(const std::string& name, const std::string& path) {
		Mesh* new_mesh = new Mesh(name, path);
		new_mesh->load();
		return new_mesh;
	}

	/**
	* Add the Mesh to the appropriate resource list.
	*
	* @returns the Mesh id
	*/
	int Mesh::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource with it's new id
		}

		return id;
	}
	/**
	* Reset all resource variables for reinitialization.
	*
	* @retval 0 success
	*/
	int Mesh::reset() {
		this->free(); // Free all memory used by this resource

		// Reset all properties
		name = "";
		path = "";

		// Reset Mesh data
		is_loaded = false;
		has_draw_failed = false;
		has_texture = false;
		vertex_amount = 0;

		return 0;
	}

	/**
	* @returns a map of all the information required to restore the Mesh
	*/
	std::map<Variant,Variant> Mesh::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = id;
		info["name"] = name;
		info["path"] = path;

		info["is_loaded"] = is_loaded;
		info["has_draw_failed"] = has_draw_failed;
		info["has_texture"] = has_texture;
		info["vertex_amount"] = vertex_amount;

		return info;
	}
	/**
	* Restore the Mesh from serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	* @retval 1 failed to load the Font
	*/
	int Mesh::deserialize(std::map<Variant,Variant>& m) {
		this->free();

		id = m["id"].i;
		name = m["name"].s;
		path = m["path"].s;

		is_loaded = false;
		has_draw_failed = m["has_draw_failed"].i;
		has_texture = false;
		vertex_amount = 0;

		if ((m["is_loaded"].i)&&(load())) {
			return 1;
		}

		return 0;
	}
	/**
	* Print all relevant information about the resource.
	*/
	void Mesh::print() const {
		Variant m (serialize());
		messenger::send({"engine", "mesh"}, E_MESSAGE::INFO, "Mesh " + m.to_str(true));
	}

	int Mesh::get_id() const {
		return id;
	}
	std::string Mesh::get_name() const {
		return name;
	}
	std::string Mesh::get_path() const {
		return path;
	}
	bool Mesh::get_is_loaded() const {
		return is_loaded;
	}

	void Mesh::set_name(const std::string& _name) {
		name = _name;
	}
	/**
	* Set the relative or absolute resource path.
	* @param _path the new path to use
	* @note If the first character is '/' then the path will be relative to
	*       the executable directory, otherwise it will be relative to the
	*       Mesh resource directory.
	*/
	void Mesh::set_path(const std::string& _path) {
		if (_path.front() == '/') {
			path = _path.substr(1);
		} else { // Append the path to the Mesh directory if no root
			path = "resources/meshes/"+_path;
		}
	}

	/**
	* Load the desired Mesh from its given filename.
	* @param index the desired mesh index from the imported scene
	*
	* @retval 0 success
	* @retval 1 failed to load since it's already loaded
	* @retval 2 failed to load since the engine is in headless mode
	* @retval 3 failed to load the object file
	* @retval 4 failed to load since the mesh's texture file is missing
	* @retval 5 failed to load the mesh's texture file
	*/
	int Mesh::load(int index) {
		if (is_loaded) { // If the Mesh has already been loaded, output a warning
			messenger::send({"engine", "mesh"}, E_MESSAGE::WARNING, "Failed to load Mesh \"" + name + "\" because it is already loaded");
			return 1;
		}

		if (get_option("is_headless").i) {
			return 2;
		}

		// Attempt to import the object file
		scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality); // Import it with "MaxQuality"
		if (scene == nullptr) { // If the file couldn't be imported, output a warning
			messenger::send({"engine", "mesh"}, E_MESSAGE::WARNING, "Failed to load Mesh \"" + name + "\": " + aiGetErrorString());
			return 3;
		}

		mesh = scene->mMeshes[index]; // Get the mesh with the desired index
		vertex_amount = mesh->mNumVertices; // Fetch the number of vertices in the mesh

		// Allocate space for the vertices and other mesh attributes
		vertices = new float[vertex_amount*3]; // 3 coordinates per vertex
		normals = new float[vertex_amount*3]; // 3 vector components per vertex normal
		uv_array = new float[vertex_amount*2]; // 2 texture coordinates per vertex
		indices = new unsigned int[mesh->mNumFaces*3]; // 3 vertices per face triangle

		for (size_t i=0; i<mesh->mNumFaces; i++) { // Iterate over the faces in the mesh
			const aiFace& face = mesh->mFaces[i]; // Get a reference to the current face
			if (face.mNumIndices < 3) { // If the face has less than three vertices, skip it
				continue;
			}

			for (size_t e=0; e<3; e++) { // Iterate over the vertices in the face
				if (mesh->mTextureCoords[0] != nullptr) { // If the vertex has texture coordinates, copy them to the uv_array
					const aiVector3D& uv = mesh->mTextureCoords[0][face.mIndices[e]];
					memcpy(uv_array+face.mIndices[e]*2, &uv, sizeof(float)*2);
				} else { // Otherwise, zero them
					uv_array[face.mIndices[e]] = 0;
					uv_array[face.mIndices[e]+1] = 0;
				}

				// Copy the vertex normals to the normals array
				const aiVector3D& norm = mesh->mNormals[face.mIndices[e]];
				memcpy(normals+face.mIndices[e]*3, &norm, sizeof(float)*3);

				// Copy the vertex coordinates to the vertex array
				const aiVector3D& vert = mesh->mVertices[face.mIndices[e]];
				memcpy(vertices+face.mIndices[e]*3, &vert, sizeof(float)*3);
			}

			// Copy the face's vertex indices to the indices array
			//memcpy(indices+i*3, &face.mIndices, sizeof(unsigned int)*3);
			indices[i*3] = face.mIndices[0];
			indices[i*3+1] = face.mIndices[1];
			indices[i*3+2] = face.mIndices[2];
		}

		// Convert the data into an OpenGL format
		glGenVertexArrays(1, &vao); // Generate the vertex object array
		glBindVertexArray(vao);

		// Bind the vertices for the Mesh
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, 3 * vertex_amount * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(render::get_program()->get_location("v_position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(render::get_program()->get_location("v_position"));

		// Bind the normals for the Mesh
		/*glGenBuffers(1, &vbo_normals);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, 3 * vertex_amount * sizeof(GLfloat), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(render::get_program()->get_location("v_normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(render::get_program()->get_location("v_normal"));*/

		// Bind the texture coordinates for the Mesh
		glGenBuffers(1, &vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, 2 * vertex_amount * sizeof(GLfloat), uv_array, GL_STATIC_DRAW);
		glVertexAttribPointer(render::get_program()->get_location("v_texcoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(render::get_program()->get_location("v_texcoord"));

		// Bind the Mesh ibo
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);

		if (mesh->HasTextureCoords(0)) { // If the mesh has a texture, load it
			material = scene->mMaterials[mesh->mMaterialIndex]; // Get the material for the mesh
			aiString tex_path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) { // Attempt to fetch the texture's path into tex_path
				// Create the full path for the texture
				std::string fullpath (tex_path.C_Str());
				if (fullpath.front() == '/') {
					fullpath = fullpath.substr(1);
				} else {
					fullpath = "resources/meshes/" + fullpath;
				}

				// Attempt to load the texure as a temporary surface
				SDL_Surface* tmp_surface;
				tmp_surface = IMG_Load(fullpath.c_str());
				if (tmp_surface == nullptr) { // If the surface could not be loaded, output a warning
					free_internal();
					messenger::send({"engine", "sprite"}, E_MESSAGE::WARNING, "Failed to load the texture for Mesh \"" + name + "\": " + util::get_sdl_error());
					return 5;
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

				// Set the texture boolean
				has_texture = true;
			} else {
				glBindVertexArray(0); // Unbind the Mesh vao

				free_internal();
				messenger::send({"engine", "mesh"}, E_MESSAGE::WARNING, "Failed to load the texture for Mesh \"" + name + "\", the material reported a texture with no file path");
				return 4;
			}
		}

		glBindVertexArray(0); // Unbind the Mesh vao

		// Set the loaded booleans
		is_loaded = true;
		has_draw_failed = false;

		return 0;
	}
	/**
	* Load the first Mesh from its given filename.
	*
	* @returns whether the load failed or not
	* @see load(int) for details
	*/
	int Mesh::load() {
		return load(0);
	}
	/**
	* Free the Mesh buffers and release the scene.
	* @note This function is only called directly if there was a failure in the middle of a call to load(), for all other cases use free().
	*/
	void Mesh::free_internal() {
		// Delete the vertex array
		delete[] vertices;
		delete[] normals;
		delete[] uv_array;
		delete[] indices;
		vertices = nullptr;
		normals = nullptr;
		uv_array = nullptr;
		indices = nullptr;

		// Delete the vertex buffers
		glDeleteBuffers(1, &vbo_vertices);
		glDeleteBuffers(1, &vbo_normals);
		glDeleteBuffers(1, &vbo_texcoords);
		glDeleteBuffers(1, &ibo);

		// Delete the texture buffer and vao
		glDeleteTextures(1, &gl_texture);
		glDeleteVertexArrays(1, &vao);

		// Finally, release the scene import
		aiReleaseImport(scene);
		scene = nullptr;

		// Reset the loaded booleans
		has_texture = false;
		is_loaded = false;
	}
	/**
	* Free the Mesh buffers.
	*
	* @retval 0 success
	*/
	int Mesh::free() {
		if (!is_loaded) { // Do not attempt to free the buffers if the Mesh hasn't been loaded
			return 0;
		}

		free_internal();

		return 0;
	}

	/**
	* Draw the Mesh with the given attributes.
	* @param pos the position to draw the mesh at
	* @param scale the scale to draw the mesh with
	* @param rotate the rotation to apply to the mesh
	* @param color the color to draw the mesh in
	* @param is_wireframe whether the mesh should be drawn in wireframe or not
	*
	* @retval 0 success
	* @retval 1 failed to draw since it's not loaded
	*/
	int Mesh::draw(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate, RGBA color, bool is_wireframe) {
		if (!is_loaded) {
			if (!has_draw_failed) {
				messenger::send({"engine", "mesh"}, E_MESSAGE::WARNING, "Failed to draw Mesh \"" + name + "\" because it is not loaded");
				has_draw_failed = true;
			}
			return 1;
		}

		glBindVertexArray(vao); // Bind the vao for the Mesh

		if (has_texture) { // If necessary, bind the Mesh texture
			glUniform1i(render::get_program()->get_location("f_texture"), 0);
			glBindTexture(GL_TEXTURE_2D, gl_texture);
		} else { // Otherwise, enable primitive drawing mode
			glUniform1i(render::get_program()->get_location("is_primitive"), 1);
		}

		// Generate the partial transformation matrix (translation and scaling) for the Mesh
		glm::mat4 model = glm::translate(glm::mat4(1.0f), pos); // Translate the Mesh the desired amount
		model = glm::scale(model, scale); // Scale the Mesh as desired
		glUniformMatrix4fv(render::get_program()->get_location("model"), 1, GL_FALSE, glm::value_ptr(model)); // Send the transformation matrix to the shader

		// Generate the rotation matrix for the Mesh
		// This is not included in the above transformation matrix because it is faster to rotate everything in the geometry shader
		glm::mat4 rotation = glm::mat4(1.0f);
		if (rotate.x != 0.0) { // Rotate around the x-axis if necessary
			rotation = glm::rotate(rotation, static_cast<float>(util::degtorad(rotate.x)), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (rotate.y != 0.0) { // Rotate around the y-axis if necessary
			rotation = glm::rotate(rotation, static_cast<float>(util::degtorad(rotate.y)), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (rotate.z != 0.0) { // Rotate around the z-axis if necessary
			rotation = glm::rotate(rotation, static_cast<float>(util::degtorad(rotate.z)), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		glUniformMatrix4fv(render::get_program()->get_location("rotation"), 1, GL_FALSE, glm::value_ptr(rotation)); // Send the rotation matrix to the shader

		// Colorize the Mesh with the given color
		glm::vec4 c (color.r, color.g, color.b, color.a);
		c /= 255.0f;
		glUniform4fv(render::get_program()->get_location("colorize"), 1, glm::value_ptr(c));

		if (is_wireframe) { // If the Mesh should be drawn in wireframe, set the polygone drawing mode to line
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		// Bind the vertices to fix disruption from primitive drawing
		glEnableVertexAttribArray(render::get_program()->get_location("v_position"));
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(render::get_program()->get_location("v_position"), 3, GL_FLOAT, GL_FALSE, 0, 0);

		// Draw the triangles from the ibo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);

		// Reset the drawing matrices
		glUniformMatrix4fv(render::get_program()->get_location("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(render::get_program()->get_location("rotation"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset the polygon drawing mode to fill

		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(render::get_program()->get_location("is_primitive"), 0);

		glBindVertexArray(0); // Unbind the vao

		return 0;
	}
	/**
	* Draw the Mesh with the given attributes.
	* @note When the function is called with no color or wireframe status, let them be white and filled polygon mode.
	* @param pos the position to draw the mesh at
	* @param scale the scale to draw the mesh with
	* @param rotate the rotation to apply to the mesh
	*/
	int Mesh::draw(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate) {
		return draw(pos, scale, rotate, {255, 255, 255, 255}, false);
	}
	/**
	* Draw the Mesh with the given attributes.
	* @note When the function is called with only position, let it have scaling with 1.0, rotation of 0.0, white color, and filled polygon drawing.
	* @param pos the position to draw the mesh at
	*/
	int Mesh::draw(glm::vec3 pos) {
		return draw(pos, glm::vec3(1.0f), glm::vec3(0.0), {255, 255, 255, 255}, false);
	}
}

#endif // BEE_MESH
