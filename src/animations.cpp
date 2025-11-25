#include "animations.h"
#include "bvh_parser.h"

#include <godot_cpp/classes/dir_access.hpp>

#include <cassert>

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
	UtilityFunctions::print("Parsing BVH Files");
	Ref<DirAccess> data = DirAccess::open(_data_path);
	PackedStringArray files = data->get_files();
	for (auto& file_name : files) {
		String file_path = _data_path.path_join(file_name);
		Frames frames;
		BVHParser parser(file_path);
		if (!parser.parse(frames) && errors) {
			errors->append_array(parser.get_errors());
		}
		_animations.push_back(frames);
	}
}

const Frames& Animations::get(int animation) const 
{
	assert(animation < _animations.size());
	return _animations[animation];
}

void Animations::setup_skeleton(Skeleton3D& skeleton) 
{
	assert(_animations.size() > 0);
	_animations[0].setup_skeleton(skeleton);
}

size_t Animations::size()
{
	return _animations.size();
}

void Animations::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("get_data_path"), &Animations::get_data_path);
	godot::ClassDB::bind_method(D_METHOD("set_data_path", "data_path"), &Animations::set_data_path);
	godot::ClassDB::add_property(get_class_static(), PropertyInfo(Variant::STRING, "data_path", godot::PROPERTY_HINT_DIR, ""), "set_data_path", "get_data_path");
}