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

class AnimationDatabase {
public:
	AnimationDatabase();

	void add(const FrameData& frame_data);

	bool setup(godot::Skeleton3D& skeleton);
	void move(godot::Skeleton3D& skeleton, int animation, int frame);

private:
	std::vector<Animation> _animations;
};

struct Root {
	Root(const RootData& root_data);

	std::vector<Quaternion> rotations;
	std::vector<Vector3> velocity;
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
};
