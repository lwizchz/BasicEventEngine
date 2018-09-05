/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PATH
#define BEE_PATH 1

#include "path.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"
#include "../util/debug.hpp"
#include "../util/real.hpp"

#include "../messenger/messenger.hpp"

#include "../core/instance.hpp"
#include "../fs/fs.hpp"

#include "../render/drawing.hpp"
#include "../render/rgba.hpp"

namespace bee {
	/**
	* Construct the node from a vector and speed.
	* @param _pos the node set_position
	* @param _speed the speed near the node
	*/
	PathNode::PathNode(const btVector3& _pos, double _speed) :
		pos(_pos),
		speed(_speed)
	{}
	/**
	* Default construct the node.
	*/
	PathNode::PathNode() :
		PathNode({0.0, 0.0, 0.0}, 0.0)
	{}

	/**
	* Construct the follower from an offset and speed.
	* @param _path the Path to follow
	* @param _offset the offset to apply to the Path nodes
	* @param _speed the speed to advance with
	*/
	PathFollower::PathFollower(Path* _path, btVector3 _offset, unsigned int _speed) :
		path(_path),
		offset(_offset),

		node(-1),
		progress(0),
		speed(_speed),

		direction(true),
		is_curved(false),
		is_closed(false),

		is_pausable(true)
	{}
	/**
	* Default construct the follower.
	*/
	PathFollower::PathFollower() :
		PathFollower(nullptr, {0.0, 0.0, 0.0}, 100)
	{}

	std::map<size_t,Path*> Path::list;
	size_t Path::next_id = 0;

	/**
	* Default construct the Path.
	* @note This constructor should only be used for temporary Paths, the other constructor should be used for all other cases.
	*/
	Path::Path() :
		Resource(),

		id(-1),
		name(),
		path(),

		nodes(),
		control_points(),
		is_loaded(false),

		coord_cache()
	{}
	/**
	* Construct the Path, add it to the Path resource list, and set the new name and file path.
	* @param _name the name of the Path to use
	* @param _path the filename of the Path's config file
	*
	* @throws int(-1) Failed to initialize Resource
	*/
	Path::Path(const std::string& _name, const std::string& _path) :
		Path() // Default initialize all variables
	{
		if (add_to_resources() == static_cast<size_t>(-1)) { // Attempt to add the Path to its resource list
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add Path resource: \"" + _name + "\" from " + _path);
			throw -1;
		}

