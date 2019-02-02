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
int frames = 0;

ObjModel spiky;
ObjModel firebucket;
DisplayList bucketlist;

//prototype
void init();
void initDisplay();
void keyboard(unsigned char key, int x, int y);
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
void LinearInterpolation();
double LinearInterpolationProcess(int keyframestart, int keyframelast, double a1, double a2, double a3);

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
				ProcessFile(line);
			}
			catch (const std::exception&)
			{
				cout << "Error while processing file";
			}
		}
		myfile.close();

		try
		{
			LinearInterpolation();
		}
		catch (const std::exception&)
		{
			cout << "Error while linear interpolation";
		}
	}
	else cout << "Unable to open file";
	DisplayValueEachObject();

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

	size_t found = line.find("object");
	if (found != string::npos)
	{
		vector<string> sep = split(line, ' ');
		if (objectCount < stoi(sep[1]))
		{
			objectCount = objectCount + 1;
			animationObjectList[objectCount].objectId = stoi(sep[1]);
			animationObjectList[objectCount].objectName = sep[2];
		}
	}
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
		count = count + 1;
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
		animationObjectList[k.objectId].keyframeCount = animationObjectList[k.objectId].keyframeCount + 1;
		animationObjectList[k.objectId].keyFrames.push_back(k);
	}
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
	for (size_t i = 0; i < objectCount; i++)
	{
		animationObjectList[objectCount].keyFrames.sort(keyframe_compare);
	}
}

//Linear interpolation
void LinearInterpolation()
{
	std::list<Keyframes>::iterator it;

	for (size_t i = 0; i < objectCount; i++)
	{
		int count = 0;
		int framenumber = 0;
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

double LinearInterpolationProcess(int keyframestart, int keyframelast, double a1, double a2, double a3)
{
	try {
		double result = keyframestart + (((keyframelast - keyframestart) / (a2 - a1))*(a3 - a1));
		if (!std::isfinite(result)) {
			return 0;
		}
		return result;
	}
	catch (exception) {
		return 0;
	}

}

//display values as per object
void DisplayValueEachObject()
{
	std::list<Keyframes>::iterator it;

	for (size_t i = 0; i < objectCount; i++)
	{
		cout << "Object ID" << animationObjectList[i].objectId;
		cout << "Object Name" << animationObjectList[i].objectName;
		(animationObjectList[i].keyFrames).sort(keyframe_compare);
		for (it = animationObjectList[i].keyFrames.begin(); it != animationObjectList[i].keyFrames.end(); ++it)
		{
			cout << '\n';
			cout << "keyframe number" << it->frame_Number;
			cout << '\n';
			cout << "pos X" << it->posX;

		}
	}

	cout << largest;
}



int main(int argc, char* argv[])
{
	Process();
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Loading OBJ Models");
	glutKeyboardFunc(keyboard);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	init();

	glutMainLoop();

	return 1;
}

void init()
{
	spiky.load(animationObjectList[0].objectName);
	firebucket.load("firebucket.obj");
	bucketlist = firebucket.getDisplayList();
	initDisplay();
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
	switch (key)
	{
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	}
}

void update()
{
	// update your variables here
	frames = frames + 1;
	sleep(1.0 / 60.0);
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glLoadIdentity();

	// display positive X, Y, and Z axes near origin
	glBegin(GL_LINES);
	glColor3d(1.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(2.0, 0.0, 0.0);
	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 2.0, 0.0);
	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 2.0);
	glEnd();

	// draw a purple wireframe cube
	glColor3d(1.0, 0.0, 1.0);

	glPushMatrix();
	glTranslated(0.0, 0.0, 0.0);
	glRotated(45, 0.0, 1.0, 0.0);
	glScaled(1.0, 1.0, 1.0);
	glutWireCube(1.0);
	glPopMatrix();

	if (frames < animationObjectList[0].keyFrames.size())
	{
		// Create iterator pointing to first element
		std::list<Keyframes>::iterator it;
		it = animationObjectList[0].keyFrames.begin();
		// Advance the iterator by 2 positions,
		std::advance(it, frames);

		for (size_t i = 0; i < 1; i++)
		{
			glPushMatrix();
			gluLookAt(7,50,0,
				(it->posX) / 2, (it->posY) / 2, (it->posZ) / 2,
				0.0, 1.0, 0.0);

					//	glScaled(it->scaleX, it->scaleY, it->scaleZ);

			/*glRotated(it->posZ, 0, 0, 1);

			glRotated(it->posY, 0, 1, 0);

			glRotated(it->posX, 1, 0, 0);  */         
			glTranslated(it->posX, it->posY, it->posZ);

			spiky.draw();
			glPopMatrix();
		}
	}
	for (size_t i = 0; i < 50; i++)
	{
		glPushMatrix();
		glTranslated(-1.0 - i, 0.0, 0.0);
		glScaled(0.005, 0.005, 0.005);

		bucketlist.draw();
		glPopMatrix();
	}


	glPopMatrix();
	glutSwapBuffers();
}
