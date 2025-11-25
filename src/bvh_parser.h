#pragma once

#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/templates/vector.hpp>

#include <sstream>

using namespace godot;

class Frames;
class RootFrames;
class JointFrames;

class BVHParser {
public:
	BVHParser(const String& file_path);

	bool parse(Frames& frames);

	const PackedStringArray& get_errors() const;

private:
	// parsing character functions
	bool _is_at_end();
	bool _expect(const std::string& str);
	void _next_line();

	const std::string& _string();
	bool _number(float& d);
	bool _int(int& i);
	bool _offset(Vector3& offset);
	bool _channels(RootFrames& root);
	bool _channels(JointFrames& joint);

	void _parse_joints(Frames& frames);
	void _parse_hierarchy(Frames& frames);
	void _parse_root(Frames& frames);
	void _parse_joint(Frames& frames);

	void _parse_motion(Frames& frames);
	void _parse_frame(int frame, Frames& frames);
	void _parse_frame_positions(int frame, RootFrames& root);
	void _parse_frame_rotations(int frame, JointFrames& joint);

	std::istringstream _stream;
	std::string _input;
	PackedStringArray _errors;
};