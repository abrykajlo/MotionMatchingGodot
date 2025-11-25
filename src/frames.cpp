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

int Frames::get_joint_count() const
{
	return _joints.size();
}

JointFrames& Frames::get_joint(int i) 
{
	return _joints[i];
}

JointFrames& Frames::add_joint()
{
	return _joints.emplace_back();
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

	skeleton.set_bone_pose_rotation(_root.get_id(), _root.get_rotation(frame));
	skeleton.set_global_position(_root.get_position(frame));

	for (const auto& joint : _joints) {
		skeleton.set_bone_pose_rotation(joint.get_id(), joint.get_rotation(frame));
	}
}

void JointFrames::set_rot_x(int frame, float rotation)
{
	assert(frame < _rot_x.size());
	_rot_x[frame] = rotation;
}

void JointFrames::set_rot_y(int frame, float rotation)
{
	assert(frame < _rot_y.size());
	_rot_y[frame] = rotation;
}

void JointFrames::set_rot_z(int frame, float rotation)
{
	assert(frame < _rot_z.size());
	_rot_z[frame] = rotation;
}

void JointFrames::set_offset(const Vector3& offset) {
	_offset = offset;
}

void JointFrames::set_id(int32_t id)
{
	_id = id;
}

void JointFrames::add_rot_channel(RotationChannel channel)
{
	_rot_channels.push_back(channel);
}

int32_t JointFrames::get_id() const {
	return _id;
}

const std::vector<RotationChannel>& JointFrames::get_rot_channels() const
{
	return _rot_channels;
}

const Vector3& JointFrames::get_offset() const
{
	return _offset;
}

Quaternion JointFrames::get_rotation(int frame) const
{
	Quaternion qx(Vector3(1, 0, 0), Math::deg_to_rad(_rot_x[frame]));
	Quaternion qy(Vector3(0, 1, 0), Math::deg_to_rad(_rot_y[frame]));
	Quaternion qz(Vector3(0, 0, 1), Math::deg_to_rad(_rot_z[frame]));
	Quaternion rotation;

	for (auto channel : _rot_channels) {
		switch (channel) {
		case RotationChannel::RotationX:
			rotation *= qx;
			break;
		case RotationChannel::RotationY:
			rotation *= qy;
			break;
		case RotationChannel::RotationZ:
			rotation *= qz;
			break;
		}
	}

	return rotation;
}

const std::string& JointFrames::get_name() const
{
	return _name;
}

void JointFrames::resize(int size)
{
	_rot_x.resize(size);
	_rot_y.resize(size);
	_rot_z.resize(size);
}

void JointFrames::set_name(const std::string& name)
{
	_name = name;
}

RootFrames::RootFrames() : JointFrames() {}

void RootFrames::set_pos_x(int frame, float x)
{
	assert(frame < _pos_x.size());
	_pos_x[frame] = x;
}

void RootFrames::set_pos_y(int frame, float y)
{
	assert(frame < _pos_y.size());
	_pos_y[frame] = y;
}

void RootFrames::set_pos_z(int frame, float z)
{
	assert(frame < _pos_z.size());
	_pos_z[frame] = z;
}

void RootFrames::add_pos_channel(PositionChannel channel)
{
	_pos_channels.push_back(channel);
}

Vector3 RootFrames::get_position(int frame) const
{
	return Vector3(_pos_x[frame], _pos_y[frame], _pos_z[frame]);
}

const std::vector<PositionChannel>& RootFrames::get_pos_channels() const
{
	return _pos_channels;
}

void RootFrames::resize(int size)
{
	_pos_x.resize(size);
	_pos_y.resize(size);
	_pos_z.resize(size);
	JointFrames::resize(size);
}
