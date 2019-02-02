#pragma once
class Keyframes
{
	int objectId;
	int frame_Number;
	double posX;
	double posY;
	double posZ;
	double rotX;
	double rotY;
	double rotZ;
	double scaleX;
	double scaleY;
	double scaleZ;

	
public:
	Keyframes();
	virtual ~Keyframes();
};

