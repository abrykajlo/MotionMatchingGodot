#pragma once

#include "frames.h"

#include <godot_cpp/classes/resource.hpp>

#include <unordered_map>

using namespace godot;

class Frames;

class Animations : public Resource {
	GDCLASS(Animations, Resource)

public:
	void set_data_path(const String& files);
	const String& get_data_path();

	void parse(PackedStringArray* errors = nullptr);

protected:
	String _data_path;
	std::unordered_map<std::string, Frames> _animations;

	static void _bind_methods();
};