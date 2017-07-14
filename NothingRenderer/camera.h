#pragma once

#include"Vector.h"
#include<math.h>
using namespace VECTOR;
class Camera {
public :


	VECTOR4* position;
	VECTOR4* direction;
	VECTOR4* centerPoint;
	VECTOR4* originalPoint;
	float nearZ, farZ;
	float fov;
	float width, height;

	void GenerateCenterPoint() { 
		VECTOR4 * dotRes = new VECTOR4(); 
		dot(dotRes,direction, nearZ);
		add(centerPoint,position,dotRes );
		delete dotRes; 
	}
	void SetWidthHeight(float wid, float hei ) {
		width = wid;
		height = hei;
	}
	void SetDirection(VECTOR4* dir) {
		direction = normalized(direction,dir);
		GenerateCenterPoint();
	}
	Camera() {
		position = new VECTOR4();
		direction = new VECTOR4();
		originalPoint = new VECTOR4();
		centerPoint = new VECTOR4();

		position->x = 0; position->y = 0; position->z = -5; position->w = 0;
		direction->x = 0; direction->y = 0; direction->z = 1; direction->w = 1;
		originalPoint->x = 0; originalPoint->y = 0; originalPoint->z = 0; 

		fov = 60;
		nearZ = 0.3f;
		farZ = 4000;
	}

	void GetCamCoordinateTransformVert(VERTS* vs) { 
		MATRIX4x4* ma = new MATRIX4x4();
		GenerateTransformMatrix(vs->verts->position, ma);
		for (int i = 0; i < vs->vertCount; i++) {
			matrixdot(vs->verts[i].position, vs->verts[i].position, ma);
			cout << vs->verts[i].position->x << "  " << vs->verts[i].position->y << "  " << vs->verts[i].position->z << endl; 
		} 
	}

	void GetClipSpaceTransfromVert(VERTS* vs) {
		MATRIX4x4 * ma = new MATRIX4x4();
		GenerateClipTransformMatrix(ma); 
		for (int i = 0; i < vs->vertCount; i++) { 
			matrixdot(vs->verts[i].position, vs->verts[i].position, ma);
			vs->verts[i].position->x = vs->verts[i].position->x / vs->verts[i].position->w;
			vs->verts[i].position->y = vs->verts[i].position->y / vs->verts[i].position->w;
			vs->verts[i].position->z = vs->verts[i].position->z / vs->verts[i].position->w;
			vs->verts[i].position->w = 1;
			cout << vs->verts[i].position->x <<"  "<< vs->verts[i].position->y << "  "<<vs->verts[i].position->z << endl;
			cout << vs->verts[i].position->w << endl;
		} 
	}
	MATRIX4x4* GenerateClipTransformMatrix(MATRIX4x4 * res){
	//	res->val[0][0] =
		float ar = width / height;

		float tanRes = 1/  tan( fov* 3.141592653 / 360);
	
		res->val[0][0] = tanRes / ar;
		res->val[1][1] = tanRes;
		res->val[2][2] = (-nearZ - farZ) / (nearZ - farZ);
		res->val[3][2] = 2 * farZ*nearZ / (nearZ - farZ);
		res->val[2][3] = 1;
		return res;
	}
}; 