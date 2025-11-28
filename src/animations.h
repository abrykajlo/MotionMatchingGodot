#pragma once

#include "frame_data.h"
#include "animation_database.h"

#include <godot_cpp/classes/resource.hpp>

#include <vector>

using namespace godot;

class FrameData;

class Animations : public Resource {
	GDCLASS(Animations, Resource)

public:
	void set_data_path(const String& files);
	const String& get_data_path();

	AnimationDatabase parse(PackedStringArray* errors = nullptr);

protected:
	String _data_path;

	static void _bind_methods();
};