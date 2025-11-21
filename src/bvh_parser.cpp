#include "bvh_parser.h"

#include "frames.h"

#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/file_access.hpp>

BVHParser::BVHParser(const String& file_path)
{
	_source = FileAccess::get_file_as_string(file_path);
}

bool BVHParser::parse(Frames& frames)
{
	_parse_hierarchy(frames);
	_parse_motion(frames);
	if (!_is_at_end()) {
		_errors.append("Expected EOF.");
	}
	return _errors.size() == 0;
}

const PackedStringArray& BVHParser::get_errors() const
{
	return _errors;
}

bool BVHParser::_is_whitespace()
{
	switch (_source[_curr]) {
	case ' ':
	case '\r':
	case '\t':
	case '\n':
		return true;
	}
	return false;
}

void BVHParser::_skip_whitespace()
{
	for (;;) {
		if (_is_at_end()) return;
		if (_is_whitespace()) {
			_curr++;
			continue;
		}
		break;
	}
}

bool BVHParser::_is_at_end()
{
	return _curr >= _source.length();
}

bool BVHParser::_expect(const String& str)
{
	if (str != _string()) {
		_errors.append(vformat("Expected \"%s.\"", str));
		return false;
	}
	return true;
}

String BVHParser::_string()
{
	_skip_whitespace();
	_start = _curr;
	for (;;) {
		if (_is_whitespace()) {
			break;
		}
		_curr++;
	}
	return _source.substr(_start, _curr - _start);
}

bool BVHParser::_number(float &d)
{
	String str = _string();
	if (str.is_valid_float()) {
		d = str.to_float();
		return true;
	}

	_errors.append("Expected floating point number.");
	return false;
}

bool BVHParser::_int(int& i)
{
	String str = _string();
	if (str.is_valid_int()) {
		i = str.to_int();
		return true;
	}

	_errors.append("Expected integer.");
	return false;
}

bool BVHParser::_offset(Vector3& offset)
{
	return _expect("OFFSET") && _number(offset.x) && _number(offset.y) && _number(offset.z);
}

bool BVHParser::_channels()
{
	_expect("CHANNELS");
	int count;
	if (!_int(count)) {
		return false;
	}

	for (int i = 0; i < count; i++) {
		String channel_name = _string();
		if (channel_name == "Xrotation") {
			_frame_parse_states.append(FrameParseState::X_ROTATION);
			continue;
		}
		if (channel_name == "Yrotation") {
			_frame_parse_states.append(FrameParseState::Y_ROTATION);
			continue;
		}
		if (channel_name == "Zrotation") {
			_frame_parse_states.append(FrameParseState::Z_ROTATION);
			continue;
		}
		if (channel_name == "Xposition") {
			_frame_parse_states.append(FrameParseState::X_POSITION);
			continue;
		}
		if (channel_name == "Yposition") {
			_frame_parse_states.append(FrameParseState::Y_POSITION);
			continue;
		}
		if (channel_name == "Zposition") {
			_frame_parse_states.append(FrameParseState::Z_POSITION);
			continue;
		}
		_errors.append(vformat("Invalid channel name \"%s\"", channel_name));
		return false;
	}
	_frame_parse_states.append(FrameParseState::NEXT_JOINT);
	return true;
}

void BVHParser::_parse_joints(Frames& frames)
{
	int count = 0;
	String next = _string();
	while (next == "JOINT") {
		_parse_joint(frames);
		next = _string();
		count++;
	}
	if (count == 0) {
		String site = _string();
		if (next != "End" || site != "Site") {
			_errors.append("Expected \"End Site\".");
		}

		_expect("{");

		Vector3 offset;
		_offset(offset);

		_expect("}");
		next = _string();
	}
	_curr = _start;
}

void BVHParser::_parse_hierarchy(Frames& frames)
{
	_expect("HIERARCHY");
	if (_expect("ROOT")) _parse_root(frames);
	UtilityFunctions::print(vformat("%d frame parse states", _frame_parse_states.size()));
}

void BVHParser::_parse_root(Frames& frames)
{
	String root_name = _string();
	_expect("{");

	Vector3 offset;
	_offset(offset);

	_channels();

	_parse_joints(frames);
	if (_string() != "}") {
		_errors.append("Expected end of ROOT.");
	}
}

void BVHParser::_parse_joint(Frames& frames)
{
	String joint_name = _string();
	_expect("{");

	Vector3 offset;
	_offset(offset);

	_channels();

	_parse_joints(frames);
	if (_string() != "}") {
		_errors.append("Expected end of JOINT");
	}
}

void BVHParser::_parse_motion(Frames& frames)
{
	_expect("MOTION");

	// parse frames
	_expect("Frames:");
	int frame_count;
	_int(frame_count);
	frames.set_frame_count(frame_count);

	// parse frame time
	_expect("Frame");
	_expect("Time:");
	float frame_time;
	_number(frame_time);
	frames.set_frame_time(frame_time);

	// parse frames
	for (int f = 0; f < frame_count; f++) {
		int joint = 0; // start at the root
		// parse root frame position
		Vector3 position;
		bool done_parsing_position = false;
		int parse_state_idx = 0;
		while (!done_parsing_position) {
			switch (_frame_parse_states[parse_state_idx]) {
			case FrameParseState::X_POSITION:
				_number(position.x);
				break;
			case FrameParseState::Y_POSITION:
				_number(position.y);
				break;
			case FrameParseState::Z_POSITION:
				_number(position.z);
				break;
			default:
				done_parsing_position = true;
			}
			parse_state_idx++;
		}

		frames.set_root_position(f, position);
		// parse rotations
		for (; parse_state_idx < _frame_parse_states.size(); parse_state_idx++) {
			Quaternion rotation(0, 0, 0, 1);
			switch (_frame_parse_states[parse_state_idx]) {
			case FrameParseState::X_ROTATION: {
				Vector3 x_euler;
				_number(x_euler.x);
				auto x = Quaternion::from_euler(x_euler);
				rotation *= x;
				break;
			}
			case FrameParseState::Y_ROTATION: {
				Vector3 y_euler;
				_number(y_euler.y);
				auto y = Quaternion::from_euler(y_euler);
				rotation *= y;
				break;
			}
			case FrameParseState::Z_ROTATION: {
				Vector3 z_euler;
				_number(z_euler.z);
				auto z = Quaternion::from_euler(z_euler);
				rotation *= z;
				break;
			}
			case FrameParseState::NEXT_JOINT: {
				frames.set_joint_rotation(f, joint++, rotation);
				rotation = Quaternion(0, 0, 0, 1); // reset quaternion
				break;
			}
			}
		}
	}
	_skip_whitespace();
}
