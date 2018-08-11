/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PATH_H
#define BEE_PATH_H 1

#include <string> // Include the required library headers
#include <map>
#include <vector>

#include <btBulletDynamicsCommon.h>

#include "resource.hpp"

#include "../data/variant.hpp"

namespace bee {
	// Forward declarations
	class Path;
	class Instance;

	/// Used to calculate Path coordinates
	struct PathNode {
		btVector3 pos; ///< The node position
		double speed; ///< The speed at the node

		// See bee/resource/path.cpp for function comments
		PathNode(const btVector3&, double);
		PathNode();
	};

	/// Used to hold Path position data
	struct PathFollower {
		Path* path; ///< The Path to follow
		btVector3 offset; ///< The offset from the Path coordinates

		unsigned int node; ///< The current node
		unsigned int progress; ///< The progress to the next node from 0 to 10'000
		unsigned int speed; ///< The speed at which the coordinates progress from 0 to 10'000

		bool direction; ///< Whether the follower is moving forward or backward
		bool is_curved; ///< Whether the path should be curved by a quadratic Bézier curve
		bool is_closed; ///< Whether the path should loop after the end

		bool is_pausable; ///< Whether the follower should pause Path movement

		// See bee/resource/path.cpp for function comments
		PathFollower(Path*, btVector3, unsigned int);
		PathFollower();
	};

	/// Used to repeatedly move in complex patterns
	class Path: public Resource {
		static std::map<size_t,Path*> list;
		static size_t next_id;

		size_t id; ///< The unique Path identifier
		std::string name; ///< An arbitrary resource name
		std::string path; ///< The path of the config file used to populate the node and control point lists

		std::vector<PathNode> nodes; ///< The list of points which the Instance will follow
		std::map<unsigned int,btVector3> control_points; ///< The list of control points for Bezier curves
		bool is_loaded; ///< Whether the Path data has been loaded from the config file

		std::vector<btVector3> coord_cache; ///< A cache of the coordinates used by the drawing function

		// See bee/resource/path.cpp for function comments
		PathNode get_node_prev(const PathFollower&) const;
		PathNode get_node(const PathFollower&) const;
		PathNode get_node_next(const PathFollower&) const;
	public:
		// See bee/resource/path.cpp for function comments
		Path();
		Path(const std::string&, const std::string&);
		~Path();

		static size_t get_amount();
		static Path* get(size_t);
		static Path* get_by_name(const std::string&);
		static Path* add(const std::string&, const std::string&);

		size_t add_to_resources();
		int reset();

		std::map<Variant,Variant> serialize() const;
		int deserialize(std::map<Variant,Variant>& m);
		void print() const;

		size_t get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		const std::vector<PathNode>& get_nodes() const;
		bool get_is_loaded() const;

		void set_name(const std::string&);
		void set_path(const std::string&);

		void add_node(const PathNode&);
		void add_control_point(unsigned int, const btVector3&);
		int remove_node(unsigned int);
		int remove_control_point(unsigned int);

		int load();
		int free();

 		void advance(PathFollower&) const;
		void advance(Instance*, PathFollower&) const;
		btVector3 get_coord(const PathFollower&) const;
		bool at_end(PathFollower&) const;

		void draw(const PathFollower&);
		void draw(const btVector3&);
	};
}

#endif // BEE_PATH_H
