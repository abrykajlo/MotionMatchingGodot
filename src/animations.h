#pragma once

#include "frames.h"

#include <godot_cpp/classes/resource.hpp>

#include <vector>

using namespace godot;

class Frames;

class Animations : public Resource {
	GDCLASS(Animations, Resource)

public:
	void set_data_path(const String& files);
	const String& get_data_path();

	void parse(PackedStringArray* errors = nullptr);
	const Frames& get(int animation) const;
	size_t size();
	void setup_skeleton(Skeleton3D& skeleton);

protected:
	String _data_path;
	std::vector<Frames> _animations;

	static void _bind_methods();
};