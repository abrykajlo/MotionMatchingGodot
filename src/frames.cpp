#include "frames.h"

void Frames::set_frame_count(size_t count)
{
	_frame_count = count;
}

void Frames::set_frame_time(float time)
{
	_frame_time = time;
}

void Frames::set_root_position(int frame, const Vector3& position)
{
}

void Frames::set_joint_rotation(int frame, int joint, const Quaternion& rotation)
{
}

void JointFrames::set_rotation(int frame, const Quaternion& rotation)
{
}

void RootFrames::set_position(int frame, const Vector3& position)
{
}
