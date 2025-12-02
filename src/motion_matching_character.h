#pragma once

#include "animations.h"
#include "matching_database.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/input_event.hpp>

using namespace godot;

class MotionMatchingCharacter : public Node3D {
	GDCLASS(MotionMatchingCharacter, Node3D)

public:
	MotionMatchingCharacter() = default;
	~MotionMatchingCharacter() override = default;

	void set_animations(Ref<Animations> animations);
	Ref<Animations> get_animations();

	virtual PackedStringArray _get_configuration_warnings() const override;
	virtual void _process(double deltaTime) override;

private:
	static void _bind_methods();
	void _notification(int what);
	bool _input_significantly_changed() const;

	Node3D* _player = nullptr;
	Skeleton3D* _skeleton = nullptr;
	Node3D* _camera_control = nullptr;

	Ref<Animations> _animations;

	AnimationDatabase _animation_database;
	MatchingDatabase _matching_database;
	
	float _yaw = 0;
	Vector3 _position;

	Vector2 _last_left_input;
	Vector2 _left_input;

	Frame _source = { 0, 0 };
	Frame _target;
	bool _blending = false;

	// timers
	float _playback_timer = 0;
	float _blend_timer = 0;
	float _search_timer = 0;
};
