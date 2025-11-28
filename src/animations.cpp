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

AnimationDatabase Animations::parse(PackedStringArray* errors)
{
	UtilityFunctions::print("Parsing BVH Files");

	AnimationDatabase anim_db;

	Ref<DirAccess> data = DirAccess::open(_data_path);
	PackedStringArray files = data->get_files();
	for (auto& file_name : files) {
		String file_path = _data_path.path_join(file_name);
		FrameData frame_data;
		BVHParser parser(file_path);
		if (!parser.parse(frame_data) && errors) {
			errors->append_array(parser.get_errors());
		}
		anim_db.add(frame_data);
	}

	return anim_db;
}

void Animations::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("get_data_path"), &Animations::get_data_path);
	godot::ClassDB::bind_method(D_METHOD("set_data_path", "data_path"), &Animations::set_data_path);
	godot::ClassDB::add_property(get_class_static(), PropertyInfo(Variant::STRING, "data_path", godot::PROPERTY_HINT_DIR, ""), "set_data_path", "get_data_path");
}