		set_name(_name);
		set_path(_path); // Set the config file path
	}
	/**
	* Remove the Path from the resource list.
	*/
	Path::~Path() {
		list.erase(id);
	}

	/**
	* @returns the number of Path resources
	*/
	size_t Path::get_amount() {
		return list.size();
	}
	/**
	* @param id the resource to get
	*
	* @returns the resource with the given id or nullptr if not found
	*/
	Path* Path::get(size_t id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/**
	* @param name the name of the desired Path
	*
	* @returns the Path resource with the given name or nullptr if not found
	*/
	Path* Path::get_by_name(const std::string& name) {
		for (auto& path : list) { // Iterate over the Paths in order to find the first one with the given name
			Path* p = path.second;
			if (p != nullptr) {
				if (p->get_name() == name) {
					return p; // Return the desired Path on success
				}
			}
		}
		return nullptr;
	}
	/**
	* Initiliaze, load, and return a newly created Path resource.
	* @param name the name to initialize the Path with
	* @param path the file path to initialize the Path with
	*
	* @returns the newly created Path
	*/
	Path* Path::add(const std::string& name, const std::string& path) {
		Path* new_path = new Path(name, path);
		if (!path.empty()) {
			new_path->load();
		}
		return new_path;
	}

	/**
	* Add the Path to the appropriate resource list.
	*
	* @returns the Path id
	*/
	size_t Path::add_to_resources() {
		if (id == static_cast<size_t>(-1)) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource with its new id
		}

		return id;
	}
	/**
	* Reset all resource variables for reinitialization.
	*
	* @retval 0 success
	*/
	int Path::reset() {
		this->free();

		// Reset all properties
		name = "";
		path = "";

		nodes.clear();
		control_points.clear();
		is_loaded = false;

		coord_cache.clear();

		return 0;
	}

	/**
	* @returns a map of all the information required to restore the Path
	*/
	std::map<Variant,Variant> Path::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = static_cast<int>(id);
		info["name"] = name;
		info["path"] = path;

		std::vector<Variant> _nodes;
		for (auto& n : nodes) {
			_nodes.emplace_back(std::vector<Variant>({
				Variant(n.pos.x()), Variant(n.pos.y()), Variant(n.pos.z())
			}));
		}
		info["nodes"] = _nodes;

		std::vector<Variant> _control_points;
		for (auto& cp : control_points) {
			std::vector<Variant> _cp;
			_cp.emplace_back(static_cast<int>(cp.first));
			_cp.emplace_back(std::vector<Variant>({
				Variant(cp.second.x()), Variant(cp.second.y()), Variant(cp.second.z())
			}));

			_control_points.emplace_back(_cp);
		}
		info["control_points"] = _control_points;

		info["is_loaded"] = is_loaded;

		return info;
	}
	/**
	* Restore the Path from serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	*/
	int Path::deserialize(std::map<Variant,Variant>& m) {
		this->free();

		id = m["id"].i;
		name = m["name"].s;
		path = m["path"].s;

		for (auto& n : m["nodes"].v) {
			btVector3 pos (n.v[0].v[0].d, n.v[0].v[1].d, n.v[0].v[2].d);
			nodes.emplace_back(pos, n.v[1].d);
		}

		for (auto& cp : m["control_points"].v) {
			btVector3 pos (cp.v[1].v[0].d, cp.v[1].v[1].d, cp.v[1].v[2].d);
			control_points.emplace(cp.v[0].i, pos);
		}

		is_loaded = false;

		if ((m["is_loaded"].i)&&(load())) {
			return 1;
		}

		return 0;
	}
	/**
	* Print all relevant information about the resource.
	*/
	void Path::print() const {
		Variant m (serialize());
		messenger::send({"engine", "path"}, E_MESSAGE::INFO, "Path " + m.to_str(true));
	}

	size_t Path::get_id() const {
		return id;
	}
	std::string Path::get_name() const {
		return name;
	}
	std::string Path::get_path() const {
		return path;
	}
	const std::vector<PathNode>& Path::get_nodes() const {
		return nodes;
	}
	bool Path::get_is_loaded() const {
		return is_loaded;
	}

	void Path::set_name(const std::string& _name) {
		name = _name;
	}
	/**
	* Set the relative or absolute resource path.
	* @param _path the new path to use
	* @note If the first character is '/' then the path will be relative to
	*       the executable directory, otherwise it will be relative to the
	*       Paths resource directory.
	*/
	void Path::set_path(const std::string& _path) {
		if (_path.empty()) {
			path.clear();
		} else if (_path.front() == '/') {
			path = _path.substr(1);
		} else { // Append the path to the Path directory if not root
			path = "resources/paths/"+_path;
		}
		is_loaded = false;
	}

	/**
	* Add a node to the node list.
	* @param new_node the PathNode to add
	*/
	void Path::add_node(const PathNode& new_node) {
		nodes.emplace_back(new_node);
		coord_cache.clear();
	}
	/**
	* Add a Bezier control point to the list.
	* @note The Path will only obey the control points if the is_curved bool is set in the PathFollower.
	* @param node the associated node
	* @param new_cp the control point to add
	*/
	void Path::add_control_point(unsigned int node, const btVector3& new_cp) {
		control_points[node] = new_cp;
		coord_cache.clear();
	}
	/**
	* Remove the node with the given index.
	* @param index the index to remove
	*
	* @retval 0 success
	* @retval 1 failed since a node with the given index doesn't exist
	*/
	int Path::remove_node(unsigned int index) {
		if (index >= nodes.size()) {
			return 1;
		}

		nodes.erase(nodes.begin()+index); // Erase the element at the given index
		coord_cache.clear();

		return 0;
	}
	/**
	* Remove the control point associated with the given node index.
	* @param node_index the node index to remove the control point of
	*
	* @retval 0 success
	* @retval 1 failed since the node with the given index does not have a control point
	*/
	int Path::remove_control_point(unsigned int node_index) {
		std::map<unsigned int,btVector3>::iterator node (control_points.find(node_index));
		if (node == control_points.end()) {
			return 1;
		}

		control_points.erase(node);
		coord_cache.clear();

		return 0;
	}

	/**
	* Load the Path from its file path.
	*
	* @retval 0 success
	* @retval 1 failed to load since it's already loaded
	* @retval 2 failed to load the file
	*/
	int Path::load() {
		if (is_loaded) { // Do not attempt to load the Path if it has already been loaded
	       messenger::send({"engine", "path"}, E_MESSAGE::WARNING, "Failed to load Path \"" + name + "\" because it has already been loaded");
	       return 1;
		}

		std::string cfg (fs::get_file(path).get());
		if (cfg.empty()) { // If the file could not be loaded, output a warning
			messenger::send({"engine", "path"}, E_MESSAGE::WARNING, "Failed to load Path \"" + name + "\" from file \"" + path + "\"");
			return 2;
		}

		// Parse the config file
		Variant m;
		m.interpret(cfg);

		// Clear the old data
		nodes.clear();
		control_points.clear();

		// Load the new nodes
		for (auto& n : m.m["nodes"].v) {
			btVector3 pos (n.v[0].v[0].d, n.v[0].v[1].d, n.v[0].v[2].d);
			nodes.emplace_back(pos, n.v[1].d);
		}

		// Load the new control points
		for (auto& cp : m.m["control_points"].v) {
			btVector3 pos (cp.v[1].v[0].d, cp.v[1].v[1].d, cp.v[1].v[2].d);
			control_points.emplace(cp.v[0].i, pos);
		}

		is_loaded = true;

		return 0;
	}
	/**
	* Free the Path node data.
	*
	* @retval 0 success
	*/
	int Path::free() {
		if (!is_loaded) { // Do not attempt to free the data if the Path has not been loaded
			return 0;
		}

		nodes.clear();
		control_points.clear();

		coord_cache.clear();

		is_loaded = false;

		return 0;
	}

	/**
	* @param pf the PathFollower to get the node for
	*
	* @returns the PathNode previous to the current node
	* @note The direction of the PathFollower is not taken into account.
	*/
	PathNode Path::get_node_prev(const PathFollower& pf) const {
		if ((pf.node > 0)&&(pf.node <= nodes.size())) {
			return nodes.at(pf.node-1);
		} else if (!nodes.empty()) {
			if (pf.is_closed) {
				return nodes.back();
			} else {
				return nodes.front();
			}
		}
		return PathNode();
	}
	/**
	* @param pf the PathFollower to get the node for
	*
	* @returns the current PathNode
	*/
	PathNode Path::get_node(const PathFollower& pf) const {
		if (pf.node < nodes.size()) {
			return nodes.at(pf.node);
		} else if (!nodes.empty()) {
			return nodes.back();
		}
		return PathNode();
	}
	/**
	* @param pf the PathFollower to get the node for
	*
	* @returns the PathNode after the current node
	* @note The direction of the PathFollower is not taken into account.
	*/
	PathNode Path::get_node_next(const PathFollower& pf) const {
		if (pf.node+1 < nodes.size()) {
			return nodes.at(pf.node+1);
		} else if (!nodes.empty()) {
			if (pf.is_closed) {
				return nodes.front();
			} else {
				return nodes.back();
			}
		}
		return PathNode();
	}

	/**
	* Advance the given follower along the Path one unit.
	* @param pf the PathFollower to advance
	*/
	void Path::advance(PathFollower& pf) const {
		const int dir = (pf.direction) ? 1 : -1;
		if (pf.node == static_cast<unsigned int>(-1)) {
			if (dir == 1) {
				pf.node = 0;
			} else {
				pf.node = nodes.size()-1;
			}
		}

		PathNode p1 (get_node(pf));
		PathNode p2 (get_node_next(pf));
		if (!pf.direction) {
			p2 = get_node_prev(pf);
		}

		const double prog = pf.progress / 10'000.0;
		pf.progress += pf.speed * (p1.speed*(1.0-prog) + p2.speed*prog);
		if (pf.progress >= 10'000) {
			pf.progress = 0;
			pf.node += dir;
		}

		if (pf.is_closed) {
			if ((dir > 0)&&(pf.node == nodes.size())) {
				pf.node = 0;
			} else if ((dir < 0)&&(pf.node == static_cast<unsigned int>(-1))) {
				pf.node = nodes.size()-1;
			}
		} else {
			if ((dir > 0)&&(pf.node == nodes.size()-1)) {
				pf.node = nodes.size();
				pf.progress = 0;
			} else if ((dir < 0)&&(pf.node == 0)) {
				pf.node = nodes.size();
				pf.progress = 0;
			}
		}
	}
	/**
	* Advance the given follower along the Path and move the given Instance to the new coordinates.
	* @param inst the Instance to move
	* @param pf the PathFollower to advance
	*
	* @see advance(PathFollower&) const for details.
	*/
	void Path::advance(Instance* inst, PathFollower& pf) const {
		advance(pf);
		if (!at_end(pf)) {
			inst->set_pos(get_coord(pf));
		}
	}
	/**
	* @param pf the PathFollower to get the coordinates for
	*
	* @returns the effective coordinates of the given follower
	*/
	btVector3 Path::get_coord(const PathFollower& pf) const {
		btVector3 c (pf.offset);

		btVector3 p1 (get_node(pf).pos);
		btVector3 p2 (get_node_next(pf).pos);
		if (!pf.direction) {
			p2 = get_node_prev(pf).pos;
		}

		btVector3 p (p1);

		if (pf.is_curved) {
			// Bezier progression

			btVector3 p3 ((p2+p1)/2);
			if (control_points.find(pf.node) != control_points.end()) {
				p3 = control_points.at(pf.node);
			}

			const double prog = pf.progress / 10'000.0;
			p = util::sqr(1.0-prog)*p1 + 2*(1.0-prog)*prog*p3 + util::sqr(prog)*p2;
		} else {
			// Linear progression
			const double prog = pf.progress / 10'000.0;
			p += (p2-p1) * prog;
		}

		c += p;

		return c;
	}
	/**
	* @param pf the PathFollower to check the completion of
	*
	* @retval true the given follower has reached the end of its Path
	* @retval false the given follower is still progressing along its Path
	*/
	bool Path::at_end(PathFollower& pf) const {
		return (pf.node == nodes.size());
	}

	/**
	* Draw the effective Path of the given follower for debugging purposes.
	* @param pf the PathFollower to draw
	*/
	void Path::draw(const PathFollower& pf) {
		RGBA c_line (E_RGB::CYAN);
		RGBA c_controls (E_RGB::RED);

		// Generate the coord cache if needed
		if (coord_cache.empty()) {
			PathFollower _pf (pf);
			_pf.offset = {0.0, 0.0, 0.0};

			advance(_pf);
			coord_cache.push_back(get_coord(_pf));
			unsigned int first_node = _pf.node;

			while (
				(!at_end(_pf))
				&&((_pf.node != first_node)||(_pf.progress != 0))
			) {
				advance(_pf);
				coord_cache.push_back(get_coord(_pf));
			}
		}

		glm::vec3 offset (util::bt_to_glm_v3(pf.offset));
		glm::vec3 v1, v2; // Declare two vectors for the start and end points of each line
		for (auto it = coord_cache.begin(); it != --coord_cache.end(); ) {
			v1 = util::bt_to_glm_v3(*it); // Get the start point from the current node
			++it; // Move to the next node
			v2 = util::bt_to_glm_v3(*it); // Get the end point from the next node

			draw_line(offset+v1, offset+v2, c_line);
		}

		// Draw the control points
		for (auto& cp : control_points) {
			draw_circle(offset+util::bt_to_glm_v3(cp.second), 7.0, -1, c_controls);
		}
	}
	/**
	* Draw the Path for debugging purposes.
	* @param offset the amount to offset the Path by
	*/
	void Path::draw(const btVector3& offset) {
		PathFollower pf (this, offset, 100);
		draw(pf);
	}
}

#endif // BEE_PATH
