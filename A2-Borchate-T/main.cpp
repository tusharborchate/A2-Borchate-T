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
#include "AnimationObject.h"

//using namespace
using namespace std;

//global variables
int objectCount = -1;
AnimationObject animationObjectList[200];
string fileName;
int largest = 0;

//prototype
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
void main(int argc, char* argv[])
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
