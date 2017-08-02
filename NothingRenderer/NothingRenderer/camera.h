#pragma once

#include"Vector.h"
#include<math.h>
using namespace VECTOR;
class Camera {
public :


	VECTOR4* position;
	VECTOR4* direction;
	VECTOR4 rotation;
	VECTOR4 u,v,n;
	MATRIX4x4 rot;
	float nearZ, farZ;
	float fov;
	float width, height;
	 
	void SetWidthHeight(float wid, float hei ) {
		width = wid;
		height = hei;
	}
	void SetRotation(float x, float y, float z, float w) {
		w = (-w + 360);
		while (w < 0)
			w += 360;
		while (w > 360)
			w -= 360;
		float qr = cos(w * 3.1415926 / 360);
		float s = sin(w * 3.1415926 / 360);
		rotation.w = qr; rotation.x = x * s; rotation.y = y * s; rotation.z = z *s;
		normalizedVector4(&rotation);
	}
	void SetDirection(VECTOR4* dir) {
		direction = normalizedVector3(direction,dir); 
	}
	void SetPosition(float x, float y, float z) {
		position->x = x; position->y = y; position->z = z;
	}
	void GetClipSpaceWidthHeight(VECTOR2 *res ) {
		res->y = tan(fov* 3.141592653 / 360) *nearZ;
		res->x = (width / height)* res->y; 
	}
	Camera() {
		position = new VECTOR4();
		direction = new VECTOR4();  

		position->x = 0; position->y =  0 ; position->z = -10; position->w = 1;
		direction->x = 0; direction->y = 0; direction->z = 1; direction->w = 0;  

		SetRotation(1, 0, 0, 0);
		fov = 60;
		nearZ = 0.3f;
		farZ = 4000;
	}
	void SelfRotateApply() { 
		GenerateRotateMatrix(rotation, &rot);
		v.x = 0; v.y = 1; v.z = 0;
		n.x = 0; n.y = 0; n.z = 1;
		u.x = 1; u.y = 0; u.z = 0;
		matrixdot(&v, &v, &rot);
		matrixdot(&n, &n, &rot);
		matrixdot(&u, &u, &rot);
	}
	void GetCamCoordinateTransformVert(Object* vs, MatrixStore& store,bool normalConvert = false) { 
		SelfRotateApply();
		GenerateCamMatrix(position, &store.W2CamM);
		for (int i = 0; i < vs->vertCount; i++) {
			matrixdot(vs->verts[i].position, vs->verts[i].position, &store.W2CamM);  
		} 
	}
	void GetClipSpaceTransfromVert(Object* vs,MatrixStore& store) { 
		GenerateClipTransformMatrix( &store.C2OthroM); 
		float halfw = width / 2, halfh = height / 2;
		for (int i = 0; i < vs->vertCount; i++) { 
			matrixdot(vs->verts[i].position, vs->verts[i].position,& store.C2OthroM); 
			vs->verts[i].position->x = (halfw* (1 + vs->verts[i].position->x / vs->verts[i].position->w) );
			vs->verts[i].position->y = (halfh* (1 + vs->verts[i].position->y / vs->verts[i].position->w) );
			vs->verts[i].position->z = vs->verts[i].position->w;
			vs->verts[i].position->w = 1 / vs->verts[i].position->w; 
		} 
	} 
	void GenerateCamMatrix(VECTOR4* pos, MATRIX4x4* ma) {

		normalizedVector3(&u, &u);
		normalizedVector3(&v, &v);
		normalizedVector3(&n, &n);
		ma->val[3][0] = -pos->x *u.x - pos->y*u.y - pos->z *u.z;
		ma->val[3][1] = -pos->x *v.x - pos->y*v.y - pos->z *v.z;
		ma->val[3][2] = -pos->x *n.x - pos->y*n.y - pos->z *n.z;
		ma->val[0][0] = u.x;
		ma->val[1][0] = u.y;
		ma->val[2][0] = u.z;
		ma->val[0][1] = v.x;
		ma->val[1][1] = v.y;
		ma->val[2][1] = v.z;
		ma->val[0][2] = n.x;
		ma->val[1][2] = n.y;
		ma->val[2][2] = n.z;
		ma->val[3][3] = 1;
	}
	MATRIX4x4* GenerateClipTransformMatrix(MATRIX4x4 * res){ 
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