//header files
#include "main.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <clocale>
#include <sstream>  
#include <vector>
#include <string>
#include <cmath>

#include <cassert>
#include "glut.h"
#include "Sleep.h"
#include  "ObjLibrary\ObjModel.h"
#include "ObjLibrary/DisplayList.h"
#include "AnimationObject.h"

//using namespace
using namespace std;
using namespace ObjLibrary;

//global variables
int objectCount = -1;
AnimationObject animationObjectList[200];
string fileName;
int largest = 0;
double translateX = 0;
double translateY = 0;
double translateZ = 0;
bool restartf1 = false;
bool restartf2 = false;

int frames = 0;

ObjModel ladybug;
ObjModel spider;
DisplayList ladybuglist;
DisplayList spiderlist;

//prototype
void init();
void initDisplay();
void keyboard(unsigned char key, int x, int y);
void special(int special_key, int x, int y);
void update();
void reshape(int w, int h);
void display();

void GetInputFile();
void to_lower(string &s);
void ProcessFile(string line);
void to_upper(string &s);
vector<string>split(string str, char delimiter);
bool keyframe_compare(Keyframes a, Keyframes b);
void SortKeyFrames();
void DisplayValueEachObject();
void DisplayValueEachFrame();

Keyframes GetValue(int frameNumber, int objectid);
int  GetPrevKeyNumber(int frameNumber, int object);
int  GetNextKeyNumber(int frameNumber, int object);
void GetLargestKeyframe();
bool updateListElement(int f, double time, int objectid);
bool KeyframeExist(int f, int objectid);
Keyframes GetKeyframe(int f, int objectid);

Keyframes LinearInterpolation(int framenumber, int objectid);
double LinearInterpolationProcess(int keyframestart, int keyframelast, double a1, double a2, int framenumber);
int Check_ObjectExist(int objectid);
void LinearInterpolationGetAllFrames();

//main
void Process()
{
	string line;
	GetInputFile();


	ifstream myfile(fileName);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			try
			{
				//step 1 read file and store data in list
				ProcessFile(line);
			}
			catch (const std::exception&)
			{
				cout << '\n';
				cout << "Error while processing file" << line;
			}
		}
		myfile.close();

		try
		{
			LinearInterpolationGetAllFrames();
			DisplayValueEachObject();
		}
		catch (const std::exception&)
		{
			cout << "Error while linear interpolation";
		}
	}
	else cout << "Unable to open file";


}

//get file path as input
void GetInputFile()
{
	cout << "Enter animation file name with path:";
	cin >> fileName;
}

// process all lines
void ProcessFile(string line)
{
	std::locale loc;
	to_lower(line);

	//if line contains object then store in object
	size_t found = line.find("object");
	if (found != string::npos)
	{
		vector<string> sep = split(line, ' ');

		objectCount = objectCount + 1;
		int count = 1;
		while (sep[count] == "")
		{
			count = count + 1;
		}
		animationObjectList[objectCount].objectId = stoi(sep[count]);
		count = count + 1;
		while (sep[count] == "")
		{
			count = count + 1;
		}
		animationObjectList[objectCount].objectName = sep[count];

	}

	//if line contains keyframe then store in keyframe
	found = line.find("keyframe");
	if (found != string::npos)
	{

		vector<string> sep = split(line, ' ');
		Keyframes k;
		int count = 1;
		while (sep[count] == "")
		{
			count = count + 1;
		}
		k.objectId = stoi(sep[count]);
		int isExist = Check_ObjectExist(k.objectId);
		count = count + 1;
		if (isExist >= 0)
		{
			while (sep[count] == "")
			{
				count = count + 1;
			}
			k.frame_Number = stoi(sep[count]);
			if (k.frame_Number > largest)
			{
				largest = k.frame_Number;
			}
			count = count + 1;

			while (sep[count] == "")
			{
				count = count + 1;
			}
			k.posX = stod(sep[count]);
			count = count + 1;
			while (sep[count] == "")
			{
				count = count + 1;
			}
			k.posY = stod(sep[count]);
			count = count + 1;
			while (sep[count] == "")
			{
				count = count + 1;
			}
			k.posZ = stod(sep[count]);
			count = count + 1;
			while (sep[count] == "")
			{
				count = count + 1;
			}
			k.rotX = stod(sep[count]);
			count = count + 1;
			while (sep[count] == "")
			{
				count = count + 1;
			}
			k.rotY = stod(sep[count]);
			count = count + 1;
			while (sep[count] == "")
			{
				count = count + 1;
			}
			k.rotZ = stod(sep[count]);
			count = count + 1;
			while (sep[count] == "")
			{
				count = count + 1;
			}
			k.scaleX = stod(sep[count]);
			count = count + 1;
			while (sep[count] == "")
			{
				count = count + 1;
			}
			k.scaleY = stod(sep[count]);
			count = count + 1;
			while (sep[count] == "")
			{
				count = count + 1;
			}
			k.scaleZ = stod(sep[count]);
			animationObjectList[isExist].keyframeCount = animationObjectList[k.objectId].keyframeCount + 1;
			animationObjectList[isExist].keyFrames.push_back(k);
		}
		else {
			cout << "test keyframes for non existent object";
			animationObjectList[objectCount].notExist = true;
		}
	}
}

