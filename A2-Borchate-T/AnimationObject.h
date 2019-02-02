#include <iostream>
#include <string>
#include <list>
#include "Keyframes.h"

using namespace std;

#pragma once
class AnimationObject
{
	

public:
	AnimationObject();
	int objectId;
	string objectName;
	int keyframeCount;
	std::list<Keyframes> keyFrames;

	virtual ~AnimationObject();
};

