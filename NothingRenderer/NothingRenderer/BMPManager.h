#pragma once
#include<string>
#include<iostream>
#include<fstream>
#include"Vector.h"
using namespace VECTOR;
using namespace std;
class BMPManager {
public :
	string BMPPath;
	VECTOR4* BMPColors;
	int width, height;
	
	void LoadBMPPicture() {
		ifstream ifile(BMPPath, ios_base::binary);
		if (!ifile)
			cout << "BMP file load failed";
		unsigned char buf[54];

		ifile.read((char*)buf, sizeof(buf));
		width = *(int *)&buf[18];
		height = abs(*(int *)& buf[22]);
		int   count = width * height  * 3;
		BMPColors = new VECTOR4[count];
		unsigned char * temp = new unsigned char[count];
		ifile.read((char*)temp, count);
		
		for (int i = 0; i < count; i += 3) {
			BMPColors[i].x = temp[i + 2] / 255.0f;
			BMPColors[i].y = temp[i + 1] / 255.0f;
			BMPColors[i].z = temp[i + 0] / 255.0f;
			BMPColors[i].w = 0;
		}
		delete temp;
	}
	const VECTOR4* GetBMPColor(float x, float y) {
		if (x < 0 || x > 1 || y < 0 || y > 1) { 
			cout << "error in get color" <<x<<" " << y << endl;
		}
		return &BMPColors[(int)(x*width -1  + y*height * width)];
	}


};