#pragma once
#include"Vector.h"
#include<list>

using namespace VECTOR;
using namespace MATRIX;
struct GEOMETRY
{
	VECTOR4* v; 
	int vertCount;
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
	VERT* GetVertsWorldCoor(VERT* verts, GEOMETRY* obj) {
		MATRIX4x4* ma = new MATRIX4x4();
		GenerateTransformMatrix(obj->position, ma);   
		for (int i = 0; i < obj->vertCount; i++) {  
			matrixdot(verts[i].position,&obj->v[i], ma);
			cout << verts[i].position->x << verts[i].position->y << verts[i].position->z	;
		}
		return verts;
	} 
	
	~ObjectHolder() {

	} 
	 
};