#include <iostream>
#include <string>
#include <list>
#include "Keyframes.h"

using namespace std;

#pragma once
class AnimationObject
{
	int objectId;
	string objectName;
	std::list<Keyframes> keyFrames;


public:
	AnimationObject();
	virtual ~AnimationObject();
};

