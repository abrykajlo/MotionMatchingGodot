#pragma once

#include "animations.h"

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
	virtual void _input(const Ref<InputEvent>& p_event) override;

protected:
	static void _bind_methods();
	void _notification(int what);

	Skeleton3D* _skeleton = nullptr;
	Ref<Animations> _animations;
	double _curr_time = 0;
};
