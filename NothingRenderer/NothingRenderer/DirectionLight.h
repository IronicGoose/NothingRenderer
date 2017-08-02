#pragma once
#include"Vector.h"
#include"ObjectInfo.h"
#include<math.h>
#include<mutex>
using namespace VECTOR;
using namespace MATRIX;
class DirectionLight
{
public:
	DirectionLight();
	~DirectionLight();
	mutex m_Locker;
	VECTOR4* position; 
	VECTOR4 rotation; 
	float* m_shadowMap;
	float nearZ, farZ; 
	float width, height;
	float orthoLeft, orthoRight, orthoTop, orthoBottom;

	MatrixStore store;
	void SetWidthHeight(float wid, float hei) {
		width = wid;
		height = hei;
		m_shadowMap = new float[width * height];
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

	void GenerateLightMatrix(VECTOR4* pos, MATRIX4x4* ma) {
		ma->val[3][0] = -pos->x;
		ma->val[3][1] = -pos->y;
		ma->val[3][2] = -pos->z;
		ma->val[3][3] = 1;
		ma->val[2][2] = 1;
		ma->val[1][1] = 1;
		ma->val[0][0] = 1;
	} 
	float ReadShadowMap(int x, int y) {

		int index = y* width + x;
		return m_shadowMap[index];
	}
	void GetLightCoordinateTransformVert(Object* vs, bool normalConvert = false) {
		GenerateCamMatrix(position, &store.W2CamM);
		for (int i = 0; i < vs->vertCount; i++) {
			matrixdot(vs->verts[i].position, vs->verts[i].position, &store.W2CamM);
			//matrixdot(vs->verts[i].position, vs->verts[i].position, &store.W2CamR);
			matrixdot(vs->verts[i].normal, vs->verts[i].normal, &store.W2CamR);
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
	void GetClipSpaceTransfromVert(Object* vs ) {
		GenerateOrthoMatrix(store.C2OthroM);
		float halfw = width / 2, halfh = height / 2;
		for (int i = 0; i < vs->vertCount; i++) {
			matrixdot(vs->verts[i].position, vs->verts[i].position, &store.C2OthroM);
			vs->verts[i].position->x = halfw* (1 + vs->verts[i].position->x / vs->verts[i].position->w);
			vs->verts[i].position->y = halfh* (1 + vs->verts[i].position->y / vs->verts[i].position->w);
			vs->verts[i].position->x = (halfw* (1 + vs->verts[i].position->x / vs->verts[i].position->w));
			vs->verts[i].position->y = (halfh* (1 + vs->verts[i].position->y / vs->verts[i].position->w));
			vs->verts[i].position->z = vs->verts[i].position->w;
			vs->verts[i].position->w = 1 / vs->verts[i].position->w; 
		}
	}
	void GenerateOrthoMatrix(MATRIX4x4& res) {
		res.val[0][0] = 2 / (orthoRight - orthoLeft);
		res.val[1][1] = 2 / (orthoTop - orthoBottom);
		res.val[2][2] = 2 / (farZ - nearZ);
		res.val[0][3] = -(orthoLeft + orthoRight) / (orthoRight - orthoLeft);
		res.val[1][3] = -(orthoTop + orthoBottom) / (orthoTop - orthoBottom);
		res.val[2][3] = (farZ + nearZ) / (farZ - nearZ);
		res.val[3][3] = 1;

	}
	void ClearShadowMap() {
		for (int i = 0; i < width * height - 1; i++) { 
			m_shadowMap[i] = -99;
		} 
	} 
	void WriteToShadowMap(float depth,int x, int y) {
		m_Locker.lock();
		int index = y* width + x; 
		m_shadowMap[index] = depth;
		m_Locker.unlock();
	}
};

DirectionLight::DirectionLight()
{
	position = new VECTOR4(); 
	position->x = 0; position->y = 1; position->z = -3; position->w = 1;  
	SetRotation(1, 0, 0, 0); 
	nearZ = 0.3f;
	farZ = 20;
	int halfLen = 2; 
	orthoLeft = -halfLen; orthoRight = halfLen;
	orthoBottom = -halfLen; orthoTop = halfLen;
}

DirectionLight::~DirectionLight()
{
}