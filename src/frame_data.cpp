#include "frame_data.h"

FrameData::FrameData() {}

void FrameData::set_frame_time(float time)
{
	_frame_time = time;
}

float FrameData::get_frame_time() const
{
	return _frame_time;
}

RootData& FrameData::get_root()
{
	return _root;
}


const RootData& FrameData::get_root() const
{
	return _root;
}

int FrameData::get_joint_count() const
{
	return _joints.size();
}

JointData& FrameData::get_joint(int jt) 
{
	return _joints[jt];
}

const JointData& FrameData::get_joint(int jt) const
{
	return _joints[jt];
}

JointData& FrameData::add_joint()
{
	return _joints.emplace_back();
}

size_t FrameData::size() const 
{
	return _frame_count;
}

void FrameData::resize(size_t size)
{
	_frame_count = size;
	_root.resize(size);
	for (auto& joint : _joints) {
		joint.resize(size);
	}
}

void JointData::set_rot_x(int frame, float rotation)
{
	_rot_x[frame] = rotation;
}

void JointData::set_rot_y(int frame, float rotation)
{
	_rot_y[frame] = rotation;
}

void JointData::set_rot_z(int frame, float rotation)
{
	_rot_z[frame] = rotation;
}

void JointData::set_offset(const Vector3& offset) {
	_offset = offset;
}

void JointData::add_rot_channel(RotationChannel channel)
{
	_rot_channels.push_back(channel);
}

const std::vector<RotationChannel>& JointData::get_rot_channels() const
{
	return _rot_channels;
}

const Vector3& JointData::get_offset() const
{
	return _offset;
}

Quaternion JointData::get_rotation(int frame) const
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

Quaternion RootData::get_rotation(int frame) const
{
	Quaternion qy(Vector3(0, 1, 0), Math::deg_to_rad(_rot_y[frame]));
	Quaternion qz(Vector3(0, 0, 1), Math::deg_to_rad(_rot_z[frame]));
	Quaternion rotation;

	for (auto channel : _rot_channels) {
		switch (channel) {
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

float RootData::get_yaw(int frame) const
{
	return Math::deg_to_rad(_rot_x[frame]);
}

const std::string& JointData::get_name() const
{
	return _name;
}

void JointData::resize(int size)
{
	_rot_x.resize(size);
	_rot_y.resize(size);
	_rot_z.resize(size);
}

size_t JointData::size() const
{
	return _rot_x.size();
}

void JointData::set_name(const std::string& name)
{
	_name = name;
}

RootData::RootData() : JointData() {}

void RootData::set_pos_x(int frame, float x)
{
	_pos_x[frame] = x;
}

void RootData::set_pos_y(int frame, float y)
{
	_pos_y[frame] = y;
}

void RootData::set_pos_z(int frame, float z)
{
	_pos_z[frame] = z;
}

void RootData::add_pos_channel(PositionChannel channel)
{
	_pos_channels.push_back(channel);
}

Vector3 RootData::get_position(int frame) const
{
	return Vector3(_pos_x[frame], _pos_y[frame], _pos_z[frame]) * 0.01;
}

const std::vector<PositionChannel>& RootData::get_pos_channels() const
{
	return _pos_channels;
}

void RootData::resize(int size)
{
	_pos_x.resize(size);
	_pos_y.resize(size);
	_pos_z.resize(size);
	JointData::resize(size);
}
