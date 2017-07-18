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

	GEOMETRY() { 
	}
	~GEOMETRY() { 
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
struct Object {
	GEOMETRY* prefab;
	VERT* verts;
	int vertCount;
	string objectName;
	VECTOR4 * position;
	Object::Object() {
		position = new VECTOR4();
	}
	Object::~Object() {
		delete position;
	}
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
	void GetVertsWorldSpace(Object* obj, MATRIX4x4* rotateMatrix ) {
		MATRIX4x4* ma = new MATRIX4x4();   
		GenerateTransformMatrix(obj->position, ma);
		
		for (int i = 0; i < obj->prefab->vertCount; i++) {
			matrixdot(obj->verts[i].position, obj->prefab->v[i],rotateMatrix);
			cout<<"Rot" << obj->verts[i].position->x << " " << obj->verts[i].position->y << " " << obj->verts[i].position->z << endl;
			matrixdot(obj->verts[i].position, obj->verts[i].position, ma);
			cout << obj->verts[i].position->x <<" "<< obj->verts[i].position->y << " " << obj->verts[i].position->z <<endl;
			if (i % 3 == 0) {
				obj->verts[i].color->x = 1;
			}
			else if (i % 3 == 1) {
				obj->verts[i].color->y = 1;
			}
			else
				obj->verts[i].color->z = 1;
		}  
	} 

	void GetVertsWorldSpace(Object* obj, GEOMETRY* prefab ) {
		MATRIX4x4* ma = new MATRIX4x4();
		GenerateTransformMatrix(obj->position, ma);

		for (int i = 0; i < prefab->vertCount; i++) {
			matrixdot(obj->verts[i].position, prefab->v[i], ma);
			cout << obj->verts[i].position->x << " " << obj->verts[i].position->y << " " << obj->verts[i].position->z << endl;
			if (i % 3 == 0) {
				obj->verts[i].color->x = 1;
			}
			else if (i % 3 == 1) {
				obj->verts[i].color->y = 1;
			}
			else
				obj->verts[i].color->z = 1;
		} 
	}
	~ObjectHolder() {

	} 
	 
};