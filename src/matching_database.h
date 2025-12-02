#pragma once

#include "animation_database.h"

#include "../kdtree-cpp/kdtree.hpp"

struct Frame {
	size_t animation = -1;
	size_t frame = -1;

	bool operator==(const Frame& rhs) const;
	bool operator!=(const Frame& rhs) const;
};

class Feature {
public:
	Feature(const Skeleton3D& skeleton, const Vector2& velocity, float yaw, float frame_time);
	Feature(const Animation& animation, size_t frame);

	void get_coord_point(Kdtree::CoordPoint& coord_point) const;

private:
	Vector2 _trajectories[3];
	Vector2 _facing[3];
	Vector2 _velocity;
};

class MatchingDatabase {
public:
	MatchingDatabase();

	void build_database(const AnimationDatabase& animation_database);

	Frame search(const Skeleton3D& skeleton, float yaw, const Vector2& left_input) const;

private:
	void _add_feature(const Animation& animation, size_t frame, size_t index);

	std::unique_ptr<Kdtree::KdTree> _kd_tree = nullptr;
	Kdtree::KdNodeVector _nodes;
	std::vector<Frame> _frames;
	float _max_velocity = 0;
	float _frame_time = 0;
};
