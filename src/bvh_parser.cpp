#include "bvh_parser.h"

#include "frame_data.h"

#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/file_access.hpp>

BVHParser::BVHParser(const String& file_path)
{
	_stream = std::istringstream(FileAccess::get_file_as_string(file_path).ascii().get_data());
}

bool BVHParser::parse(FrameData& frames)
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

const std::string& BVHParser::_string()
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

bool BVHParser::_channels(JointData& joint)
{
	_expect("CHANNELS");
	int count;
	if (!_int(count)) {
		return false;
	}

	for (int i = 0; i < count; i++) {
		std::string_view channel_name = _string();
		if (channel_name == "Xrotation") {
			joint.add_rot_channel(RotationChannel::RotationX);
			continue;
		}
		if (channel_name == "Yrotation") {
			joint.add_rot_channel(RotationChannel::RotationY);
			continue;
		}
		if (channel_name == "Zrotation") {
			joint.add_rot_channel(RotationChannel::RotationZ);
			continue;
		}
		_errors.append(vformat("Invalid channel name \"%s\"", _input.c_str()));
		return false;
	}
	return true;
}

bool BVHParser::_channels(RootData& root)
{
	_expect("CHANNELS");
	int count;
	if (!_int(count)) {
		return false;
	}

	for (int i = 0; i < count; i++) {
		std::string_view channel_name = _string();
		if (channel_name == "Xrotation") {
			root.add_rot_channel(RotationChannel::RotationX);
			continue;
		}
		if (channel_name == "Yrotation") {
			root.add_rot_channel(RotationChannel::RotationY);
			continue;
		}
		if (channel_name == "Zrotation") {
			root.add_rot_channel(RotationChannel::RotationZ);
			continue;
		}
		if (channel_name == "Xposition") {
			root.add_pos_channel(PositionChannel::PositionX);
			continue;
		}
		if (channel_name == "Yposition") {
			root.add_pos_channel(PositionChannel::PositionY);
			continue;
		}
		if (channel_name == "Zposition") {
			root.add_pos_channel(PositionChannel::PositionZ);
			continue;
		}
		_errors.append(vformat("Invalid channel name \"%s\"", _input.c_str()));
		return false;
	}
	return true;
}

void BVHParser::_parse_joints(FrameData& frames)
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

void BVHParser::_parse_hierarchy(FrameData& frames)
{
	_expect("HIERARCHY");
	if (_expect("ROOT")) _parse_root(frames);
}

void BVHParser::_parse_root(FrameData& frames)
{
	_string();
	frames.get_root().set_name("Model_" + _input);
	_expect("{");

	Vector3 offset;
	_offset(offset);
	frames.get_root().set_offset(offset);

	_channels(frames.get_root());

	_parse_joints(frames);
	if (_input != "}") {
		_errors.append("Expected end of ROOT.");
	}
}

void BVHParser::_parse_joint(FrameData& frames)
{
	JointData& joint = frames.add_joint();

	std::string joint_name = _string();
	joint.set_name("Model_" + joint_name);
	_expect("{");

	Vector3 offset;
	_offset(offset);
	joint.set_offset(offset);


	_channels(joint);

	_parse_joints(frames);
	if (_input != "}") {
		_errors.append("Expected end of JOINT");
	}
}

void BVHParser::_parse_motion(FrameData& frames)
{
	_expect("MOTION");

	// parse frames
	_expect("FrameData:");
	int frame_count;
	_int(frame_count);
	frames.resize(frame_count);

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

void BVHParser::_parse_frame(int frame, FrameData& frames)
{
	// parse root frame position
	_parse_frame_positions(frame, frames.get_root());
	_parse_frame_rotations(frame, frames.get_root());

	for (int jt = 0; jt < frames.get_joint_count(); jt++) {
		_parse_frame_rotations(frame, frames.get_joint(jt));
	}

	_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void BVHParser::_parse_frame_positions(int frame, RootData& root)
{
	for (auto pos_channel : root.get_pos_channels()) {
		switch (pos_channel) {
		case PositionChannel::PositionX:
			float x;
			_stream >> x;
			if (_stream.fail()) {
				_errors.append("Expected XPosition number.");
			}
			root.set_pos_x(frame, x);
			break;
		case PositionChannel::PositionY:
			float y;
			_stream >> y;
			if (_stream.fail()) {
				_errors.append("Expected YPosition number.");
			}
			root.set_pos_y(frame, y);
			break;
		case PositionChannel::PositionZ:
			float z;
			_stream >> z;
			if (_stream.fail()) {
				_errors.append("Expected ZPosition number.");
			}
			root.set_pos_z(frame, z);
			break;
		}
	}
}

void BVHParser::_parse_frame_rotations(int frame, JointData& joint)
{
	for (auto rot_channel : joint.get_rot_channels()) {
		switch (rot_channel) {
		case RotationChannel::RotationX:
			float x;
			_stream >> x;
			if (_stream.fail()) {
				_errors.append("Expected XPosition number.");
			}
			joint.set_rot_x(frame, x);
			break;
		case RotationChannel::RotationY:
			float y;
			_stream >> y;
			if (_stream.fail()) {
				_errors.append("Expected YPosition number.");
			}
			joint.set_rot_y(frame, y);
			break;
		case RotationChannel::RotationZ:
			float z;
			_stream >> z;
			if (_stream.fail()) {
				_errors.append("Expected ZPosition number.");
			}
			joint.set_rot_z(frame, z);
			break;
		}
	}
}
