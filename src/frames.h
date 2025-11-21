#pragma once

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/quaternion.hpp>

#include <string>
#include <vector>

using namespace godot;

class JointFrames {
public:
	JointFrames(int parent);
	JointFrames(const std::string& name, int parent, const Vector3& offset);

	void set_rotation(int frame, const Quaternion& rotation);
	void resize(int size);
	void set_name(const std::string& name);
	void set_offset(const Vector3& offset);

private:
	int _parent;
	std::string _name;
	Vector3 _offset;
	std::vector<Quaternion> _rotations;
};

class RootFrames : public JointFrames {
public:
	RootFrames();

	void set_position(int frame, const Vector3& position);
	void resize(int size);

private:
	std::vector<Vector3> _positions;
};

class Frames {
public:
	Frames();

	void set_frame_count(size_t count);
	void set_frame_time(float time);
	RootFrames& get_root();
	JointFrames& get_joint(int joint);
	int add_joint(const std::string& name, int parent, const Vector3& offset);
	void resize(int size);

private:
	size_t _frame_count = 0;
	float _frame_time = 0;
	RootFrames _root;
	std::vector<JointFrames> _joints;
};