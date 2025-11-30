#include "animation_database.h"

#include "constants.h"
#include "frame.h"
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

void AnimationDatabase::move(godot::Skeleton3D& skeleton, const Frame& from, const Frame& to, float playback_timer, float blend_timer) const
{
	const Animation& A = _animations[from.animation];
	const Animation& B = _animations[to.animation];

	// sample A
	int baseA = int(playback_timer / A.frame_time);
	int iA0 = baseA + from.frame;
	int iA1 = iA0 + 1;
	float tA = fmod(playback_timer, A.frame_time) / A.frame_time;

	// sample B
	int baseB = int(playback_timer / B.frame_time);
	int iB0 = baseB + to.frame;
	int iB1 = iB0 + 1;
	float tB = fmod(playback_timer, B.frame_time) / B.frame_time;

	float w = Math::clamp(blend_timer / k_blend_time, 0.0f, 1.0f);

	// root
	Quaternion rootA = A.root.rotations[iA0].slerp(A.root.rotations[iA1], tA);
	Quaternion rootB = B.root.rotations[iB0].slerp(B.root.rotations[iB1], tB);

	Quaternion root = rootA.slerp(rootB, w);
	skeleton.set_bone_pose_rotation(A.root.id, root);

	// joints
	for (int i = 0; i < A.joints.size(); ++i)
	{
		Quaternion poseA = A.joints[i].rotations[iA0].slerp(A.joints[i].rotations[iA1], tA);
		Quaternion poseB = B.joints[i].rotations[iB0].slerp(B.joints[i].rotations[iB1], tB);

		Quaternion q = poseA.slerp(poseB, w);
		skeleton.set_bone_pose_rotation(A.joints[i].id, q);
	}
}

void AnimationDatabase::move(Skeleton3D& skeleton, const Frame& frame, float playback_timer) const
{
	const Animation& anim = _animations[frame.animation];
	
	const float frame_time = anim.frame_time;

	// compute sample position
	int base = int(playback_timer / frame_time);
	int i0 = frame.frame + base;
	int i1 = i0 + 1;
	
	float t = fmod(playback_timer, frame_time) / frame_time;
	
	// root
	const Quaternion& root0 = anim.root.rotations[i0];
	const Quaternion& root1 = anim.root.rotations[i1];
	Quaternion root = root0.slerp(root1, t);

	skeleton.set_bone_pose_rotation(anim.root.id, root);

	// joints
	for (const auto& joint : anim.joints) {
		const Quaternion& q0 = joint.rotations[i0];
		const Quaternion& q1 = joint.rotations[i1];
		Quaternion blended = q0.slerp(q1, t);

		skeleton.set_bone_pose_rotation(joint.id, blended);
	}
}


size_t AnimationDatabase::size() const
{
	return _animations.size();
}

const Animation& AnimationDatabase::get(size_t animation) const
{
	return _animations[animation];
}

Animation::Animation(const FrameData& frame_data)
	: root(frame_data.get_root())
	, frame_time(frame_data.get_frame_time())
	, frames(frame_data.size() - 1)
{
	Basis basis(Vector3(0, 0, -1), Vector3(0, 1, 0), Vector3(-1, 0, 0)); // this is the default basis
	const RootData& root_data = frame_data.get_root();
	for (int f = 0; f < frames; f++) {
		//basis.rotate
		root.rotations[f] = root_data.get_rotation(f);
		root.velocity[f] = (root_data.get_position(f + 1) - root_data.get_position(f)) / frame_time;
		root.yaw_rate[f] = (root_data.get_yaw(f + 1) - root_data.get_yaw(f)) / frame_time;
	}
	
	const int joint_count = frame_data.get_joint_count();
	joints.reserve(joint_count);

	for (int jt = 0; jt < joint_count; jt++) {
		joints.emplace_back(frame_data.get_joint(jt));
	}

	for (int jt = 0; jt < joint_count; jt++) {
		for (int f = 0; f < frames; f++) {
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
