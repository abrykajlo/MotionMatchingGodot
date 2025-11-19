#include "animations.h"

void Animations::set_data_path(const String& data_path)
{
	_data_path = data_path;
}

const String& Animations::get_data_path()
{
	return _data_path;
}

void Animations::parse(PackedStringArray* errors)
{
	if (errors) {
		errors->append("errors update");
	}
}

void Animations::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("get_data_path"), &Animations::get_data_path);
	godot::ClassDB::bind_method(D_METHOD("set_data_path", "data_path"), &Animations::set_data_path);
	godot::ClassDB::add_property(get_class_static(), PropertyInfo(Variant::STRING, "data_path", godot::PROPERTY_HINT_DIR, ""), "set_data_path", "get_data_path");
}