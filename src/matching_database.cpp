#include "matching_database.h"

namespace {
	static int k_trajectory_count = 3; // number of trajectory points to calculate
	static int k_trajectory_step = 20; // number of frames ahead to count trajectory
}

MatchingDatabase::MatchingDatabase() {}

void MatchingDatabase::build_database(const AnimationDatabase& animation_database)
{
	_frame_time = animation_database.get(0).frame_time;
	for (size_t a = 0; a < animation_database.size(); a++) {
		const Animation& animation = animation_database.get(a);
		for (size_t f = 0; f < animation.frames; f++) {
			_frames.push_back(Frame { a, f });
			_add_feature(animation, f, _frames.size() - 1);
			
			const float velocity = animation.root.velocity[f].length();
			if (velocity > _max_velocity) {
				_max_velocity = velocity;
			}
		}
	}

	_kd_tree = std::make_unique<Kdtree::KdTree>(&_nodes);
}

Frame MatchingDatabase::search(const Skeleton3D& skeleton, float yaw, const Vector2& left_input) const
{
	Vector2 velocity = left_input * _max_velocity;

	Feature feature(skeleton, velocity, yaw, _frame_time);
	Kdtree::CoordPoint feature_vector;
	feature.get_coord_point(feature_vector);

	Kdtree::KdNodeVector node;
	_kd_tree->k_nearest_neighbors(feature_vector, 1, &node);
	
	return _frames[node[0].index];
}

void MatchingDatabase::_add_feature(const Animation& animation, size_t frame, size_t index)
{
	Kdtree::KdNode& node = _nodes.emplace_back();
	node.index = index;

	Feature feature(animation, frame);
	feature.get_coord_point(node.point);
}

bool Frame::operator==(const Frame& rhs) const
{
	return animation == rhs.animation && frame == rhs.frame;
}

bool Frame::operator!=(const Frame& rhs) const
{
	return !(*this == rhs);
}

Feature::Feature(const Skeleton3D& skeleton, const Vector2& velocity, float yaw, float frame_time)
	: _velocity(velocity.rotated(-yaw))
{

	Vector2 position; // default is 0,0,0
	for (int t = 0; t < k_trajectory_count; t++) {
		for (int s = 0; s < k_trajectory_step; s++) {
			position += velocity.rotated(-yaw) * frame_time;
		}
		_trajectories[t] = position;
		_facing[t] = _velocity.normalized();
	}
}

Feature::Feature(const Animation& animation, size_t frame)
{
	Vector3 position; // default is 0,0,0
	_velocity.y = -animation.root.velocity[frame].x;
	_velocity.x = -animation.root.velocity[frame].z;
	float facing = 0;
	for (int t = 0; t < k_trajectory_count; t++) {
		// find a trajectory position every ten frames
		for (int s = 0; s < k_trajectory_step; s++) {
			Vector3 velocity = animation.root.velocity[frame].rotated(Vector3(0, 1, 0), -facing);
			position += velocity * animation.frame_time;
			facing += animation.root.yaw_rate[frame] * animation.frame_time;
			frame++;
		}
		_trajectories[t].y = -position.x;
		_trajectories[t].x = -position.z;
		_facing[t] = Vector2(0, 1).rotated(facing);
	}
}

void Feature::get_coord_point(Kdtree::CoordPoint& coord_point) const
{
	for (int t = 0; t < k_trajectory_count; t++) {
		coord_point.push_back(_trajectories[t].x);
		coord_point.push_back(_trajectories[t].y);
	}

	for (int t = 0; t < k_trajectory_count; t++) {
		coord_point.push_back(_facing[t].x);
		coord_point.push_back(_facing[t].y);
	}

	coord_point.push_back(_velocity.x);
	coord_point.push_back(_velocity.y);
}
