/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/cimport.h> // Include the required Assimp headers
#include <assimp/postprocess.h>

#include "mesh.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/real.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"

#include "../render/renderer.hpp"
#include "../render/shader.hpp"

namespace bee {
	std::map<int,Mesh*> Mesh::list;
	int Mesh::next_id = 0;

	/*
	* Mesh::Mesh() - Default construct the mesh
	* ! This constructor should only be directly used for temporary meshes, the other constructor should be used for all other cases
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
	/*
	* Mesh::Mesh() - Construct the mesh, add it to the mesh resource list, and set the new name and path
	*/
	Mesh::Mesh(const std::string& new_name, const std::string& new_path) :
		Mesh() // Default initialize all variables
	{
		add_to_resources(); // Add the mesh to the appropriate resource list
		if (id < 0) { // If the mesh could not be added to the resource list, output a warning
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add mesh resource: \"" + new_name + "\" from " + new_path);
			throw(-1); // Throw an exception
		}

		set_name(new_name); // Set the mesh name
		set_path(new_path); // Set the mesh object file path
	}
	/*
	* Mesh::~Mesh() - Free the mesh data and remove it from the resource list
	*/
	Mesh::~Mesh() {
		this->free(); // Free all the mesh data
		list.erase(id); // Remove the mesh from the resource list
	}
	/*
	* Mesh::add_to_resources() - Add the mesh to the appropriate resource list
	*/
	int Mesh::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return 0; // Return 0 on success
	}
	/*
	* Mesh::get_amount() - Return the amount of mesh resources
	*/
	size_t Mesh::get_amount() {
		return list.size();
	}
	/*
	* Mesh::get() - Return the resource with the given id
	* @id: the resource to get
	*/
	Mesh* Mesh::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/*
	* Mesh::reset() - Reset all resource variables for reinitialization
	*/
	int Mesh::reset() {
		this->free(); // Free all memory used by this resource

		// Reset all properties
		name = "";
		path = "";

		// Reset mesh data
		is_loaded = false;
		has_draw_failed = false;
		vertex_amount = 0;

		return 0; // Return 0 on success
	}
	/*
	* Mesh::print() - Print all relevant information about the resource
	*/
	int Mesh::print() const {
		std::stringstream s; // Declare the output stream
		s << // Append all info to the output
		"Mesh { "
		"\n	id    " << id <<
		"\n	name  " << name <<
		"\n	path  " << path <<
		"\n}\n";
		messenger::send({"engine", "resource"}, E_MESSAGE::INFO, s.str()); // Send the info to the messaging system for output

		return 0; // Return 0 on success
	}

	/*
	* Mesh::get_*() - Return the requested resource information
	*/
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

	/*
	* Mesh::set_*() - Set the requested resource data
	*/
	int Mesh::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Mesh::set_path(const std::string& new_path) {
		path = "resources/meshes/"+new_path; // Append the path to the mesh directory
		return 0;
	}

	/*
	* Mesh::load() - Load the desired mesh from its given filename
	* @mesh_index: the desired mesh index from the imported scene
	*/
	int Mesh::load(int mesh_index) {
		if (is_loaded) { // If the mesh has already been loaded, output a warning
			messenger::send({"engine", "mesh"}, E_MESSAGE::WARNING, "Failed to load mesh \"" + name + "\" because it is already loaded");
			return 1; // Return 1 when already loaded
		}

		if (get_options().is_headless) {
			return 2; // Return 2 when in headless mode
		}

		if (get_options().renderer_type == E_RENDERER::SDL) { // If the SDL rendering mode is enabled, output a warning
			messenger::send({"engine", "mesh"}, E_MESSAGE::WARNING, "Failed to load mesh because SDL rendering is currently enabled");
			return 3; // Return 3 when SDL rendering is enabled
		}

		// Attempt to import the object file
		scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality); // Import it with "MaxQuality"
		if (scene == nullptr) { // If the file couldn't be imported, output a warning
			messenger::send({"engine", "mesh"}, E_MESSAGE::WARNING, "Failed to load mesh \"" + name + "\": " + aiGetErrorString());
			return 4; // Return 4 on import failure
		}

		mesh = scene->mMeshes[mesh_index]; // Get the mesh with the desired index
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

		// Bind the vertices for the mesh
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, 3 * vertex_amount * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(engine->renderer->program->get_location("v_position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(engine->renderer->program->get_location("v_position"));

		// Bind the normals for the mesh
		glGenBuffers(1, &vbo_normals);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, 3 * vertex_amount * sizeof(GLfloat), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(engine->renderer->program->get_location("v_normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(engine->renderer->program->get_location("v_normal"));

		// Bind the texture coordinates for the mesh
		glGenBuffers(1, &vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, 2 * vertex_amount * sizeof(GLfloat), uv_array, GL_STATIC_DRAW);
		glVertexAttribPointer(engine->renderer->program->get_location("v_texcoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(engine->renderer->program->get_location("v_texcoord"));

		// Bind the mesh ibo
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);

		if (mesh->HasTextureCoords(0)) { // If the mesh has a texture, load it
			material = scene->mMaterials[mesh->mMaterialIndex]; // Get the material for the mesh
			aiString tex_path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) { // Attempt to fetch the texture's path into tex_path
				std::string fullpath = "resources/meshes/" + std::string(tex_path.C_Str()); // Create the full path for the texture

				// Attempt to load the texure as a temporary surface
				SDL_Surface* tmp_surface;
				tmp_surface = IMG_Load(fullpath.c_str());
				if (tmp_surface == nullptr) { // If the surface could not be loaded, output a warning
					free_internal();
					messenger::send({"engine", "sprite"}, E_MESSAGE::WARNING, "Failed to load the texture for mesh \"" + name + "\": " + IMG_GetError());
					return 5; // Return 5 on texture load failure
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
				glBindVertexArray(0); // Unbind the mesh vao

				free_internal();
				messenger::send({"engine", "mesh"}, E_MESSAGE::WARNING, "Failed to load the texture for mesh \"" + name + "\", the material reported a texture with no file path");
				return 6; // Return 6 on missing texture file
			}
		}

		glBindVertexArray(0); // Unbind the mesh vao

		// Set the loaded booleans
		is_loaded = true;
		has_draw_failed = false;

		return 0; // Return 0 on success
	}
	/*
	* Mesh::load() - Load the first mesh from its given filename
	*/
	int Mesh::load() {
		return load(0); // Return the attempt to load the first mesh in the scene
	}
	/*
	* Mesh::free_internal() - Free the mesh buffers and release the scene
	* ! This function is only called directly if there was a failure in the middle of a call to load(), for all other cases use free()
	*/
	int Mesh::free_internal() {
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

		return 0; // Return 0 on success
	}
	/*
	* Mesh::free() - Free the mesh buffers if they have already been loaded
	*/
	int Mesh::free() {
		if (!is_loaded) { // Do not attempt to free the buffers if the mesh hasn't been loaded
			return 0; // Return 0 on success
		}

		return free_internal(); // Return the attempt to free the mesh buffers
	}

	/*
	* Mesh::draw() - Draw the mesh with the given attributes
	* @pos: the position to draw the mesh at
	* @scale: the scale to draw the mesh with
	* @rotate: the rotation to apply to the mesh
	* @color: the color to draw the mesh in
	* @is_wireframe: whether the mesh should be drawn in wireframe or not
	*/
	int Mesh::draw(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate, RGBA color, bool is_wireframe) {
		if (!is_loaded) {
			if (!has_draw_failed) {
				messenger::send({"engine", "mesh"}, E_MESSAGE::WARNING, "Failed to draw mesh \"" + name + "\" because it is not loaded");
				has_draw_failed = true;
			}
			return 1;
		}

		glBindVertexArray(vao); // Bind the vao for the mesh

		if (has_texture) { // If necessary, bind the mesh texture
			glUniform1i(engine->renderer->program->get_location("f_texture"), 0);
			glBindTexture(GL_TEXTURE_2D, gl_texture);
		} else { // Otherwise, enable primitive drawing mode
			glUniform1i(engine->renderer->program->get_location("is_primitive"), 1);
		}

		// Generate the partial transformation matrix (translation and scaling) for the mesh
		glm::mat4 model = glm::translate(glm::mat4(1.0f), pos); // Translate the mesh the desired amount
		model = glm::scale(model, scale); // Scale the mesh as desired
		glUniformMatrix4fv(engine->renderer->program->get_location("model"), 1, GL_FALSE, glm::value_ptr(model)); // Send the transformation matrix to the shader

		// Generate the rotation matrix for the mesh
		// This is not included in the above transformation matrix because it is faster to rotate everything in the geometry shader
		glm::mat4 rotation = glm::mat4(1.0f);
		if (rotate.x != 0.0) { // Rotate around the x-axis if necessary
			rotation = glm::rotate(rotation, static_cast<float>(degtorad(rotate.x)), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (rotate.y != 0.0) { // Rotate around the y-axis if necessary
			rotation = glm::rotate(rotation, static_cast<float>(degtorad(rotate.y)), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (rotate.z != 0.0) { // Rotate around the z-axis if necessary
			rotation = glm::rotate(rotation, static_cast<float>(degtorad(rotate.z)), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		glUniformMatrix4fv(engine->renderer->program->get_location("rotation"), 1, GL_FALSE, glm::value_ptr(rotation)); // Send the rotation matrix to the shader

		// Colorize the mesh with the given color
		glm::vec4 c (color.r, color.g, color.b, color.a);
		c /= 255.0f;
		glUniform4fv(engine->renderer->program->get_location("colorize"), 1, glm::value_ptr(c));

		if (is_wireframe) { // If the mesh should be drawn in wireframe, set the polygone drawing mode to line
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		// Bind the vertices to fix disruption from primitive drawing
		glEnableVertexAttribArray(engine->renderer->program->get_location("v_position"));
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(engine->renderer->program->get_location("v_position"), 3, GL_FLOAT, GL_FALSE, 0, 0);

		// Draw the triangles from the ibo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);

		// Reset the drawing matrices
		glUniformMatrix4fv(engine->renderer->program->get_location("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(engine->renderer->program->get_location("rotation"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset the polygon drawing mode to fill

		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(engine->renderer->program->get_location("is_primitive"), 0);

		glBindVertexArray(0); // Unbind the vao

		return 0; // Return 0 on success
	}
	/*
	* Mesh::draw() - Draw the mesh with the given attributes
	* ! When the function is called with no color or wireframe status, simply call it with white and filled polygon mode
	* @pos: the position to draw the mesh at
	* @scale: the scale to draw the mesh with
	* @rotate: the rotation to apply to the mesh
	*/
	int Mesh::draw(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate) {
		return draw(pos, scale, rotate, {255, 255, 255, 255}, false); // Return the result of drawing the transformed mesh
	}
	/*
	* Mesh::draw() - Draw the mesh with the given attributes
	* ! When the function is called with only position, simply call it with sane defaults
	* @pos: the position to draw the mesh at
	*/
	int Mesh::draw(glm::vec3 pos) {
		return draw(pos, glm::vec3(1.0f), glm::vec3(0.0), {255, 255, 255, 255}, false); // Return the result of drawing the translated mesh
	}
}

#endif // BEE_MESH
