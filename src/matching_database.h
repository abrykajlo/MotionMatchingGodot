#pragma once

#include "animation_database.h"

#include "../kdtree-cpp/kdtree.hpp"

struct Frame {
	size_t animation = -1;
	size_t frame = -1;

	bool operator==(const Frame& rhs) const;
	bool operator!=(const Frame& rhs) const;
};

class MatchingDatabase {
public:
	MatchingDatabase();

	void build_database(const AnimationDatabase& animation_database);

	Frame search(const Skeleton3D& skeleton, const Vector2& left_input, float right_input) const;

private:
	void _add_feature(const Animation& animation, size_t frame, size_t index);
	void _get_trajectories(Kdtree::CoordPoint& feature_vector, const Animation& animation, size_t frame) const;
	void _get_trajectories(Kdtree::CoordPoint& feature_vector, const Vector2& velocity, float yaw_rate) const;

	std::unique_ptr<Kdtree::KdTree> _kd_tree = nullptr;
	Kdtree::KdNodeVector _nodes;
	std::vector<Frame> _frames;
	float _max_velocity = 0;
	float _max_yaw_rate = 0;
	float _frame_time = 0;
};
