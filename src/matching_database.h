#pragma once

#include "animations.h"

#include "../kdtree-cpp/kdtree.hpp"

struct Data {
	int animation = -1;
	int frame = -1;
};

class MotionDatabase {
public:
	MotionDatabase();

private:
	std::unique_ptr<Kdtree::KdTree> _kdtree = nullptr;
};

class FeatureVector : public std::vector<float> {

};