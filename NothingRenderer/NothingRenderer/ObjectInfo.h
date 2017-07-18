#pragma once
#include"Vector.h"
#include<vector>
using namespace std;
using namespace VECTOR;
using namespace MATRIX;
struct FACEINFO {
	VECTOR4 a;
	VECTOR4 b;
	VECTOR4	c;
};
struct GEOMETRY
{
	string name;
	vector<VECTOR4*> v; 
	vector<FACEINFO*> f;
	int vertCount;
	int faceCount;
	VECTOR4* position;

	GEOMETRY() {
		position = new VECTOR4();
	}
	~GEOMETRY() {
		delete(position);
	}
};
struct  VERT {
	VECTOR4 * position;
	VECTOR4 * normal;
	VECTOR4 * color; 
	VERT::VERT() {
		position = new VECTOR4();
		normal = new VECTOR4();
		color = new VECTOR4();
		color->x = 0; color->y = 0; color->z = 0; color->w = 1;
		position->w = 1;
	}
};
struct VERTS {
	VERT* verts;
	int vertCount;
};

class ObjectHolder {
	list<GEOMETRY*> cubes; 
public:
	void RegistCubeInfo(GEOMETRY* cu) { 
		cubes.push_back(cu);
	}
	list<GEOMETRY*>  GetCubeInfo() {
		return cubes;
	} 
	VERT* GetVertsWorldSpace(VERT* verts, GEOMETRY* obj, MATRIX4x4* rotateMatrix ) {
		MATRIX4x4* ma = new MATRIX4x4();   
		GenerateTransformMatrix(obj->position, ma);   
		
		for (int i = 0; i < obj->vertCount; i++) {
			matrixdot(verts[i].position, obj->v[i],rotateMatrix);
			cout<<"Rot" << verts[i].position->x << " " << verts[i].position->y << " " << verts[i].position->z << endl;
			matrixdot(verts[i].position, verts[i].position, ma);
			cout << verts[i].position->x <<" "<< verts[i].position->y << " " <<verts[i].position->z <<endl;  
			if (i % 3 == 0) {
				verts[i].color->x = 1;
			}
			else if (i % 3 == 1) {
				verts[i].color->y = 1;
			}
			else
				verts[i].color->z = 1;
		} 
		return verts;
	} 

	VERT* GetVertsWorldSpace(VERT* verts, GEOMETRY* obj) {
		MATRIX4x4* ma = new MATRIX4x4();
		GenerateTransformMatrix(obj->position, ma);

		for (int i = 0; i < obj->vertCount; i++) {
			matrixdot(verts[i].position, obj->v[i], ma);
			cout << verts[i].position->x << " " << verts[i].position->y << " " << verts[i].position->z << endl;
			if (i % 3 == 0) {
				verts[i].color->x = 1;
			}
			else if (i % 3 == 1) {
				verts[i].color->y = 1;
			}
			else
				verts[i].color->z = 1;
		}
		return verts;
	}
	~ObjectHolder() {

	} 
	 
};