int Check_ObjectExist(int objectid)
{
	for (size_t i = 0; i <= objectCount; i++)
	{
		if (animationObjectList[i].objectId == objectid)
		{
			return i;
		}

	}
	return -1;
}

void to_lower(string &s) {
	for (int i = 0; i < s.length(); i++)
		if (s[i] >= 'A' and s[i] <= 'Z')
			s[i] = s[i] | (1 << 5);
}

void to_upper(string &s) {
	for (int i = 0; i < s.length(); i++)
		if (s[i] >= 'a' and s[i] <= 'z')
			s[i] = s[i] & (~(1 << 5));
}

vector<string> split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}

bool keyframe_compare(Keyframes a, Keyframes b)
{
	if (a.frame_Number < b.frame_Number)
	{
		return true;
	}
	else {
		return false;
	}
}

void SortKeyFrames()
{
	for (size_t i = 0; i <= objectCount; i++)
	{
		if (animationObjectList[i].notExist == false)
		{
			animationObjectList[i].keyFrames.sort(keyframe_compare);
		}
	}
}

void LinearInterpolationGetAllFrames()
{
	std::list<Keyframes>::iterator it;

	for (size_t i = 0; i <= objectCount; i++)
	{
		int count = 0;
		int framenumber = 0;
		if (animationObjectList[i].keyFrames.size() == 0)
		{
			animationObjectList[i].notExist = true;
			continue;
		}
		//sorting the keyframes of all objects
		(animationObjectList[i].keyFrames).sort(keyframe_compare);
		for (it = animationObjectList[i].keyFrames.begin(); it != animationObjectList[i].keyFrames.end(); ++it)
		{
			if (count == 0)
			{

				std::list<Keyframes>::iterator iter = animationObjectList[i].keyFrames.end();
				std::advance(iter, -1);
				framenumber = iter->frame_Number;
			}

			count = count + 1;
			int currentFrameNumber = it->frame_Number;

			if (currentFrameNumber != framenumber)
			{

				auto next = std::next(it, 1);
				int nextFrameNumber = next->frame_Number;

				if (nextFrameNumber != (currentFrameNumber + 1))
				{
					
					int framecount = currentFrameNumber;
					while (framecount != ((nextFrameNumber)-1))
					{
						
				
						double posx = it->posX;
						double nextposx = next->posX;

						Keyframes k;
						k.frame_Number = framecount + 1;
						k.objectId = animationObjectList[i].objectId;
						k.posX = LinearInterpolationProcess(currentFrameNumber, nextFrameNumber, posx, nextposx, framecount + 1);
						k.posY = LinearInterpolationProcess(currentFrameNumber, nextFrameNumber, it->posY, next->posY, framecount + 1);
						k.posZ = LinearInterpolationProcess(currentFrameNumber, nextFrameNumber, it->posZ, next->posZ, framecount + 1);
						k.scaleX = LinearInterpolationProcess(currentFrameNumber, nextFrameNumber, it->scaleX, next->scaleX, framecount + 1);
						k.scaleY = LinearInterpolationProcess(currentFrameNumber, nextFrameNumber, it->scaleY, next->scaleY, framecount + 1);
						k.scaleZ = LinearInterpolationProcess(currentFrameNumber, nextFrameNumber, it->scaleZ, next->scaleZ, framecount + 1);
						k.rotX = LinearInterpolationProcess(currentFrameNumber, nextFrameNumber, it->rotX, next->rotX, framecount + 1);
						k.rotY = LinearInterpolationProcess(currentFrameNumber, nextFrameNumber, it->rotY, next->rotY, framecount + 1);
						k.rotZ = LinearInterpolationProcess(currentFrameNumber, nextFrameNumber, it->rotZ, next->rotZ, framecount + 1);
						animationObjectList[i].keyFrames.push_back(k);
						framecount = framecount + 1;
						if (framecount == 195)
						{
							cout << framecount;
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}

}



//Linear interpolation
Keyframes LinearInterpolation(int framenumber, int objectid)
{

	//get previous key frame number
	int getprevkey = GetPrevKeyNumber(framenumber, objectid);
	//get next key frame number
	int getnextkey = GetNextKeyNumber(framenumber, objectid);


	//get previous frame keyframe and attributes
	Keyframes k = GetValue(getprevkey, objectid);

	//get next frame keyframe and attributes
	Keyframes key1 = GetValue(getnextkey, objectid);

	//get keyframe for new framenumber by linear interpolation process
	Keyframes knew;
	knew.posX = LinearInterpolationProcess(getprevkey, getnextkey, k.posX, key1.posX, framenumber);
	knew.posY = LinearInterpolationProcess(getprevkey, getnextkey, k.posY, key1.posY, framenumber);
	knew.posZ = LinearInterpolationProcess(getprevkey, getnextkey, k.posZ, key1.posZ, framenumber);
	knew.scaleX = LinearInterpolationProcess(getprevkey, getnextkey, k.scaleX, key1.scaleX, framenumber);
	knew.scaleY = LinearInterpolationProcess(getprevkey, getnextkey, k.scaleY, key1.scaleY, framenumber);
	knew.scaleZ = LinearInterpolationProcess(getprevkey, getnextkey, k.scaleZ, key1.scaleZ, framenumber);
	knew.rotX = LinearInterpolationProcess(getprevkey, getnextkey, k.rotX, key1.rotX, framenumber);
	knew.rotY = LinearInterpolationProcess(getprevkey, getnextkey, k.rotY, key1.rotY, framenumber);
	knew.rotZ = LinearInterpolationProcess(getprevkey, getnextkey, k.rotZ, key1.rotZ, framenumber);

	knew.objectId = objectid;
	knew.frame_Number = framenumber;
	animationObjectList[objectid].keyFrames.push_back(knew);
	return knew;
}

double LinearInterpolationProcess(int keyframestart, int keyframelast, double a1, double a2, int framenumber)
{
	try {
		if (a2 - a1 == 0)
		{
			int diff = framenumber - keyframestart;
			return  a1 + (diff / 1000) + 0.01;
		}
		double result = a1 + (((a2 - a1) / (keyframelast - keyframestart))*(framenumber - keyframestart));
		if (!std::isfinite(result)) {
			return 0;
		}
		return result;
	}
	catch (exception) {
		return 0;
	}

}

Keyframes GetValue(int frameNumber, int objectid)
{
	std::list<Keyframes>::iterator it;
	int foundprevframe = 0;
	for (it = animationObjectList[objectid].keyFrames.begin(); it != animationObjectList[objectid].keyFrames.end(); ++it)
	{
		if (frameNumber == it->frame_Number)
		{
			return  (*it);
		}
	}
}
//display values as per object
void DisplayValueEachObject()
{
	std::list<Keyframes>::iterator it;

	for (size_t i = 0; i <= objectCount; i++)
	{
		if (animationObjectList[i].notExist == false)
		{
			cout << "\n";
			cout << "---------------------------------------------";

			cout << "Object ID" << animationObjectList[i].objectId;
			cout << "\n";

			cout << "Object Name" << animationObjectList[i].objectName;
			cout << "\n";

			(animationObjectList[i].keyFrames).sort(keyframe_compare);
			int count = 0;
			for (it = animationObjectList[i].keyFrames.begin(); it != animationObjectList[i].keyFrames.end(); ++it)
			{
				if (count == 0)
				{

				}
				cout << "\n";
				cout << "---------------------------------------------";
				cout << "\n";
				cout << "keyframe number" << it->frame_Number;
				cout << "\n";
				cout << "---------------------------------------------";
				cout << "\n";
				cout << "translation(" << it->posX << "," << it->posY << "," << it->posZ << ")";
				cout << "\n";
				cout << "---------------------------------------------";
				cout << "\n";
				cout << "rotation(" << it->rotX << "," << it->rotY << "," << it->rotZ << ")";
				cout << "\n";
				cout << "---------------------------------------------";
				cout << "\n";
				cout << "scaling(" << it->scaleX << "," << it->scaleY << "," << it->scaleZ << ")";
				cout << "\n";

			}
		}
	}

}

void DisplayValueEachFrame()
{
	std::list<Keyframes>::iterator it;

	for (size_t i = 0; i < largest; i++)
	{
		cout << "frame : " << i;
		if (animationObjectList[i].notExist == false)
		{

			cout << "Object ID" << animationObjectList[i].objectId;
			cout << "Object Name" << animationObjectList[i].objectName;
			(animationObjectList[i].keyFrames).sort(keyframe_compare);
			int count = 0;
			for (it = animationObjectList[i].keyFrames.begin(); it != animationObjectList[i].keyFrames.end(); ++it)
			{
				if (count == 0)
				{

				}
				cout << '\n';
				cout << "keyframe number" << it->frame_Number;
				cout << '\n';
				cout << "translation(" << it->posX << "," << it->posY << "," << it->posZ << ")";
				cout << '\n';
				cout << "rotation(" << it->rotX << "," << it->rotY << "," << it->rotZ << ")";
				cout << '\n';
				cout << "scaling(" << it->scaleX << "," << it->scaleY << "," << it->scaleZ << ")";
				cout << '\n';
				cout << "time : " << it->time;
			}
		}
	}


}


int  GetPrevKeyNumber(int frameNumber, int object)
{
	std::list<Keyframes>::iterator it;
	int foundprevframe = 0;
	for (it = animationObjectList[object].keyFrames.begin(); it != animationObjectList[object].keyFrames.end(); ++it)
	{
		if (frameNumber < it->frame_Number)
		{
			return foundprevframe;
		}
		foundprevframe = it->frame_Number;
	}
}

int  GetNextKeyNumber(int frameNumber, int object)
{
	std::list<Keyframes>::iterator it;
	int foundnextframe = 0;
	for (it = animationObjectList[object].keyFrames.begin(); it != animationObjectList[object].keyFrames.end(); ++it)
	{
		if (frameNumber < it->frame_Number)
		{
			foundnextframe = it->frame_Number;
			return foundnextframe;

		}
	}
	return foundnextframe;

}

void GetLargestKeyframe()
{


	for (size_t i = 0; i <= objectCount; i++)
	{
		if (animationObjectList[i].notExist == false)
		{
			std::list<Keyframes>::iterator iter = animationObjectList[i].keyFrames.end();
			std::advance(iter, -1);
			animationObjectList[i].highest = iter->frame_Number;
			iter = animationObjectList[i].keyFrames.begin();
			std::advance(iter, 0);
			animationObjectList[i].lowest = iter->frame_Number;
		}
	}
}

bool updateListElement(int f, double time, int objectid)
{

	double prevtime = 0;

	std::list<Keyframes>::iterator it;
	for (it = animationObjectList[objectid].keyFrames.begin(); it != animationObjectList[objectid].keyFrames.end(); ++it)
	{

		Keyframes temp = *it;
		if (temp.frame_Number == f - 1)
		{
			prevtime = it->time;
		}
		if (temp.frame_Number == f)
		{
			temp.time = prevtime + time;
			it->time = prevtime + time;
			return true;
		}
	}
	return false;
}

Keyframes GetKeyframe(int f, int objectid)
{

	Keyframes k;
	std::list<Keyframes>::iterator it;
	for (it = animationObjectList[objectid].keyFrames.begin(); it != animationObjectList[objectid].keyFrames.end(); ++it)
	{

		k = *it;
		if (it->frame_Number == f)
		{
			return k;
		}
	}
}


bool KeyframeExist(int f, int objectid)
{


	std::list<Keyframes>::iterator it;
	for (it = animationObjectList[objectid].keyFrames.begin(); it != animationObjectList[objectid].keyFrames.end(); ++it)
	{


		if (it->frame_Number == f)
		{
			return true;
		}
	}
	return false;
}

int main(int argc, char* argv[])
{
	Process();
	//step 2 sort all keyframes by ascending order
	SortKeyFrames();
	//get largest and smallest keyframe for each object
	GetLargestKeyframe();
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Loading OBJ Models");
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	init();

	glutMainLoop();

	return 1;
}

void init()
{
	//initialize before
	initDisplay();

	ladybug.load("ladybug.obj");

	spider.load("spider.obj");
	spiderlist = spider.getDisplayList();
	ladybuglist = ladybug.getDisplayList();

}

void initDisplay()
{
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{

}
void special(int special_key, int x, int y)
{
	switch (special_key)
	{
	case GLUT_KEY_F1:
		frames = 0;
		restartf1 = true;
		objectCount = 0;
		break;

	case GLUT_KEY_F2:

		restartf1 = true;
		objectCount = 1;
		break;
	}
}

void update()
{
	// update your variables here
	sleep(1.5 / 60.0);
	frames = frames + 1;

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (frames < largest + 5)
	{
		//for all frames
		for (size_t f = frames; f <= frames; f++)
		{
			cout << "\n";
			cout << "---------------------------------------------";
			cout << "\n";
			cout << "Keyframe - " << f;
			//for all objects
			Keyframes k;
			glLoadIdentity();

			gluLookAt(7.0, 100.0, 7.0,
				0.0, 0.0, 0.0,
				0.0, 1.0, 0.0);

			for (size_t o = 0; o <= objectCount; o++)
			{
				cout << "\n";
				cout << "---------------------------------------------";
				cout << "\n";
				cout << "object id" << animationObjectList[o].objectId;
				int size = animationObjectList[o].keyFrames.size();
				if (size == 0)
				{
					cout << "\n";
					cout << "---------------------------------------------";

					cout << "Tests out-of-order and missing object IDs(test1)";
					cout << "\n";
				}
				else
				{

					//get time for each object for each frame
					double prevtime = glutGet(GLUT_ELAPSED_TIME);


					//if object largest frame is smaller than current frame
					if (size <= frames )
					{
						cout << "\n";
						cout << "---------------------------------------------";
						cout << "\n";
						cout << "object does not exist";
					}
					else {
						//if (!KeyframeExist(f, o))
						//{
						//	//if current frame not exist then linear interpolation
						////	k = LinearInterpolation(f, animationObjectList[o].objectId);
						//}
						//else {
						//	
						//}

						k = GetKeyframe(f, o);
						//draw object
						glPushMatrix();
						cout << "\n";
						cout << "---------------------------------------------";
						cout << "\n";
						cout << "translation(" << k.posX << "," << k.posY << "," << k.posZ << ")";
						cout << "\n";
						cout << "---------------------------------------------";
						cout << "\n";
						cout << "rotation(" << k.rotX << "," << k.rotY << "," << k.rotZ << ")";
						cout << "\n";
						cout << "---------------------------------------------";
						cout << "\n";
						cout << "scaling(" << k.scaleX << "," << k.scaleY << "," << k.scaleZ << ")";
						cout << "\n";

						glTranslated(k.posX, k.posY, k.posZ);
						glRotated(k.rotX, 1, 0, 0);
						glRotated(k.rotY, 0, 1, 0);


						glRotated(k.rotZ, 0, 0, 1);


						glScaled(k.scaleX, k.scaleY, k.scaleZ);

						if (animationObjectList[o].objectName == "ladybug.obj")
						{
							ladybuglist.draw();
						}
						else if (animationObjectList[o].objectName == "spider.obj") {
							spiderlist.draw();
						}

						glPopMatrix();


						//object draw complete

						//double time = glutGet(GLUT_ELAPSED_TIME);
						//double timediff = time - prevtime;
						//k.time = timediff;
						//k.frame_Number = f;

						////get previous keyframe time and current keyframe time and update time
						//updateListElement(f, timediff, o);
					}

				}


			}
			glutSwapBuffers();

		}



	}


}
