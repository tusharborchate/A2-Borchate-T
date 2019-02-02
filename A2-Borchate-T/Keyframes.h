#pragma once
class Keyframes
{
	
	
public:
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
	

	Keyframes();
	virtual ~Keyframes();
};

