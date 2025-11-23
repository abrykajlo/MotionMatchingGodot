#include "motion_matching_character.h"

#include <godot_cpp/classes/mesh_instance3d.hpp>

#include <cassert>

void MotionMatchingCharacter::set_animations(Ref<Animations> animations) {
	_animations = animations;
}

Ref<Animations> MotionMatchingCharacter::get_animations() {
	return _animations;
}

void MotionMatchingCharacter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_animations"), &MotionMatchingCharacter::get_animations);
	ClassDB::bind_method(D_METHOD("set_animations", "animations"), &MotionMatchingCharacter::set_animations);
	ClassDB::add_property(get_class_static(), PropertyInfo(Variant::OBJECT, "animations", PROPERTY_HINT_RESOURCE_TYPE, "Animations"), "set_animations", "get_animations");
}

void MotionMatchingCharacter::_notification(int what) {
	switch (what) {
	case NOTIFICATION_READY:
		{
			TypedArray<Node> children = find_children("*", "Skeleton3D");
			_skeleton = Object::cast_to<Skeleton3D>(children[0]);
			assert(_skeleton != nullptr);

			if (_animations.is_valid()) {
				_animations->parse();
				const Frames& frames = _animations->get(0);
				_animations->setup_skeleton(*_skeleton);
				frames.move_skeleton(*_skeleton, _curr_time);
			}
		}
	}
}

PackedStringArray MotionMatchingCharacter::_get_configuration_warnings() const
{
	PackedStringArray errors;

	// parse animations
	if (_animations.is_null()) {
		errors.append("Animations should not be empty");
	}
	else {
		_animations->parse(&errors);
	}

	// get skeleton child node
	TypedArray<Node> children = find_children("*", "MeshInstance3D");
	if (children.size() != 1) {
		errors.append("Expected one skeleton");
	}

	return errors;
}

void MotionMatchingCharacter::_process(double delta_time)
{
	_curr_time += delta_time;
	_animations->get(0).move_skeleton(*_skeleton, _curr_time);
}
