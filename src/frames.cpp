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

JointFrames& Frames::get_joint(int joint)
{
	assert(joint - 1 < static_cast<int>(_joints.size()));
	if (joint == 0) {
		return _root;
	}
	return _joints[joint - 1];
}

int Frames::add_joint(const std::string& name, int parent, const Vector3& offset)
{
	_joints.emplace_back(name, parent, offset);
	return _joints.size();
}

void Frames::resize(int size)
{
	_root.resize(size);
	for (auto& joint : _joints) {
		joint.resize(size);
	}
}

JointFrames::JointFrames(int parent) : _parent(parent) {}

JointFrames::JointFrames(const std::string& name, int parent, const Vector3& offset) : _name(name), _parent(parent), _offset(offset) {}

void JointFrames::set_rotation(int frame, const Quaternion& rotation)
{
	assert(frame < _rotations.size());
	_rotations[frame] = rotation;
}

void JointFrames::set_offset(const Vector3& offset) {
	_offset = offset;
}

void JointFrames::resize(int size)
{
	_rotations.resize(size);
}

void JointFrames::set_name(const std::string& name)
{
	_name = name;
}

RootFrames::RootFrames() : JointFrames(-1) {}

void RootFrames::set_position(int frame, const Vector3& position)
{
	assert(frame < _positions.size());
	_positions[frame].x = position.x;
}

void RootFrames::resize(int size)
{
	_positions.resize(size);
	JointFrames::resize(size);
}
