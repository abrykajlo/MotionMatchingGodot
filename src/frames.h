#pragma once

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/quaternion.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>

#include <string>
#include <vector>

using namespace godot;

enum class RotationChannel {
	RotationX,
	RotationY,
	RotationZ,
};

enum class PositionChannel {
	PositionX,
	PositionY,
	PositionZ,
};

class JointFrames {
public:
	JointFrames() = default;

	void resize(int size);

	void set_rot_x(int frame, float rotation);
	void set_rot_y(int frame, float rotation);
	void set_rot_z(int frame, float rotation);
	void set_name(const std::string& name);
	void set_offset(const Vector3& offset);
	void set_id(int32_t id);
	void add_rot_channel(RotationChannel channel);

	const Vector3& get_offset() const;
	Quaternion get_rotation(int frame) const;
	const std::string& get_name() const;
	int32_t get_id() const;
	const std::vector<RotationChannel>& get_rot_channels() const;

private:
	int32_t _id = -1;
	std::string _name;
	Vector3 _offset;
	std::vector<RotationChannel> _rot_channels;
	std::vector<float> _rot_x;
	std::vector<float> _rot_y;
	std::vector<float> _rot_z;
};

class RootFrames : public JointFrames {
public:
	RootFrames();

	void resize(int size);

	void set_pos_x(int frame, float x);
	void set_pos_y(int frame, float y);
	void set_pos_z(int frame, float z);
	void add_pos_channel(PositionChannel channel);
	
	Vector3 get_position(int frame) const;
	const std::vector<PositionChannel>& get_pos_channels() const;

private:
	std::vector<PositionChannel> _pos_channels;
	std::vector<float> _pos_x;
	std::vector<float> _pos_y;
	std::vector<float> _pos_z;
};

class Frames {
public:
	Frames();

	// for parsing
	void set_frame_count(size_t count);
	void set_frame_time(float time);
	RootFrames& get_root();
	const RootFrames& get_root() const;
	int get_joint_count() const;
	JointFrames& get_joint(int size);
	JointFrames& add_joint();
	void resize(int size);

	// for skeletons
	void setup_skeleton(Skeleton3D& skeleton);
	void move_skeleton(Skeleton3D& skeleton, double frame_time) const;

private:
	size_t _frame_count = 0;
	float _frame_time = 0;
	RootFrames _root;
	std::vector<JointFrames> _joints;
};