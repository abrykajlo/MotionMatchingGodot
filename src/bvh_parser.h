#pragma once

#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

class Frames;

enum class FrameParseState {
	X_POSITION,
	Y_POSITION,
	Z_POSITION,

	X_ROTATION,
	Y_ROTATION,
	Z_ROTATION,

	NEXT_JOINT,
};

class BVHParser {
public:
	BVHParser(const String& file_path);

	bool parse(Frames& frames);

	const PackedStringArray& get_errors() const;

private:
	// parsing character functions
	bool _is_whitespace();
	void _skip_whitespace();
	bool _is_at_end();
	bool _expect(const String& str);
	
	String _string();
	bool _number(float& d);
	bool _int(int& i);
	bool _offset(Vector3& offset);
	bool _channels();

	void _parse_joints(Frames& frames);
	void _parse_hierarchy(Frames& frames);
	void _parse_root(Frames& frames);
	void _parse_joint(Frames& frames);

	void _parse_motion(Frames& frames);

	size_t _start = 0;
	size_t _curr = 0;
	String _source;
	Vector<FrameParseState> _frame_parse_states;
	PackedStringArray _errors;
};