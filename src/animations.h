#pragma once

#include <godot_cpp/classes/resource.hpp>

using namespace godot;

class Animations : public Resource {
	GDCLASS(Animations, Resource)

public:
	void set_data_path(const String& files);
	const String& get_data_path();

	void parse(PackedStringArray* errors = nullptr);

protected:
	String _data_path;

	static void _bind_methods();
};