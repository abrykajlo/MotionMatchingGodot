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
		}
	}
}

PackedStringArray MotionMatching::_get_configuration_warnings() const
{
	PackedStringArray errors;
	_animations->parse(&errors);
	return errors;
}
