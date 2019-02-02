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

using namespace std;
int objectCount = -1;
AnimationObject animationObjectList[200];
string fileName;

void GetInputFile();
void to_lower(string &s);
void ProcessFile(string line);
void to_upper(string &s);
vector<string>split(string str, char delimiter);


void main(int argc, char* argv[])
{
	string line;
	GetInputFile();


	ifstream myfile(fileName);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			ProcessFile(line);
		}
		myfile.close();

		for (size_t i = 0; i < objectCount; i++)
		{
			cout << "Object ID" << animationObjectList[i].objectId;
			cout << "Object Name" << animationObjectList[i].objectName;

		}
	}
	else cout << "Unable to open file";

	
}

void GetInputFile()
{
	cout << "Enter animation file name with path:";
	cin >> fileName;
}

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