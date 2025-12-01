#include "motion_matching_character.h"

#include "constants.h"

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
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	switch (what) {
	case NOTIFICATION_READY:
		{
			_skeleton = get_node<Skeleton3D>("../Player/Skeleton3D");

			_camera_control = get_node<Node3D>("CameraControl");

			if (_animations.is_valid()) {
				_animation_database = _animations->parse();
				_matching_database.build_database(_animation_database);
				if (_animation_database.setup(*_skeleton)) {
					UtilityFunctions::print("Skeleton setup");
					Transform3D transform;
					_animation_database.move(_position, _yaw, *_skeleton, _source, 0, 0);
				}
			}
		}
	}
}

bool MotionMatchingCharacter::_input_significantly_changed() const
{
	return false;
}

PackedStringArray MotionMatchingCharacter::_get_configuration_warnings() const
{
	PackedStringArray errors;

	// parse animations
	if (_animations.is_null()) {
		errors.append("Animations should not be empty");
	}

	// get skeleton child node
	Skeleton3D* skeleton = get_node<Skeleton3D>("../Player/Skeleton3D");
	if (skeleton == nullptr) {
		errors.append("Expected Player sibling node with Skeleton3D child");
	}

	if (get_node<Node3D>("CameraControl") == nullptr) {
		errors.append("Expected Node3D named CameraControl");
	}

	return errors;
}

void MotionMatchingCharacter::_process(double delta_time)
{
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	const Input* input = Input::get_singleton();
	_left_input = input->get_vector("move_left", "move_right", "move_backward", "move_forward");
	_right_input = input->get_axis("camera_left", "camera_right");

	_playback_timer += delta_time;
	
	if (_blending) {
		_blend_timer += delta_time;

		_animation_database.move(
			_position,
			_yaw,
			*_skeleton,
			_source, 
			_target, 
			_playback_timer,
			_blend_timer,
			delta_time
		);
		_position.y = 0;
		set_position(_position);
		
		if (_blend_timer > k_blend_time) {
			_blending = false;
			_source = _target;
			_blend_timer = 0;
			_playback_timer = 0;
		}
	}
	else {
		_animation_database.move(
			_position,
			_yaw,
			*_skeleton,
			_source,
			_playback_timer,
			delta_time
		);
		_position.y = 0;
		set_position(_position);

		_search_timer += delta_time;

		if (_search_timer >= k_search_interval || _input_significantly_changed()) {
			Frame result = _matching_database.search(*_skeleton, _left_input, _right_input);
			if (result != _source) {
				_target = result;

				_blending = true;
				_blend_timer = 0;
			}
			
			_search_timer = 0;
		}
	}
}
