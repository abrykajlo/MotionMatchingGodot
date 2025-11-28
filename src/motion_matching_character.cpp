#include "motion_matching_character.h"

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>

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
			if (children.size() > 0) {
				_skeleton = Object::cast_to<Skeleton3D>(children[0]);
			}

			if (_animations.is_valid()) {
				_animation_database = _animations->parse();
				if (_animation_database.setup(*_skeleton)) {
					_animation_database.move(*_skeleton, 0, frame);
				}
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

	// get skeleton child node
	TypedArray<Node> children = find_children("*", "Skeleton3D");
	if (children.size() != 1) {
		errors.append("Expected one skeleton");
	}

	return errors;
}

void MotionMatchingCharacter::_process(double delta_time)
{
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}
	frame++;
	_animation_database.move(*_skeleton, 0, frame);
}

void MotionMatchingCharacter::_input(const Ref<InputEvent>& p_event)
{
	Input* input = Input::get_singleton();
	if (input->is_action_pressed("move_forward")) {
		UtilityFunctions::print("input");
	}
}
