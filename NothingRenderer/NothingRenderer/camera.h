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
		direction = normalizedVector3(direction,dir);
		GenerateCenterPoint();
	}
	void GetClipSpaceWidthHeight(VECTOR2 *res ) {
		res->y = tan(fov* 3.141592653 / 360) *nearZ;
		res->x = (width / height)* res->y; 
	}
	Camera() {
		position = new VECTOR4();
		direction = new VECTOR4(); 
		centerPoint = new VECTOR4();

		position->x = 0; position->y =1 ; position->z = -3; position->w = 1;
		direction->x = 0; direction->y = 0; direction->z = 1; direction->w = 0; 

		fov = 60;
		nearZ = 0.3f;
		farZ = 4000;
	}

	void GetCamCoordinateTransformVert(Object* vs,bool normalConvert = false) { 
		MATRIX4x4* ma = new MATRIX4x4();
		GenerateCamMatrix(position, ma);
		for (int i = 0; i < vs->prefab->normalCount; i++) { 
			matrixdot(vs->verts[i].normal, vs->verts[i].normal, ma);
		}
		for (int i = 0; i < vs->vertCount; i++) {
			matrixdot(vs->verts[i].position, vs->verts[i].position, ma);
	//		cout << vs->verts[i].position->x << "  " << vs->verts[i].position->y << "  " << vs->verts[i].position->z << endl; 
		} 
	}

	void GetClipSpaceTransfromVert(Object* vs) {
		MATRIX4x4 * ma = new MATRIX4x4();
		GenerateClipTransformMatrix(ma);
		float halfw = width / 2, halfh = height / 2;
		for (int i = 0; i < vs->vertCount; i++) { 
			matrixdot(vs->verts[i].position, vs->verts[i].position, ma);
	//		cout << vs->verts[i].position->x << "  " << vs->verts[i].position->y << "  " << vs->verts[i].position->z << endl;
	//		cout << vs->verts[i].position->w << endl;
			vs->verts[i].position->x =  (int)halfw* (1+ vs->verts[i].position->x / vs->verts[i].position->w );
			vs->verts[i].position->y =  (int)halfh* (1+  vs->verts[i].position->y / vs->verts[i].position->w );
			vs->verts[i].position->z = vs->verts[i].position->z / vs->verts[i].position->w;
			vs->verts[i].position->w = 1;
	//		cout << vs->verts[i].position->x <<"  "<< vs->verts[i].position->y << "  "<<vs->verts[i].position->z << endl;
		} 
	}
	void GetUV(Object * vs) {
		VECTOR2 nearWH ;
		GetClipSpaceWidthHeight(&nearWH);
	//	cout << "near plane  " << nearWH.x << " " << nearWH.y<<endl;
		for (int i = 0; i < vs->vertCount; i++) { 
	//		cout << vs->verts[i].position->x << " " << vs->verts[i].position->y << endl;
		}


	}
	void GenerateCamMatrix(VECTOR4* pos, MATRIX4x4* ma) {
		ma->val[3][0] = -pos->x;
		ma->val[3][1] = -pos->y;
		ma->val[3][2] = -pos->z;
		ma->val[3][3] = 1;
		ma->val[2][2] = 1;
		ma->val[1][1] = 1;
		ma->val[0][0] = 1;
	}
	MATRIX4x4* GenerateClipTransformMatrix(MATRIX4x4 * res){ 
		float ar = width / height;
		float tanRes = 1/  tan( fov* 3.141592653 / 360);
	
		res->val[0][0] = tanRes / ar;
		res->val[1][1] = tanRes;
		res->val[2][2] = (-nearZ - farZ) / (nearZ - farZ);
		res->val[3][2] = 2 * farZ*nearZ / (nearZ - farZ);
		res->val[2][3] = -1;
		return res;
	}

}; 