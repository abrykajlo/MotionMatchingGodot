#pragma once

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>

using namespace godot;

class BiovisionHierarchy : public Resource {
	GDCLASS(BiovisionHierarchy, Resource)

protected:
	String _file_path = "";
	Skeleton3D _skeleton;

	static void _bind_methods();

public:
	BiovisionHierarchy() = default;
	virtual ~BiovisionHierarchy() override;

	void set_file_path(const String& file_path);
	const String& get_file_path();
};