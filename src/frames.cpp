#include "frames.h"

#include <cassert>

Frames::Frames() {}

void Frames::set_frame_count(size_t count)
{
	_frame_count = count;
	resize(count);
}

void Frames::set_frame_time(float time)
{
	_frame_time = time;
}

RootFrames& Frames::get_root()
{
	return _root;
}


const RootFrames& Frames::get_root() const
{
	return _root;
}

JointFrames& Frames::get_joint(int joint)
{
	assert(joint - 1 < static_cast<int>(_joints.size()));
	if (joint == 0) {
		return _root;
	}
	return _joints[joint - 1];
}

const JointFrames& Frames::get_joint(int joint) const
{
	assert(joint - 1 < static_cast<int>(_joints.size()));
	if (joint == 0) {
		return _root;
	}
	return _joints[joint - 1];
}

void Frames::add_joint(const std::string& name, const Vector3& offset)
{
	_joints.emplace_back(name, offset);
}

void Frames::resize(int size)
{
	_root.resize(size);
	for (auto& joint : _joints) {
		joint.resize(size);
	}
}

void Frames::setup_skeleton(Skeleton3D& skeleton)
{
	int32_t root_id = skeleton.find_bone(_root.get_name().c_str());
	assert(root_id != -1);
	_root.set_id(root_id);

	for (auto& joint : _joints) {
		int32_t id = skeleton.find_bone(joint.get_name().c_str());
		assert(id != -1);
		joint.set_id(id);
	}
}

void Frames::move_skeleton(Skeleton3D& skeleton, double time) const
{
	int frame = static_cast<int>(time / _frame_time);
	int next_frame = frame + 1;
	float weight = time - static_cast<float>(frame) * _frame_time;
	UtilityFunctions::print(frame);

	skeleton.set_bone_pose_rotation(_root.get_id(), _root.get_rotation(frame));

	for (const auto& joint : _joints) {
		skeleton.set_bone_pose_rotation(joint.get_id(), joint.get_rotation(frame));
	}
}

JointFrames::JointFrames(const std::string& name, const Vector3& offset) : _name(name), _offset(offset) {}

void JointFrames::set_rotation(int frame, const Quaternion& rotation)
{
	assert(frame < _rotations.size());
	_rotations[frame] = rotation;
}

void JointFrames::set_offset(const Vector3& offset) {
	_offset = offset;
}

void JointFrames::set_id(int32_t id)
{
	_id = id;
}

int32_t JointFrames::get_id() const {
	return _id;
}

const Vector3& JointFrames::get_offset() const
{
	return _offset;
}

const Quaternion& JointFrames::get_rotation(int frame) const
{
	assert(frame < _rotations.size());
	return _rotations[frame];
}

const std::string& JointFrames::get_name() const
{
	return _name;
}

void JointFrames::resize(int size)
{
	_rotations.resize(size);
}

void JointFrames::set_name(const std::string& name)
{
	_name = name;
}

RootFrames::RootFrames() : JointFrames() {}

void RootFrames::set_position(int frame, const Vector3& position)
{
	assert(frame < _positions.size());
	_positions[frame].x = position.x;
}

const Vector3& RootFrames::get_position(int frame) const
{
	assert(frame < _positions.size());
	return _positions[frame];
}

void RootFrames::resize(int size)
{
	_positions.resize(size);
	JointFrames::resize(size);
}
