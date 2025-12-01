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

			const float yaw_rate = animation.root.yaw_rate[f];
			if (yaw_rate > _max_yaw_rate) {
				_max_yaw_rate = yaw_rate;
			}
		}
	}

	_kd_tree = std::make_unique<Kdtree::KdTree>(&_nodes);
}

Frame MatchingDatabase::search(const Skeleton3D& skeleton, const Vector2& left_input, float right_input) const
{
	const float yaw_rate = right_input * _max_yaw_rate;
	Vector2 velocity = left_input * _max_velocity;

	Kdtree::CoordPoint feature_vector;
	_get_trajectories(feature_vector, velocity, yaw_rate);

	Kdtree::KdNodeVector node;
	_kd_tree->k_nearest_neighbors(feature_vector, 1, &node);
	
	return _frames[node[0].index];
}

void MatchingDatabase::_add_feature(const Animation& animation, size_t frame, size_t index)
{
	Kdtree::KdNode& node = _nodes.emplace_back();
	node.index = index;

	_get_trajectories(node.point, animation, frame);
}

void MatchingDatabase::_get_trajectories(Kdtree::CoordPoint& feature_vector, const Animation& animation, size_t frame) const
{
	Vector3 position; // default is 0,0,0
	float facing = 0;
	for (int t = 0; t < k_trajectory_count; t++) {
		// find a trajectory position every ten frames
		for (int s = 0; s < k_trajectory_step; s++) {
			Vector3 velocity = animation.root.velocity[frame].rotated(Vector3(0, 1, 0), facing);
			position += velocity * animation.frame_time;
			facing += animation.root.yaw_rate[frame] * animation.frame_time;
			frame++;
		}
		// exclude y coordination since we are projecting on the ground
		feature_vector.push_back(position.x);
		feature_vector.push_back(position.z);
	}
}

void MatchingDatabase::_get_trajectories(Kdtree::CoordPoint& feature_vector, const Vector2& velocity, float yaw_rate) const
{
	Vector2 position; // default is 0,0,0
	float facing = 0;
	for (int t = 0; t < k_trajectory_count; t++) {
		for (int s = 0; s < k_trajectory_step; s++) {
			position += velocity.rotated(facing) * _frame_time;
			facing += yaw_rate * _frame_time;
		}
		feature_vector.push_back(position.x);
		feature_vector.push_back(position.y);
	}
}

bool Frame::operator==(const Frame& rhs) const
{
	return animation == rhs.animation && frame == rhs.frame;
}

bool Frame::operator!=(const Frame& rhs) const
{
	return !(*this == rhs);
}
