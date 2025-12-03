#pragma once

#include <godot_cpp/variant/quaternion.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>

#include <vector>

using namespace godot;

class FrameData;
class RootData;
class JointData;

class Animation;
class Root;
class Joint;

struct Frame;

class AnimationDatabase {
public:
	AnimationDatabase();

	void add(const FrameData& frame_data);

	bool setup(Skeleton3D& skeleton);
	void move(Vector3& position, float& yaw, Skeleton3D& skeleton, Frame& frame, float& playback_timer, float delta_time) const;
	void move(Vector3& transform, float& yaw, Skeleton3D& skeleton, const Frame& from, const Frame& to, float playback_timer, float blend_timer, float delta_time) const;

	size_t size() const;
	const Animation& get(size_t animation) const;

private:
	std::vector<Animation> _animations;
};

struct Root {
	Root(const RootData& root_data);

	std::vector<Quaternion> rotations;
	std::vector<Vector3> velocity;
	std::vector<float> height;
	std::vector<float> yaw_rate;

	uint32_t id = -1;
	String name;
	Vector3 offset;
};

struct Joint {
	Joint(const JointData& joint_data);

	std::vector<Quaternion> rotations;
	uint32_t id = -1;
	String name;
	Vector3 offset;
};

struct Animation {
	Animation(const FrameData& frame_data);

	Root root;
	std::vector<Joint> joints;
	float frame_time = 0;
	int frames = 0;
};
