#include "animations.h"
#include "bvh_parser.h"

#include <godot_cpp/classes/dir_access.hpp>

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
	Ref<DirAccess> data = DirAccess::open(_data_path);
	PackedStringArray files = data->get_files();
	for (auto& file_name : files) {
		String file_path = _data_path.path_join(file_name);
		Frames frames;
		BVHParser parser(file_path);
		if (!parser.parse(frames) && errors) {
			errors->append_array(parser.get_errors());
		}
		_animations.insert(std::make_pair(file_name.ascii().get_data(), frames));
	}
}

void Animations::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("get_data_path"), &Animations::get_data_path);
	godot::ClassDB::bind_method(D_METHOD("set_data_path", "data_path"), &Animations::set_data_path);
	godot::ClassDB::add_property(get_class_static(), PropertyInfo(Variant::STRING, "data_path", godot::PROPERTY_HINT_DIR, ""), "set_data_path", "get_data_path");
}