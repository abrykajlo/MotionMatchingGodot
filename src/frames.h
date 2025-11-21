#pragma once

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/vector3.hpp>

using namespace godot;

class JointFrames {
public:
	void set_rotation(int frame, const Quaternion& rotation);

private:
	String _name;
	Vector3 _offset;
	Vector<Quaternion> _rotations;
};

class RootFrames : public JointFrames {
public:
	void set_position(int frame, const Vector3& position);

private:
	Vector<Vector3> _positions;
};

class Frames {
public:
	void set_frame_count(size_t count);
	void set_frame_time(float time);
	void set_root_position(int frame, const Vector3& position);
	void set_joint_rotation(int frame, int joint, const Quaternion& rotation);

private:
	size_t _frame_count = 0;
	float _frame_time = 0;
	RootFrames _root;
	Vector<JointFrames> _joints;
};