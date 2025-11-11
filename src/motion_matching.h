#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

class MotionMatching : public Node {
	GDCLASS(MotionMatching, Node)

protected:
	static void _bind_methods();

public:
	MotionMatching() = default;
	~MotionMatching() override = default;
};
