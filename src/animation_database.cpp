#include "animation_database.h"

#include "frame_data.h"

AnimationDatabase::AnimationDatabase() {}

void AnimationDatabase::add(const FrameData& frame_data)
{
	_animations.emplace_back(frame_data);
}

bool AnimationDatabase::setup(Skeleton3D& skeleton)
{
	if (_animations.size() == 0) {
		return false;
	}

	Animation& animation = _animations[0];
	animation.root.id = skeleton.find_bone(animation.root.name);

	for (auto& joint : animation.joints) {
		joint.id = skeleton.find_bone(joint.name);
	}

	return true;
}

void AnimationDatabase::move(Skeleton3D& skeleton, int animation, int frame)
{
	const Animation& anim = _animations[animation];

	skeleton.set_bone_pose_rotation(anim.root.id, anim.root.rotations[frame]);
	//auto base = anim.root.offset;
	//auto position = anim.root.positions[frame];
	//skeleton.set_global_position((position - base) * 0.01);

	for (const auto& joint : anim.joints) {
		skeleton.set_bone_pose_rotation(joint.id, joint.rotations[frame]);
	}
}

Animation::Animation(const FrameData& frame_data)
	: root(frame_data.get_root())
	, frame_time(frame_data.get_frame_time())
{
	// subtract one from the frame count since we need the last frame to calculate root linear and angular velocity
	const int frame_count = frame_data.size() - 1;
	const RootData& root_data = frame_data.get_root();
	for (int f = 0; f < frame_count; f++) {
		root.rotations[f] = root_data.get_rotation(f);
		root.velocity[f] = root_data.get_velocity(f);
		root.yaw_rate[f] = root_data.get_yaw_rate(f);
	}
	
	const int joint_count = frame_data.get_joint_count();
	joints.reserve(joint_count);

	for (int jt = 0; jt < joint_count; jt++) {
		joints.emplace_back(frame_data.get_joint(jt));
	}

	for (int jt = 0; jt < joint_count; jt++) {
		for (int f = 0; f < frame_count; f++) {
			joints[jt].rotations[f] = frame_data.get_joint(jt).get_rotation(f);
		}
	}
}

Root::Root(const RootData& root_data)
	: name(root_data.get_name().c_str())
	, offset(root_data.get_offset()) 
{
	const size_t frame_count = root_data.size();
	rotations.resize(frame_count);
	velocity.resize(frame_count);
	yaw_rate.resize(frame_count);
}

Joint::Joint(const JointData& joint_data)
	: name(joint_data.get_name().c_str())
	, offset(joint_data.get_offset())
{
	const size_t frame_count = joint_data.size();
	rotations.resize(frame_count);
}
