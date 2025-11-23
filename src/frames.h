#pragma once

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/quaternion.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>

#include <string>
#include <vector>

using namespace godot;

class JointFrames {
public:
	JointFrames() = default;
	JointFrames(const std::string& name, const Vector3& offset);

	void set_rotation(int frame, const Quaternion& rotation);
	void resize(int size);
	void set_name(const std::string& name);
	void set_offset(const Vector3& offset);
	void set_id(int32_t id);

	const Vector3& get_offset() const;
	const Quaternion& get_rotation(int frame) const;
	const std::string& get_name() const;
	int32_t get_id() const;

private:
	int32_t _id = -1;
	std::string _name;
	Vector3 _offset;
	std::vector<Quaternion> _rotations;
};

class RootFrames : public JointFrames {
public:
	RootFrames();

	void set_position(int frame, const Vector3& position);
	const Vector3& get_position(int frame) const;
	void resize(int size);

private:
	std::vector<Vector3> _positions;
};

class Frames {
public:
	Frames();

	// for parsing
	void set_frame_count(size_t count);
	void set_frame_time(float time);
	RootFrames& get_root();
	const RootFrames& get_root() const;
	JointFrames& get_joint(int joint);
	const JointFrames& get_joint(int joint) const;
	void add_joint(const std::string& name, const Vector3& offset);
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