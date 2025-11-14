#include "biovision_hierarchy.h"

void BiovisionHierarchy::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("set_file", "file_path"), &BiovisionHierarchy::set_file_path);
	godot::ClassDB::bind_method(D_METHOD("get_file"), &BiovisionHierarchy::get_file_path);
	godot::ClassDB::add_property(get_class_static(), PropertyInfo(Variant::STRING, "file_path", PROPERTY_HINT_FILE, "*.bvh"), "set_file", "get_file");
}

BiovisionHierarchy::~BiovisionHierarchy() {}

void BiovisionHierarchy::set_file_path(const String& file_path) {
	_file_path = file_path;
}

const String& BiovisionHierarchy::get_file_path() {
	return _file_path;
}