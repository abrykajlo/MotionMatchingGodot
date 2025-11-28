#pragma once

#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/templates/vector.hpp>

#include <sstream>

using namespace godot;

class FrameData;
class RootData;
class JointData;

class BVHParser {
public:
	BVHParser(const String& file_path);

	bool parse(FrameData& frames);

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
	bool _channels(RootData& root);
	bool _channels(JointData& joint);

	void _parse_joints(FrameData& frames);
	void _parse_hierarchy(FrameData& frames);
	void _parse_root(FrameData& frames);
	void _parse_joint(FrameData& frames);

	void _parse_motion(FrameData& frames);
	void _parse_frame(int frame, FrameData& frames);
	void _parse_frame_positions(int frame, RootData& root);
	void _parse_frame_rotations(int frame, JointData& joint);

	std::istringstream _stream;
	std::string _input;
	PackedStringArray _errors;
};