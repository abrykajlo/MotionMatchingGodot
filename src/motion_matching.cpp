#include "motion_matching.h"

void MotionMatching::set_animations(Ref<Animations> animations) {
	_animations = animations;
}

Ref<Animations> MotionMatching::get_animations() {
	return _animations;
}

void MotionMatching::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_animations"), &MotionMatching::get_animations);
	ClassDB::bind_method(D_METHOD("set_animations", "animations"), &MotionMatching::set_animations);
	ClassDB::add_property(get_class_static(), PropertyInfo(Variant::OBJECT, "animations", PROPERTY_HINT_RESOURCE_TYPE, "Animations"), "set_animations", "get_animations");
}

void MotionMatching::_notification(int what) {
	switch (what) {
	case NOTIFICATION_READY:
		{
			TypedArray<Node> children = find_children("*", "Skeleton3D");
			_skeleton = Object::cast_to<Skeleton3D>(children[0]);

			_animations->parse();
		}
	}
}

PackedStringArray MotionMatching::_get_configuration_warnings() const
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
	TypedArray<Node> children = find_children("*", "Skeleton3D");
	if (children.size() != 1) {
		errors.append("Expected one skeleton");
	}

	return errors;
}
