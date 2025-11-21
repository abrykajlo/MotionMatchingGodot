#include "bvh_parser.h"

#include "frames.h"

#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/file_access.hpp>

BVHParser::BVHParser(const String& file_path)
{
	_stream = std::istringstream(FileAccess::get_file_as_string(file_path).ascii().get_data());
}

bool BVHParser::parse(Frames& frames)
{
	_parse_hierarchy(frames);
	_parse_motion(frames);
	if (!_is_at_end()) {
		_errors.append(vformat("Expected EOF got %s.", _input.c_str()));
	}
	return _errors.size() == 0;
}

const PackedStringArray& BVHParser::get_errors() const
{
	return _errors;
}

bool BVHParser::_is_at_end()
{
	return _stream.eof();
}

bool BVHParser::_expect(const std::string& str)
{
	if (str != _string()) {
		_errors.append(vformat("Expected \"%s.\"", str.c_str()));
		return false;
	}
	return true;
}

void BVHParser::_next_line()
{
	_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string_view BVHParser::_string()
{
	_stream >> _input;
	return _input;
}

bool BVHParser::_number(float &d)
{
	_stream >> d;
	if (_stream.fail()) {
		_errors.append("Expected floating point number.");
		return false;
	}

	return true;
}

bool BVHParser::_int(int& i)
{
	_stream >> i;
	if (_stream.fail()) {
		_errors.append("Expected integer.");
		return false;
	}

	return true;
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
		std::string_view channel_name = _string();
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
		_errors.append(vformat("Invalid channel name \"%s\"", _input.c_str()));
		return false;
	}
	_frame_parse_states.append(FrameParseState::NEXT_JOINT);
	return true;
}

void BVHParser::_parse_joints(Frames& frames)
{
	int count = 0;
	std::string_view next = _string();
	while (next == "JOINT") {
		_parse_joint(frames);
		next = _string();
		count++;
	}
	if (count == 0) {
		if (next != "End") {
			_errors.append("Expected \"End\".");
		}
		_expect("Site");

		_expect("{");

		Vector3 offset;
		_offset(offset);

		_expect("}");
		next = _string();
	}
}

void BVHParser::_parse_hierarchy(Frames& frames)
{
	_expect("HIERARCHY");
	if (_expect("ROOT")) _parse_root(frames);
	UtilityFunctions::print(vformat("%d frame parse states", _frame_parse_states.size()));
}

void BVHParser::_parse_root(Frames& frames)
{
	std::string_view root_name = _string();
	_expect("{");

	Vector3 offset;
	_offset(offset);

	_channels();

	_parse_joints(frames);
	if (_input != "}") {
		_errors.append("Expected end of ROOT.");
	}
}

void BVHParser::_parse_joint(Frames& frames)
{
	std::string_view joint_name = _string();
	_expect("{");

	Vector3 offset;
	_offset(offset);

	_channels();

	_parse_joints(frames);
	if (_input != "}") {
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

	_next_line();

	// parse frames
	for (int f = 0; f < frame_count; f++) {
		_parse_frame(f, frames);
	}

	_next_line();
}

void BVHParser::_parse_frame(int frame, Frames& frames)
{
	std::getline(_stream, _input);
	std::istringstream frame_stream(_input);
	int joint = 0; // start at the root
	// parse root frame position
	Vector3 position;
	bool done_parsing_position = false;
	int parse_state_idx = 0;
	while (!done_parsing_position) {
		switch (_frame_parse_states[parse_state_idx]) {
		case FrameParseState::X_POSITION:
			frame_stream >> position.x;
			if (frame_stream.fail()) {
				_errors.append("Expected XPosition number.");
			}
			break;
		case FrameParseState::Y_POSITION:
			frame_stream >> position.y;
			if (frame_stream.fail()) {
				_errors.append("Expected YPosition number.");
			}
			break;
		case FrameParseState::Z_POSITION:
			frame_stream >> position.z;
			if (frame_stream.fail()) {
				_errors.append("Expected ZPosition number.");
			}
			break;
		default:
			done_parsing_position = true;
		}
		parse_state_idx++;
	}

	frames.set_root_position(frame, position);
	// parse rotations
	for (; parse_state_idx < _frame_parse_states.size(); parse_state_idx++) {
		Quaternion rotation(0, 0, 0, 1);
		switch (_frame_parse_states[parse_state_idx]) {
		case FrameParseState::X_ROTATION: {
			Vector3 x_euler;
			frame_stream >> x_euler.x;
			if (frame_stream.fail()) {
				_errors.append("Expected XRotation number.");
			}
			auto x = Quaternion::from_euler(x_euler);
			rotation *= x;
			break;
		}
		case FrameParseState::Y_ROTATION: {
			Vector3 y_euler;
			frame_stream >> y_euler.y;
			if (frame_stream.fail()) {
				_errors.append("Expected YRotation number.");
			}
			auto y = Quaternion::from_euler(y_euler);
			rotation *= y;
			break;
		}
		case FrameParseState::Z_ROTATION: {
			Vector3 z_euler;
			frame_stream >> z_euler.z;
			if (frame_stream.fail()) {
				_errors.append("Expected ZRotation number.");
			}
			auto z = Quaternion::from_euler(z_euler);
			rotation *= z;
			break;
		}
		case FrameParseState::NEXT_JOINT: {
			frames.set_joint_rotation(frame, joint++, rotation);
			rotation = Quaternion(0, 0, 0, 1); // reset quaternion
			break;
		}
		}
	}

	frame_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	if (!frame_stream.eof()) {
		_errors.append("Expected end of frame.");
	}
}
