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
		VECTOR4 black(0, 0, 0, 0), white(1, 1, 1, 0), col;
		bool triggerA = false,triggerB = false;
		width = 200; height = 200;
		BMPColors = new VECTOR4[40000];
		copy(&white, &col);
		for (int i = 0; i < 200; i++) {
			for (int j = 0; j < 200; j++) {
				if ((int)((i + j) * 0.05) % 2 == 0) {
					BMPColors[i* width + j] = white;
				}
				else
					BMPColors[i * width + j] = black; 
			}
		} 

	}
	const VECTOR4* GetBMPColor(float x, float y) {  
		x = (width - 1) * x; y =( height-1)* y ; 
		return &BMPColors[ (int)x + (int)y * width];
	}
	const VECTOR4* GetBMPColor(int x, int y) { 
		return &BMPColors[x-1   + (y-1) *width];
	}


};